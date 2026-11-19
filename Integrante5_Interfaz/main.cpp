#ifdef _WIN32
#define NOMINMAX
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#endif

#include <chrono>
#include <filesystem>
#include <iostream>
#include <string>
#include <utility>
#include <vector>

#include "CSVReader.h"
#include "CatalogoPeliculas.h"
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

void prepararConsola() {
#ifdef _WIN32
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);
    HANDLE salida = GetStdHandle(STD_OUTPUT_HANDLE);
    DWORD modo = 0;
    if (salida != INVALID_HANDLE_VALUE && GetConsoleMode(salida, &modo))
        SetConsoleMode(salida, modo | 0x0004 );
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
    vector<Pelicula> leidas = lector.leerArchivo(rutaCSV);
    tiempos.msLecturaCSV = msDesde(inicio);

    cout << "Construyendo indice de busqueda (arbol Trie)...\n";
    inicio = chrono::steady_clock::now();
    CatalogoPeliculas& catalogo = CatalogoPeliculas::getInstancia();
    if (!catalogo.cargarDesdeVector(std::move(leidas))) {
        cerr << "No se pudo cargar ninguna pelicula. Verifica el formato del CSV.\n";
        return 1;
    }
    tiempos.msIndice = msDesde(inicio);

    const vector<Pelicula>& peliculas = catalogo.getPeliculas();
    const Indexador& indexador = catalogo.getIndexador();

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
