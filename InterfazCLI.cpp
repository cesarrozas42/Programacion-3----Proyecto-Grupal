#include "InterfazCLI.h"

#include <algorithm>
#include <cctype>
#include <iostream>
#include <utility>

using namespace std;

namespace {
constexpr size_t TAMANO_PAGINA = 5;

string aMinusculas(string texto) {
    transform(texto.begin(), texto.end(), texto.begin(), [](unsigned char c) {
        return static_cast<char>(tolower(c));
    });
    return texto;
}
}

InterfazCLI::InterfazCLI(const vector<Pelicula>& catalogo,
                         const Relevancia& relevancia,
                         const Recomendador& recomendador,
                         const MotorDeBusqueda& motor,
                         Usuario& usuario,
                         string archivoEstado)
    : catalogo(catalogo), relevancia(relevancia), recomendador(recomendador),
      motor(motor), usuario(usuario), archivoEstado(std::move(archivoEstado)) {}

string InterfazCLI::leerLinea(const string& mensaje) {
    cout << mensaje;
    string entrada;
    getline(cin, entrada);
    return entrada;
}

int InterfazCLI::leerEntero(const string& texto, int minimo, int maximo) {
    try {
        size_t leidos = 0;
        int valor = stoi(texto, &leidos);
        if (leidos == texto.size() && valor >= minimo && valor <= maximo) return valor;
    } catch (...) {
    }
    return -1;
}

void InterfazCLI::pausar() {
    leerLinea("\nPresiona ENTER para continuar...");
}

bool InterfazCLI::idValido(int idPelicula) const {
    return idPelicula >= 0 && idPelicula < static_cast<int>(catalogo.size());
}

void InterfazCLI::guardarEstado() const {
    usuario.guardar(archivoEstado);
}

void InterfazCLI::mostrarEncabezado() const {
    cout << "\n============================================================\n"
         << "                    CINESEARCH\n"
         << "       Busqueda y recomendaciones de peliculas\n"
         << "============================================================\n";
}

void InterfazCLI::mostrarResumen(int idPelicula, int numero) const {
    if (!idValido(idPelicula)) return;
    const Pelicula& pelicula = catalogo[idPelicula];
    cout << (numero > 0 ? "  " + to_string(numero) + ". " : "  - ")
         << pelicula.getTitulo() << " (" << pelicula.getAnio() << ")";
    if (!pelicula.getGenero().empty()) cout << " | " << pelicula.getGenero();
    cout << "\n";
}

void InterfazCLI::mostrarColeccion(const string& titulo,
                                   const vector<int>& ids,
                                   size_t limite) const {
    cout << "\n" << titulo << "\n" << string(titulo.size(), '-') << "\n";
    size_t mostradas = 0;
    for (int id : ids) {
        if (idValido(id) && mostradas < limite) {
            mostrarResumen(id);
            ++mostradas;
        }
    }
    if (mostradas == 0) cout << "  No hay peliculas para mostrar.\n";
    else if (ids.size() > mostradas) cout << "  ... y " << ids.size() - mostradas << " mas.\n";
}

void InterfazCLI::mostrarInicio() const {
    mostrarEncabezado();
    cout << "Bienvenido, " << usuario.getNombre() << ".\n";
    mostrarColeccion("VER MAS TARDE", usuario.getVerMasTarde());
    mostrarColeccion("RECOMENDADAS PARA TI", recomendador.recomendar(usuario, 5));
}

void InterfazCLI::verDetalle(int idPelicula) {
    if (!idValido(idPelicula)) return;
    while (true) {
        const Pelicula& p = catalogo[idPelicula];
        cout << "\n============================================================\n"
             << p.getTitulo() << " (" << p.getAnio() << ")\n"
             << "============================================================\n"
             << "Origen:   " << p.getOrigen() << "\n"
             << "Director: " << p.getDirector() << "\n"
             << "Reparto:  " << p.getCasting() << "\n"
             << "Genero:   " << p.getGenero() << "\n\n"
             << "SINOPSIS\n" << p.getSinopsis() << "\n\n"
             << "1. " << (usuario.tieneLike(idPelicula) ? "Quitar Like" : "Dar Like") << "\n"
             << "2. " << (usuario.tieneVerMasTarde(idPelicula)
                               ? "Quitar de Ver mas tarde" : "Agregar a Ver mas tarde") << "\n"
             << "0. Volver\n";

        const string opcion = leerLinea("Elige una opcion: ");
        if (opcion == "0") return;
        if (opcion == "1") {
            if (usuario.tieneLike(idPelicula)) {
                usuario.quitarLike(idPelicula);
                cout << "Like eliminado.\n";
            } else {
                usuario.agregarLike(idPelicula);
                cout << "Like agregado.\n";
            }
            guardarEstado();
        } else if (opcion == "2") {
            if (usuario.tieneVerMasTarde(idPelicula)) {
                usuario.quitarVerMasTarde(idPelicula);
                cout << "Pelicula eliminada de Ver mas tarde.\n";
            } else {
                usuario.agregarVerMasTarde(idPelicula);
                cout << "Pelicula agregada a Ver mas tarde.\n";
            }
            guardarEstado();
        } else cout << "Opcion invalida.\n";
    }
}

