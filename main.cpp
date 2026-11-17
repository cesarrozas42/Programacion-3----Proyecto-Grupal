#ifdef _WIN32
// windows.h va primero: los headers del proyecto usan "using namespace std"
// y si se incluye despues, std::byte choca con el byte de Windows.
#define NOMINMAX
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#endif

#include <chrono>
#include <filesystem>
#include <iostream>
#include <string>
#include <vector>

#include "CSVReader.h"
#include "Indexador.h"
#include "InterfazCLI.h"
#include "Recomendador.h"
#include "Relevancia.h"
#include "Usuario.h"

using namespace std;
namespace fs = std::filesystem;

string localizarCSV(int argc, char* argv[]) {
    vector<fs::path> candidatos;
    if (argc > 1) candidatos.emplace_back(argv[1]);
    const fs::path ejecutable = fs::absolute(argv[0]).parent_path();
    candidatos.emplace_back("wiki_movie_plots_deduped.csv");
    candidatos.emplace_back(fs::path("datos") / "wiki_movie_plots_deduped.csv");
    candidatos.emplace_back(ejecutable / "wiki_movie_plots_deduped.csv");
    candidatos.emplace_back(ejecutable.parent_path() / "wiki_movie_plots_deduped.csv");
    for (const fs::path& candidato : candidatos)
        if (fs::exists(candidato) && fs::is_regular_file(candidato)) return candidato.string();
    return "";
}

// Consola en UTF-8 y con colores ANSI tambien fuera de CLion (cmd, PowerShell).
void prepararConsola() {
#ifdef _WIN32
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);
    HANDLE salida = GetStdHandle(STD_OUTPUT_HANDLE);
    DWORD modo = 0;
    if (salida != INVALID_HANDLE_VALUE && GetConsoleMode(salida, &modo))
        SetConsoleMode(salida, modo | 0x0004 /* ENABLE_VIRTUAL_TERMINAL_PROCESSING */);
#endif
}

double msDesde(chrono::steady_clock::time_point inicio) {
    return chrono::duration<double, milli>(chrono::steady_clock::now() - inicio).count();
}

int main(int argc, char* argv[]) {
    prepararConsola();

    const string rutaCSV = localizarCSV(argc, argv);
    if (rutaCSV.empty()) {
        cerr << "No se encontro wiki_movie_plots_deduped.csv.\n"
             << "Colocalo junto al proyecto o ejecuta:\n"
             << "  plataforma_streaming <ruta-del-csv>\n";
        return 1;
    }

    TiemposCarga tiempos;

    cout << "Cargando catalogo desde: " << rutaCSV << "\n";
    auto inicio = chrono::steady_clock::now();
    CSVReader lector;
    vector<Pelicula> peliculas = lector.leerArchivo(rutaCSV);
    tiempos.msLecturaCSV = msDesde(inicio);
    if (peliculas.empty()) {
        cerr << "No se pudo cargar ninguna pelicula. Verifica el formato del CSV.\n";
        return 1;
    }

    cout << "Construyendo indice de busqueda (arbol Trie)...\n";
    inicio = chrono::steady_clock::now();
    Indexador indexador;
    indexador.construirIndice(peliculas);
    tiempos.msIndice = msDesde(inicio);

    // Las busquedas por director, genero y actor ahora pasan por las
    // estrategias de EstrategiaBusqueda (Strategy); MotorDeBusqueda sigue
    // disponible para las pruebas de la parte 1.
    Relevancia relevancia(peliculas, indexador);
    Recomendador recomendador(peliculas, indexador);

    Usuario usuario(1, "Usuario");
    const string archivoEstado = "estado_usuario.txt";
    usuario.cargar(archivoEstado);

    InterfazCLI interfaz(peliculas, indexador, relevancia, recomendador,
                         usuario, archivoEstado, tiempos);
    interfaz.ejecutar();
    return 0;
}
