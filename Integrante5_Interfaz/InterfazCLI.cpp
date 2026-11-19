#include "InterfazCLI.h"

#include <algorithm>
#include <cctype>
#include <chrono>
#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <sstream>
#include <utility>

using namespace std;

namespace {

constexpr size_t TAMANO_PAGINA = 5;   // "las cinco mas importantes" (requisito de la parte 1)
constexpr size_t ANCHO = 60;          // mismo ancho de las lineas de la parte 1
constexpr size_t MAX_SUGERENCIAS = 8;

// ---------- Colores ANSI (la consola de CLion los muestra) ----------
// Para desactivarlos: definir la variable de entorno NO_COLOR.
bool coloresActivos() {
    static const bool activos = getenv("NO_COLOR") == nullptr;
    return activos;
}
string c(const char* codigo) { return coloresActivos() ? string(codigo) : string(); }
string RESET() { return c("\033[0m"); }
string NEGRITA() { return c("\033[1m"); }
string CIAN() { return c("\033[36m"); }
string AMARILLO() { return c("\033[33m"); }
string VERDE() { return c("\033[32m"); }
string ROJO() { return c("\033[31m"); }
string GRIS() { return c("\033[90m"); }

using Reloj = chrono::steady_clock;
double msDesde(Reloj::time_point inicio) {
    return chrono::duration<double, milli>(Reloj::now() - inicio).count();
}

string formatoTiempo(double ms) {
    char buffer[32];
    if (ms >= 1000.0) snprintf(buffer, sizeof(buffer), "%.2f s", ms / 1000.0);
    else snprintf(buffer, sizeof(buffer), "%.2f ms", ms);
    return buffer;
}

string conMiles(size_t numero) {
    string texto = to_string(numero);
    for (int i = static_cast<int>(texto.size()) - 3; i > 0; i -= 3) texto.insert(i, ",");
    return texto;
}

// ---------- Texto UTF-8 (los titulos pueden traer tildes) ----------
bool esContinuacionUtf8(unsigned char c) { return (c & 0xC0) == 0x80; }

size_t largoVisible(const string& texto) {
    size_t largo = 0;
    for (unsigned char c : texto)
        if (!esContinuacionUtf8(c)) ++largo;
    return largo;
}

// Recorta a "ancho" caracteres (con "...") y rellena con espacios.
string ajustar(const string& texto, size_t ancho) {
    string salida;
    if (largoVisible(texto) <= ancho) {
        salida = texto;
    } else {
        size_t visibles = 0, i = 0;
        while (i < texto.size() && visibles < ancho - 3) {
            ++i;
            while (i < texto.size() && esContinuacionUtf8(static_cast<unsigned char>(texto[i]))) ++i;
            ++visibles;
        }
        salida = texto.substr(0, i) + "...";
    }
    return salida + string(ancho - min(ancho, largoVisible(salida)), ' ');
}

string centrar(const string& texto, size_t ancho) {
    const size_t largo = largoVisible(texto);
    if (largo >= ancho) return texto;
    return string((ancho - largo) / 2, ' ') + texto;
}

// Parte un texto largo en lineas sin cortar palabras.
vector<string> envolver(const string& texto, size_t ancho) {
    vector<string> lineas;
    istringstream entrada(texto);
    string palabra, actual;
    while (entrada >> palabra) {
        if (!actual.empty() && largoVisible(actual) + 1 + largoVisible(palabra) > ancho) {
            lineas.push_back(actual);
            actual.clear();
        }
        actual += (actual.empty() ? "" : " ") + palabra;
    }
    if (!actual.empty()) lineas.push_back(actual);
    return lineas;
}

string minusculas(string texto) {
    for (char& c : texto) c = static_cast<char>(tolower(static_cast<unsigned char>(c)));
    return texto;
}

// Misma normalizacion que el Trie: minusculas y solo letras/numeros.
vector<string> palabrasDe(const string& texto) {
    vector<string> palabras;
    string palabra;
    for (unsigned char c : texto) {
        if (isalnum(c)) {
            palabra += static_cast<char>(tolower(c));
        } else if (!palabra.empty()) {
            palabras.push_back(palabra);
            palabra.clear();
        }
    }
    if (!palabra.empty()) palabras.push_back(palabra);
    return palabras;
}

string recortarEspacios(const string& texto) {
    const size_t inicio = texto.find_first_not_of(" \t\r\n");
    if (inicio == string::npos) return "";
    const size_t fin = texto.find_last_not_of(" \t\r\n");
    return texto.substr(inicio, fin - inicio + 1);
}

string lineaDoble() { return string(ANCHO, '='); }
string lineaSimple() { return string(ANCHO, '-'); }
string etiquetaNueva() { return GRIS() + "  (nuevo)" + RESET(); }

} // namespace