void InterfazCLI::navegarResultados(const string& titulo, const vector<int>& ids) {
    vector<int> validos;
    copy_if(ids.begin(), ids.end(), back_inserter(validos),
            [this](int id) { return idValido(id); });
    if (validos.empty()) {
        cout << "\nNo se encontraron coincidencias.\n";
        pausar();
        return;
    }

    size_t pagina = 0;
    const size_t totalPaginas = (validos.size() + TAMANO_PAGINA - 1) / TAMANO_PAGINA;
    while (true) {
        const size_t inicio = pagina * TAMANO_PAGINA;
        const size_t fin = min(inicio + TAMANO_PAGINA, validos.size());
        cout << "\n" << titulo << "\nPagina " << pagina + 1 << " de " << totalPaginas
             << " | " << validos.size() << " coincidencias\n"
             << "------------------------------------------------------------\n";
        for (size_t i = inicio; i < fin; ++i)
            mostrarResumen(validos[i], static_cast<int>(i - inicio + 1));

        cout << "\nSelecciona 1-" << fin - inicio << " para ver la sinopsis";
        if (pagina + 1 < totalPaginas) cout << " | N: siguiente";
        if (pagina > 0) cout << " | P: anterior";
        cout << " | 0: volver\n";

        string opcion = aMinusculas(leerLinea("Opcion: "));
        if (opcion == "0") return;
        if (opcion == "n" && pagina + 1 < totalPaginas) {
            ++pagina;
            continue;
        }
        if (opcion == "p" && pagina > 0) {
            --pagina;
            continue;
        }
        int seleccion = leerEntero(opcion, 1, static_cast<int>(fin - inicio));
        if (seleccion != -1) verDetalle(validos[inicio + seleccion - 1]);
        else cout << "Opcion invalida.\n";
    }
}

void InterfazCLI::buscarPorTexto() {
    const string consulta = leerLinea("\nPalabra, frase o sub-palabra: ");
    if (consulta.find_first_not_of(" \t") == string::npos) {
        cout << "La consulta no puede estar vacia.\n";
        pausar();
        return;
    }
    vector<ResultadoBusqueda> resultados = relevancia.buscar(consulta);
    vector<int> ids;
    ids.reserve(resultados.size());
    for (const auto& resultado : resultados) ids.push_back(resultado.idPelicula);
    navegarResultados("RESULTADOS PARA: " + consulta, ids);
}

void InterfazCLI::buscarPorTag(const string& tipo) {
    const string valor = leerLinea("\nValor de " + tipo + ": ");
    if (valor.find_first_not_of(" \t") == string::npos) {
        cout << "El valor no puede estar vacio.\n";
        pausar();
        return;
    }
    vector<int> ids;
    if (tipo == "director") ids = motor.buscarPorDirector(valor);
    else if (tipo == "genero") ids = motor.buscarPorGenero(valor);
    else ids = motor.buscarPorActor(valor);
    navegarResultados("RESULTADOS POR " + tipo + ": " + valor, ids);
}

void InterfazCLI::mostrarRecomendadas() {
    navegarResultados("RECOMENDADAS PARA TI", recomendador.recomendar(usuario, 25));
}

void InterfazCLI::mostrarVerMasTarde() {
    navegarResultados("TU LISTA: VER MAS TARDE", usuario.getVerMasTarde());
}

void InterfazCLI::ejecutar() {
    mostrarInicio();
    while (true) {
        cout << "\nMENU PRINCIPAL\n"
             << "1. Buscar por titulo o sinopsis\n"
             << "2. Buscar por director\n"
             << "3. Buscar por genero\n"
             << "4. Buscar por actor o actriz\n"
             << "5. Ver recomendaciones\n"
             << "6. Ver lista Ver mas tarde\n"
             << "0. Salir\n";
        const string opcion = leerLinea("Elige una opcion: ");
        if (opcion == "1") buscarPorTexto();
        else if (opcion == "2") buscarPorTag("director");
        else if (opcion == "3") buscarPorTag("genero");
        else if (opcion == "4") buscarPorTag("actor");
        else if (opcion == "5") mostrarRecomendadas();
        else if (opcion == "6") mostrarVerMasTarde();
        else if (opcion == "0") {
            guardarEstado();
            cout << "\nTu perfil fue guardado. Hasta luego!\n";
            return;
        } else cout << "Opcion invalida. Intenta nuevamente.\n";
    }
}
