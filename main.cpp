#include <iostream>
#include <string>
#include <vector>
#include <limits>

#include "CSVReader.h"
#include "Indexador.h"
#include "Motor_de_busqueda.h"
#include "Relevancia.h"
#include "Recomendador.h"
#include "Usuario.h"

using namespace std;

void limpiarBufferEntrada() {
    cin.clear();
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
}

int leerOpcionEntero() {
    int opcion;
    if (!(cin >> opcion)) {
        limpiarBufferEntrada();
        return -1;
    }
    limpiarBufferEntrada();
    return opcion;
}

void mostrarResumen(const vector<Pelicula>& peliculas, int id) {
    if (id < 0 || id >= static_cast<int>(peliculas.size())) {
        cout << "  (id invalido)\n";
        return;
    }
    const Pelicula& p = peliculas[id];
    cout << "  [" << id << "] " << p.getTitulo()
         << " (" << p.getAnio() << ") - " << p.getGenero() << "\n";
}

void mostrarResultados(const vector<Pelicula>& peliculas,
                       const vector<int>& ids) {
    if (ids.empty()) {
        cout << "  Sin resultados.\n";
        return;
    }
    for (int id : ids) {
        mostrarResumen(peliculas, id);
    }
}

void verDetalle(const vector<Pelicula>& peliculas, int id) {
    if (id < 0 || id >= static_cast<int>(peliculas.size())) {
        cout << "Id invalido.\n";
        return;
    }
    const Pelicula& p = peliculas[id];
    cout << "\n--- " << p.getTitulo() << " (" << p.getAnio() << ") ---\n";
    cout << "Director: " << p.getDirector() << "\n";
    cout << "Reparto: " << p.getCasting() << "\n";
    cout << "Genero: " << p.getGenero() << "\n";
    cout << "Sinopsis: " << p.getSinopsis() << "\n\n";
}

void menuAccionesSobreResultados(const vector<Pelicula>& peliculas,
                                 Usuario& usuario) {
    cout << "\n  a) Ver sinopsis de un id\n";
    cout << "  b) Dar like a un id\n";
    cout << "  c) Agregar un id a ver mas tarde\n";
    cout << "  d) Volver al menu principal\n";
    cout << "  Elige una opcion: ";

    string opcion;
    getline(cin, opcion);

    if (opcion == "a" || opcion == "b" || opcion == "c") {
        cout << "  Ingresa el id: ";
        int id = leerOpcionEntero();

        if (opcion == "a") {
            verDetalle(peliculas, id);
        } else if (opcion == "b") {
            if (usuario.agregarLike(id)) {
                cout << "  Like agregado.\n";
            } else {
                cout << "  Ya tenias like en esa pelicula.\n";
            }
        } else if (opcion == "c") {
            if (usuario.agregarVerMasTarde(id)) {
                cout << "  Agregada a ver mas tarde.\n";
            } else {
                cout << "  Ya estaba en tu lista de ver mas tarde.\n";
            }
        }
    }
}

void flujoBusquedaPorTexto(const vector<Pelicula>& peliculas,
                           const Relevancia& relevancia,
                           Usuario& usuario) {
    cout << "Ingresa palabra o frase a buscar: ";
    string consulta;
    getline(cin, consulta);

    vector<ResultadoBusqueda> resultados = relevancia.buscar(consulta);

    if (resultados.empty()) {
        cout << "Sin resultados para \"" << consulta << "\".\n";
        return;
    }

    int pagina = 1;
    while (true) {
        vector<int> idsPagina = relevancia.obtenerPagina(resultados, pagina, 5);

        if (idsPagina.empty()) {
            cout << "No hay mas resultados.\n";
            pagina = 1;
            idsPagina = relevancia.obtenerPagina(resultados, pagina, 5);
        }

        cout << "\nResultados para \"" << consulta << "\" (pagina "
             << pagina << "):\n";
        mostrarResultados(peliculas, idsPagina);

        cout << "\n  [n] siguiente pagina\n";
        cout << "  [m] menu de acciones (ver/like/ver mas tarde)\n";
        cout << "  [s] salir de la busqueda\n";
        cout << "  Elige: ";

        string opcion;
        getline(cin, opcion);

        if (opcion == "n") {
            pagina++;
        } else if (opcion == "m") {
            menuAccionesSobreResultados(peliculas, usuario);
        } else if (opcion == "s") {
            break;
        }
    }
}