// =====================================================================
// Construccion y entrada
// =====================================================================

InterfazCLI::InterfazCLI(const vector<Pelicula>& catalogo,
                         const Indexador& indexador,
                         const Relevancia& relevancia,
                         const Recomendador& recomendador,
                         Usuario& usuario,
                         string archivoEstado,
                         TiemposCarga tiemposCarga)
    : catalogo(catalogo), indexador(indexador), relevancia(relevancia),
      recomendador(recomendador), usuario(usuario),
      archivoEstado(std::move(archivoEstado)), tiemposCarga(tiemposCarga) {
    usuario.agregarObservador(this);  // OBSERVER: el menu se suscribe al Usuario
}

InterfazCLI::~InterfazCLI() {
    usuario.quitarObservador(this);
}

// OBSERVER: aviso del Usuario. El menu muestra el cambio y guarda el estado
// (antes, en la parte 1, el menu tenia que acordarse de guardar tras cada opcion).
void InterfazCLI::actualizar(const Usuario&, TipoEventoUsuario evento, int idPelicula) {
    string mensaje;
    switch (evento) {
        case TipoEventoUsuario::LIKE_AGREGADO:           mensaje = "Like agregado. Tus recomendaciones se actualizaron."; break;
        case TipoEventoUsuario::LIKE_ELIMINADO:          mensaje = "Like eliminado."; break;
        case TipoEventoUsuario::VER_MAS_TARDE_AGREGADO:  mensaje = "Pelicula agregada a Ver mas tarde."; break;
        case TipoEventoUsuario::VER_MAS_TARDE_ELIMINADO: mensaje = "Pelicula eliminada de Ver mas tarde."; break;
    }
    mostrarAviso(mensaje);
    if (idValido(idPelicula))
        cout << GRIS() << "[Observer] El Usuario aviso del cambio en \"" << catalogo[idPelicula].getTitulo()
             << "\"; estado guardado en " << archivoEstado << "." << RESET() << "\n";
    guardarEstado();
}

// =====================================================================
// STRATEGY: como se busca (integrante 4)
// =====================================================================

string InterfazCLI::nombreModo() const {
    switch (modo) {
        case ModoBusqueda::ARBOL:      return "Arbol";
        case ModoBusqueda::SECUENCIAL: return "Secuencial";
        case ModoBusqueda::PARALELA:   return "Paralela";
    }
    return "";
}

void InterfazCLI::cambiarModo() {
    cout << "\n" << NEGRITA() << "MODO DE BUSQUEDA (Strategy)" << RESET() << "\n"
         << "Actual: " << VERDE() << nombreModo() << RESET() << "\n"
         << CIAN() << "1." << RESET() << " Arbol       " << GRIS() << "baja por el Trie de palabras / sufijos (parte 2)" << RESET() << "\n"
         << CIAN() << "2." << RESET() << " Secuencial  " << GRIS() << "revisa las peliculas una por una" << RESET() << "\n"
         << CIAN() << "3." << RESET() << " Paralela    " << GRIS() << "reparte el catalogo entre hilos" << RESET() << "\n"
         << CIAN() << "0." << RESET() << " Volver\n";
    const string opcion = leerLinea("Elige una opcion: ");
    if (opcion == "1") modo = ModoBusqueda::ARBOL;
    else if (opcion == "2") modo = ModoBusqueda::SECUENCIAL;
    else if (opcion == "3") modo = ModoBusqueda::PARALELA;
    else return;
    mostrarAviso("Ahora las busquedas usan el modo " + nombreModo() + ". Compara los tiempos con la opcion 8.");
}

string InterfazCLI::leerLinea(const string& mensaje) {
    cout << mensaje << flush;
    string entrada;
    if (!getline(cin, entrada)) {
        entradaTerminada = true;  // se cerro la entrada: salir sin quedarse en bucle
        return "";
    }
    if (entrada.rfind("\xEF\xBB\xBF", 0) == 0) entrada.erase(0, 3);  // marca BOM de algunas consolas
    return recortarEspacios(entrada);
}

int InterfazCLI::leerEntero(const string& texto, int minimo, int maximo) {
    try {
        size_t leidos = 0;
        const int valor = stoi(texto, &leidos);
        if (leidos == texto.size() && valor >= minimo && valor <= maximo) return valor;
    } catch (...) {
    }
    return -1;
}

void InterfazCLI::pausar() {
    if (!entradaTerminada) leerLinea("\nPresiona ENTER para continuar...");
}

// =====================================================================
// Utilidades de pantalla
// =====================================================================

bool InterfazCLI::idValido(int idPelicula) const {
    return idPelicula >= 0 && idPelicula < static_cast<int>(catalogo.size());
}

void InterfazCLI::guardarEstado() const {
    usuario.guardar(archivoEstado);
}

// [L] = tiene Like, [V] = esta en Ver mas tarde
string InterfazCLI::marcas(int idPelicula) const {
    string texto;
    if (usuario.tieneLike(idPelicula)) texto += " " + ROJO() + "[L]" + RESET();
    if (usuario.tieneVerMasTarde(idPelicula)) texto += " " + AMARILLO() + "[V]" + RESET();
    return texto;
}

void InterfazCLI::mostrarAviso(const string& texto, bool ok) const {
    cout << (ok ? VERDE() : ROJO()) << texto << RESET() << "\n";
}

void InterfazCLI::mostrarEncabezado() const {
    cout << "\n" << CIAN() << lineaDoble() << "\n" << RESET()
         << NEGRITA() << centrar("CINESEARCH", ANCHO) << RESET() << "\n"
         << centrar("Busqueda y recomendaciones de peliculas", ANCHO) << "\n"
         << GRIS() << centrar("Parte 2: arbol de palabras + arbol de sufijos", ANCHO) << RESET() << "\n"
         << CIAN() << lineaDoble() << RESET() << "\n";
}

// "  1. Titulo (anio) | genero [L] [V]"  (mismo formato de la parte 1)
void InterfazCLI::mostrarResumen(int idPelicula, int numero, const string& extra) const {
    if (!idValido(idPelicula)) return;
    const Pelicula& pelicula = catalogo[idPelicula];
    cout << (numero > 0 ? "  " + CIAN() + to_string(numero) + "." + RESET() + " " : "  - ")
         << pelicula.getTitulo() << " (" << pelicula.getAnio() << ")";
    if (!pelicula.getGenero().empty()) cout << GRIS() << " | " << pelicula.getGenero() << RESET();
    cout << marcas(idPelicula) << extra << "\n";
}

void InterfazCLI::mostrarColeccion(const string& titulo, const vector<int>& ids, size_t limite) const {
    cout << "\n" << NEGRITA() << titulo << RESET() << "\n" << string(titulo.size(), '-') << "\n";
    size_t mostradas = 0;
    for (int id : ids) {
        if (idValido(id) && mostradas < limite) {
            mostrarResumen(id);
            ++mostradas;
        }
    }
    if (mostradas == 0) cout << GRIS() << "  No hay peliculas para mostrar." << RESET() << "\n";
    else if (ids.size() > mostradas) cout << GRIS() << "  ... y " << ids.size() - mostradas << " mas." << RESET() << "\n";
}