void flujoBusquedaPorTag(const vector<Pelicula>& peliculas,
                         const MotorDeBusqueda& motor,
                         Usuario& usuario,
                         const string& tipoTag) {
    cout << "Ingresa " << tipoTag << " a buscar: ";
    string valor;
    getline(cin, valor);

    vector<int> ids;
    if (tipoTag == "director") {
        ids = motor.buscarPorDirector(valor);
    } else if (tipoTag == "genero") {
        ids = motor.buscarPorGenero(valor);
    } else {
        ids = motor.buscarPorActor(valor);
    }

    cout << "\nResultados:\n";
    mostrarResultados(peliculas, ids);

    if (!ids.empty()) {
        menuAccionesSobreResultados(peliculas, usuario);
    }
}

void mostrarRecomendadas(const vector<Pelicula>& peliculas,
                         const Recomendador& recomendador,
                         const Usuario& usuario) {
    vector<int> recomendadas = recomendador.recomendar(usuario, 5);

    cout << "\nRecomendadas para ti:\n";
    if (recomendadas.empty()) {
        cout << "  Dale like a alguna pelicula para recibir recomendaciones.\n";
    } else {
        mostrarResultados(peliculas, recomendadas);
    }
}

void mostrarVerMasTarde(const vector<Pelicula>& peliculas,
                        const Usuario& usuario) {
    vector<int> verMasTarde = usuario.getVerMasTarde();

    cout << "\nTu lista de ver mas tarde:\n";
    mostrarResultados(peliculas, verMasTarde);
}

int main() {
#ifdef _WIN32
    system("chcp 65001 > nul");
#endif

    // 1. Cargar CSV (una sola vez)
    CSVReader lector;
    vector<Pelicula> peliculas =
            lector.leerArchivo("wiki_movie_plots_deduped.csv");

    if (peliculas.empty()) {
        cout << "No se pudieron cargar peliculas. Verifica el CSV.\n";
        return 1;
    }

    // 2. Construir indice (una sola vez)
    Indexador indexador;
    indexador.construirIndice(peliculas);

    // 3. Crear objetos que dependen del indice
    Relevancia relevancia(peliculas, indexador);
    Recomendador recomendador(peliculas, indexador);
    MotorDeBusqueda motor(indexador);

    // 4. Crear usuario y cargar su estado guardado
    Usuario usuario(1, "Edi");
    usuario.cargar("estado_usuario.txt");   // <-- CARGA ESTADO

    // 5. Bucle principal del menu
    while (true) {
        cout << "\n===== BUSCADOR DE PELICULAS =====\n";
        cout << "1. Buscar por titulo/sinopsis\n";
        cout << "2. Buscar por director\n";
        cout << "3. Buscar por genero\n";
        cout << "4. Buscar por actor\n";
        cout << "5. Ver recomendadas\n";
        cout << "6. Ver mi lista de ver mas tarde\n";
        cout << "7. Salir\n";
        cout << "Elige una opcion: ";

        int opcion = leerOpcionEntero();

        switch (opcion) {
            case 1:
                flujoBusquedaPorTexto(peliculas, relevancia, usuario);
                break;
            case 2:
                flujoBusquedaPorTag(peliculas, motor, usuario, "director");
                break;
            case 3:
                flujoBusquedaPorTag(peliculas, motor, usuario, "genero");
                break;
            case 4:
                flujoBusquedaPorTag(peliculas, motor, usuario, "actor");
                break;
            case 5:
                mostrarRecomendadas(peliculas, recomendador, usuario);
                break;
            case 6:
                mostrarVerMasTarde(peliculas, usuario);
                break;
            case 7:
                usuario.guardar("estado_usuario.txt");   // <-- GUARDA ESTADO
                cout << "Hasta luego!\n";
                return 0;
            default:
                cout << "Opcion invalida.\n";
        }
    }

    return 0;
}