void InterfazCLI::mostrarInicio() const {
    mostrarEncabezado();
    cout << "Bienvenido, " << NEGRITA() << usuario.getNombre() << RESET() << ".\n"
         << GRIS() << "Catalogo de " << conMiles(catalogo.size()) << " peliculas listo en "
         << formatoTiempo(tiemposCarga.msLecturaCSV + tiemposCarga.msIndice)
         << " (CSV " << formatoTiempo(tiemposCarga.msLecturaCSV)
         << " + indice " << formatoTiempo(tiemposCarga.msIndice) << ")." << RESET() << "\n";
    mostrarColeccion("VER MAS TARDE", usuario.getVerMasTarde());
    mostrarColeccion("RECOMENDADAS PARA TI", recomendador.recomendar(usuario, 5));
}

// =====================================================================
// Detalle de una pelicula (sinopsis, Like y Ver mas tarde)
// =====================================================================

void InterfazCLI::verDetalle(int idPelicula) {
    if (!idValido(idPelicula)) return;
    while (!entradaTerminada) {
        const Pelicula& p = catalogo[idPelicula];
        const bool tieneLike = usuario.tieneLike(idPelicula);
        const bool enLista = usuario.tieneVerMasTarde(idPelicula);

        cout << "\n" << CIAN() << lineaDoble() << RESET() << "\n"
             << NEGRITA() << p.getTitulo() << " (" << p.getAnio() << ")" << RESET() << marcas(idPelicula) << "\n"
             << CIAN() << lineaDoble() << RESET() << "\n"
             << GRIS() << "Origen:   " << RESET() << p.getOrigen() << "\n"
             << GRIS() << "Director: " << RESET() << p.getDirector() << "\n";
        // El reparto puede ser largo: se parte en lineas
        const vector<string> reparto = envolver(p.getCasting().empty() ? "-" : p.getCasting(), ANCHO - 10);
        for (size_t i = 0; i < reparto.size(); ++i)
            cout << GRIS() << (i == 0 ? "Reparto:  " : "          ") << RESET() << reparto[i] << "\n";
        cout << GRIS() << "Genero:   " << RESET() << p.getGenero() << "\n\n"
             << NEGRITA() << "SINOPSIS" << RESET() << "\n";
        const vector<string> sinopsis = envolver(p.getSinopsis(), ANCHO);
        if (sinopsis.empty()) cout << GRIS() << "(Sin sinopsis en el dataset)" << RESET() << "\n";
        for (const string& linea : sinopsis) cout << linea << "\n";
        cout << "\n"
             << CIAN() << "1." << RESET() << " " << (tieneLike ? "Quitar Like" : "Dar Like") << "\n"
             << CIAN() << "2." << RESET() << " " << (enLista ? "Quitar de Ver mas tarde" : "Agregar a Ver mas tarde") << "\n"
             << CIAN() << "0." << RESET() << " Volver\n";

        const string opcion = leerLinea("Elige una opcion: ");
        if (entradaTerminada || opcion == "0") return;
        // El aviso y el guardado los hace actualizar() (Observer)
        if (opcion == "1") {
            if (tieneLike) usuario.quitarLike(idPelicula);
            else usuario.agregarLike(idPelicula);
        } else if (opcion == "2") {
            if (enLista) usuario.quitarVerMasTarde(idPelicula);
            else usuario.agregarVerMasTarde(idPelicula);
        } else {
            mostrarAviso("Opcion invalida.", false);
        }
    }
}

// =====================================================================
// Resultados de 5 en 5 (parte 1) + tiempo de busqueda (parte 2)
// =====================================================================

void InterfazCLI::navegarResultados(const string& titulo, vector<int> ids,
                                    const vector<double>& puntajes, double ms,
                                    const function<vector<int>()>& recargar) {
    size_t pagina = 0;
    while (!entradaTerminada) {
        vector<int> validos;
        vector<double> puntos;
        for (size_t i = 0; i < ids.size(); ++i) {
            if (!idValido(ids[i])) continue;
            validos.push_back(ids[i]);
            if (i < puntajes.size()) puntos.push_back(puntajes[i]);
        }
        if (validos.empty()) {
            cout << "\nNo se encontraron coincidencias.\n";
            pausar();
            return;
        }

        const size_t totalPaginas = (validos.size() + TAMANO_PAGINA - 1) / TAMANO_PAGINA;
        pagina = min(pagina, totalPaginas - 1);
        const size_t inicio = pagina * TAMANO_PAGINA;
        const size_t fin = min(inicio + TAMANO_PAGINA, validos.size());

        cout << "\n" << NEGRITA() << titulo << RESET() << "\n"
             << "Pagina " << pagina + 1 << " de " << totalPaginas
             << " | " << conMiles(validos.size()) << " coincidencias";
        if (ms >= 0.0) cout << " | " << VERDE() << "busqueda en " << formatoTiempo(ms) << RESET();
        cout << "\n" << lineaSimple() << "\n";
        for (size_t i = inicio; i < fin; ++i) {
            string extra;
            if (i < puntos.size()) {
                char buffer[32];
                snprintf(buffer, sizeof(buffer), " (relevancia %.0f)", puntos[i]);
                extra = GRIS() + buffer + RESET();
            }
            mostrarResumen(validos[i], static_cast<int>(i - inicio + 1), extra);
        }

        cout << "\nSelecciona " << CIAN() << "1-" << fin - inicio << RESET() << " para ver la sinopsis";
        if (pagina + 1 < totalPaginas) cout << " | " << CIAN() << "N" << RESET() << ": siguiente";
        if (pagina > 0) cout << " | " << CIAN() << "P" << RESET() << ": anterior";
        cout << " | " << CIAN() << "0" << RESET() << ": volver\n";

        const string opcion = minusculas(leerLinea("Opcion: "));
        if (entradaTerminada || opcion == "0") return;
        if (opcion == "n" && pagina + 1 < totalPaginas) {
            ++pagina;
            continue;
        }
        if (opcion == "p" && pagina > 0) {
            --pagina;
            continue;
        }
        const int seleccion = leerEntero(opcion, 1, static_cast<int>(fin - inicio));
        if (seleccion == -1) {
            mostrarAviso("Opcion invalida.", false);
            continue;
        }
        verDetalle(validos[inicio + seleccion - 1]);
        if (recargar) ids = recargar();  // la lista pudo cambiar en el detalle
    }
}

// =====================================================================
// Busquedas
// =====================================================================

// Parte 2: si la ultima palabra esta incompleta, el arbol sugiere como
// completarla (de la palabra que aparece en mas peliculas a la que menos).
string InterfazCLI::pedirConAutocompletado() {
    cout << GRIS() << "\nTip: escribe el inicio de una palabra y el arbol te sugiere como completarla." << RESET();
    const string texto = leerLinea("\nPalabra, frase o sub-palabra: ");
    vector<string> palabras = palabrasDe(texto);
    if (palabras.empty()) return "";

    const string ultima = palabras.back();
    if (ultima.size() >= 2) {
        const auto inicio = Reloj::now();
        vector<string> sugerencias = indexador.autocompletar(ultima, MAX_SUGERENCIAS);
        const double ms = msDesde(inicio);
        sugerencias.erase(remove(sugerencias.begin(), sugerencias.end(), ultima), sugerencias.end());

        if (!sugerencias.empty()) {
            cout << "\n" << NEGRITA() << "Autocompletado del arbol para \"" << ultima << "\"" << RESET()
                 << GRIS() << " (" << formatoTiempo(ms) << ")" << RESET() << "\n";
            for (size_t i = 0; i < sugerencias.size(); ++i) {
                cout << "  " << CIAN() << i + 1 << "." << RESET() << " " << ajustar(sugerencias[i], 14);
                if (i % 3 == 2 || i + 1 == sugerencias.size()) cout << "\n";
            }
            const string opcion = leerLinea("Elige un numero para completar o ENTER para buscar \"" + ultima + "\": ");
            const int elegida = leerEntero(opcion, 1, static_cast<int>(sugerencias.size()));
            if (elegida != -1) palabras.back() = sugerencias[elegida - 1];
        }
    }

    string consulta;
    for (const string& palabra : palabras) consulta += (consulta.empty() ? "" : " ") + palabra;
    return consulta;
}

void InterfazCLI::buscarPorTexto() {
    const string consulta = pedirConAutocompletado();
    if (consulta.empty()) {
        if (!entradaTerminada) {
            cout << "La consulta no puede estar vacia.\n";
            pausar();
        }
        return;
    }

    vector<int> ids;
    vector<double> puntajes;
    const auto inicio = Reloj::now();
    if (modo == ModoBusqueda::ARBOL) {
        // Arbol + Relevancia (titulo pesa mas que sinopsis, bonus por frase completa)
        for (const ResultadoBusqueda& r : relevancia.buscar(consulta)) {
            ids.push_back(r.idPelicula);
            puntajes.push_back(r.score);
        }
    } else {
        // Secuencial o Paralela: la estrategia revisa el texto de cada pelicula
        const EstrategiaConcretaBusqueda estrategia(catalogo, indexador, TipoBusqueda::SUBCADENA, modo);
        ids = estrategia.buscar(consulta);
    }
    const double ms = msDesde(inicio);

    registrar("Texto [" + nombreModo() + "]", consulta, ids.size(), ms);
    navegarResultados("RESULTADOS PARA: " + consulta + "  (modo " + nombreModo() + ")", ids, puntajes, ms);
}

void InterfazCLI::buscarPorTag(const string& tipo) {
    const string valor = leerLinea("\nValor de " + tipo + ": ");
    if (palabrasDe(valor).empty()) {
        if (!entradaTerminada) {
            cout << "El valor no puede estar vacio.\n";
            pausar();
        }
        return;
    }

    TipoBusqueda tipoBusqueda = TipoBusqueda::ACTOR;
    if (tipo == "director") tipoBusqueda = TipoBusqueda::DIRECTOR;
    else if (tipo == "genero") tipoBusqueda = TipoBusqueda::GENERO;

    // STRATEGY: la misma busqueda con el modo elegido (Arbol / Secuencial / Paralela)
    const EstrategiaConcretaBusqueda estrategia(catalogo, indexador, tipoBusqueda, modo);
    const auto inicio = Reloj::now();
    const vector<int> ids = estrategia.buscar(valor);
    const double ms = msDesde(inicio);

    registrar(tipo + " [" + nombreModo() + "]", valor, ids.size(), ms);
    string tipoMayus = tipo;
    for (char& ch : tipoMayus) ch = static_cast<char>(toupper(static_cast<unsigned char>(ch)));
    navegarResultados("RESULTADOS POR " + tipoMayus + ": " + valor + "  (modo " + nombreModo() + ")", ids, {}, ms);
}

void InterfazCLI::mostrarRecomendadas() {
    const auto inicio = Reloj::now();
    const vector<int> ids = recomendador.recomendar(usuario, 25);
    const double ms = msDesde(inicio);
    if (ids.empty()) {
        cout << "\nDale Like a alguna pelicula para recibir recomendaciones.\n";
        pausar();
        return;
    }
    registrar("Recomendaciones", "segun tus Likes", ids.size(), ms);
    navegarResultados("RECOMENDADAS PARA TI", ids, {}, ms);
}

void InterfazCLI::mostrarVerMasTarde() {
    if (usuario.getVerMasTarde().empty()) {
        cout << "\nTu lista Ver mas tarde esta vacia.\n";
        pausar();
        return;
    }
    navegarResultados("TU LISTA: VER MAS TARDE", usuario.getVerMasTarde(), {}, -1.0,
                      [this]() { return usuario.getVerMasTarde(); });
}

void InterfazCLI::mostrarLikes() {
    if (usuario.getLikes().empty()) {
        cout << "\nTodavia no le diste Like a ninguna pelicula.\n";
        pausar();
        return;
    }
    navegarResultados("TUS LIKES", usuario.getLikes(), {}, -1.0,
                      [this]() { return usuario.getLikes(); });
}

// =====================================================================
// Tiempos medidos (parte 2)
// =====================================================================

void InterfazCLI::registrar(const string& tipo, const string& consulta, size_t resultados, double ms) {
    historial.push_back({tipo, consulta, resultados, ms});
}

void InterfazCLI::mostrarTiempos() const {
    cout << "\n" << NEGRITA() << "TIEMPOS MEDIDOS" << RESET() << "\n" << lineaSimple() << "\n"
         << "Lectura del CSV:          " << VERDE() << formatoTiempo(tiemposCarga.msLecturaCSV) << RESET() << "\n"
         << "Construccion del indice:  " << VERDE() << formatoTiempo(tiemposCarga.msIndice) << RESET() << "\n"
         << lineaSimple() << "\n";
    if (historial.empty()) {
        cout << GRIS() << "Todavia no hiciste busquedas en esta sesion." << RESET() << "\n";
        return;
    }
    cout << GRIS() << ajustar("#", 4) << ajustar("BUSQUEDA", 17) << ajustar("CONSULTA", 18)
         << ajustar("RESULT.", 9) << "TIEMPO" << RESET() << "\n";
    for (size_t i = 0; i < historial.size(); ++i) {
        const RegistroBusqueda& r = historial[i];
        cout << ajustar(to_string(i + 1), 4) << ajustar(r.tipo, 17) << ajustar(r.consulta, 18)
             << ajustar(conMiles(r.resultados), 9) << VERDE() << formatoTiempo(r.ms) << RESET() << "\n";
    }
}

// =====================================================================
// Menu principal (mismas opciones de la parte 1 + las nuevas)
// =====================================================================

void InterfazCLI::ejecutar() {
    mostrarInicio();
    while (!entradaTerminada) {
        cout << "\n" << NEGRITA() << "MENU PRINCIPAL" << RESET() << "\n"
             << GRIS() << "Likes: " << usuario.getLikes().size()
             << " | Ver mas tarde: " << usuario.getVerMasTarde().size()
             << " | Modo de busqueda: " << RESET() << VERDE() << nombreModo() << RESET() << "\n"
             << CIAN() << "1." << RESET() << " Buscar por titulo o sinopsis" << GRIS() << "  (con autocompletado)" << RESET() << "\n"
             << CIAN() << "2." << RESET() << " Buscar por director\n"
             << CIAN() << "3." << RESET() << " Buscar por genero\n"
             << CIAN() << "4." << RESET() << " Buscar por actor o actriz\n"
             << CIAN() << "5." << RESET() << " Ver recomendaciones\n"
             << CIAN() << "6." << RESET() << " Ver lista Ver mas tarde\n"
             << CIAN() << "7." << RESET() << " Ver mis Likes" << etiquetaNueva() << "\n"
             << CIAN() << "8." << RESET() << " Ver tiempos de busqueda" << etiquetaNueva() << "\n"
             << CIAN() << "9." << RESET() << " Cambiar modo de busqueda: Arbol / Secuencial / Paralela" << etiquetaNueva() << "\n"
             << CIAN() << "0." << RESET() << " Salir\n";
        const string opcion = leerLinea("Elige una opcion: ");
        if (entradaTerminada) break;

        if (opcion == "1") buscarPorTexto();
        else if (opcion == "2") buscarPorTag("director");
        else if (opcion == "3") buscarPorTag("genero");
        else if (opcion == "4") buscarPorTag("actor");
        else if (opcion == "5") mostrarRecomendadas();
        else if (opcion == "6") mostrarVerMasTarde();
        else if (opcion == "7") mostrarLikes();
        else if (opcion == "8") {
            mostrarTiempos();
            pausar();
        } else if (opcion == "9") {
            cambiarModo();
        } else if (opcion == "0") {
            break;
        } else {
            mostrarAviso("Opcion invalida. Intenta nuevamente.", false);
        }
    }
    guardarEstado();
    cout << "\nTu perfil fue guardado. Hasta luego!\n";
}
