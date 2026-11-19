#include <algorithm>
#include <chrono>
#include <filesystem>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>
#include <thread>
#include <vector>

#include "CSVReader.h"
#include "Indexador.h"
#include "ParallelEngine.h"

using namespace std;
namespace fs = std::filesystem;

const int REPETICIONES = 3;
const int VUELTAS_BUSQUEDA = 40;

struct Resultado {
    string modo;
    double ms;
    bool correcto;
};

class SilenciarSalida {
    ostringstream descarte;
    streambuf* anterior;
public:
    SilenciarSalida() : anterior(cout.rdbuf(descarte.rdbuf())) {}
    ~SilenciarSalida() { cout.rdbuf(anterior); }
};

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

template <typename Tarea>
double medirMejorTiempo(int repeticiones, Tarea tarea) {
    double mejor = -1;
    for (int i = 0; i < repeticiones; ++i) {
        auto inicio = chrono::steady_clock::now();
        tarea();
        double ms = chrono::duration<double, milli>(chrono::steady_clock::now() - inicio).count();
        if (mejor < 0 || ms < mejor) mejor = ms;
    }
    return mejor;
}

bool mismasPeliculas(const vector<Pelicula>& a, const vector<Pelicula>& b) {
    if (a.size() != b.size()) return false;
    for (size_t i = 0; i < a.size(); ++i) {
        if (a[i].getId() != b[i].getId() || a[i].getAnio() != b[i].getAnio() ||
            a[i].getTitulo() != b[i].getTitulo() || a[i].getDirector() != b[i].getDirector() ||
            a[i].getTituloNormalizado() != b[i].getTituloNormalizado() ||
            a[i].getSinopsisNormalizada() != b[i].getSinopsisNormalizada())
            return false;
    }
    return true;
}

void imprimirTabla(const string& titulo, const vector<Resultado>& filas) {
    const double referencia = filas.front().ms;
    cout << "\n" << titulo << "\n";
    cout << left << setw(14) << "Modo" << right << setw(14) << "Tiempo (ms)"
         << setw(14) << "Aceleracion" << "   Resultado\n";
    cout << string(56, '-') << "\n";
    for (const Resultado& fila : filas) {
        cout << left << setw(14) << fila.modo << right << fixed
             << setw(14) << setprecision(1) << fila.ms
             << setw(13) << setprecision(2) << referencia / fila.ms << "x"
             << "   " << (fila.correcto ? "OK" : "ERROR: no coincide con el secuencial") << "\n";
    }
    auto mejor = min_element(filas.begin(), filas.end(),
                             [](const Resultado& x, const Resultado& y) { return x.ms < y.ms; });
    cout << "Mas rapido: " << mejor->modo << " (" << setprecision(2) << referencia / mejor->ms
         << "x frente al secuencial)\n";
}

vector<int> modosDeHilos() {
    vector<int> hilos = {1, 2, 3, 4, 8};
    const int nucleos = static_cast<int>(thread::hardware_concurrency());
    if (nucleos > 0 && find(hilos.begin(), hilos.end(), nucleos) == hilos.end()) hilos.push_back(nucleos);
    sort(hilos.begin(), hilos.end());
    return hilos;
}

string nombreModo(int hilos) {
    return to_string(hilos) + (hilos == 1 ? " hilo" : " hilos");
}

long long huella(const vector<int>& ids) {
    long long suma = static_cast<long long>(ids.size()) * 1000003LL;
    for (int id : ids) suma += id;
    return suma;
}

int main(int argc, char* argv[]) {
    const string ruta = localizarCSV(argc, argv);
    if (ruta.empty()) {
        cerr << "No se encontro wiki_movie_plots_deduped.csv.\n"
             << "Colocalo junto al proyecto o ejecuta: benchmark <ruta-del-csv>\n";
        return 1;
    }

    cout << "=== BENCHMARK: SECUENCIAL vs HILOS ===\n";
    cout << "CSV: " << ruta << "\n";
    cout << "Nucleos logicos de esta PC: " << thread::hardware_concurrency() << "\n";
    cout << "Cada tiempo es el mejor de " << REPETICIONES << " repeticiones.\n";
#ifndef NDEBUG
    cout << "Compilado en Debug: en Release los tiempos son menores, pero la comparacion vale igual.\n";
#endif

    vector<Pelicula> referencia;
    vector<Resultado> carga;
    {
        SilenciarSalida silencio;
        double ms = medirMejorTiempo(REPETICIONES, [&] {
            CSVReader lector;
            referencia = lector.leerArchivo(ruta);
        });
        carga.push_back({"Secuencial", ms, !referencia.empty()});
    }
    if (referencia.empty()) {
        cerr << "El CSV no tiene peliculas validas.\n";
        return 1;
    }
    cout << "Peliculas cargadas: " << referencia.size() << "\n";

    for (int hilos : modosDeHilos()) {
        vector<Pelicula> paralelas;
        double ms = medirMejorTiempo(REPETICIONES, [&] {
            paralelas = ParallelEngine::cargarCSVParalelo(ruta, hilos);
        });
        carga.push_back({nombreModo(hilos), ms, mismasPeliculas(referencia, paralelas)});
    }
    imprimirTabla("1) CARGA DEL CSV", carga);

    cout << "\nConstruyendo el indice (arbol Trie) para las busquedas...\n";
    Indexador indexador;
    double msIndice = medirMejorTiempo(1, [&] { indexador.construirIndice(referencia); });
    cout << "Indice listo en " << fixed << setprecision(1) << msIndice << " ms\n";

    const vector<string> consultas = {"matrix", "ghost", "ship", "action", "drama", "love",
                                      "war", "star", "detective", "space", "vampire", "island"};
    vector<string> tareas;
    for (int v = 0; v < VUELTAS_BUSQUEDA; ++v)
        tareas.insert(tareas.end(), consultas.begin(), consultas.end());

    vector<long long> esperado(tareas.size());
    vector<Resultado> busqueda;
    double msSecuencial = medirMejorTiempo(REPETICIONES, [&] {
        for (size_t i = 0; i < tareas.size(); ++i) esperado[i] = huella(indexador.buscarContenido(tareas[i]));
    });
    busqueda.push_back({"Secuencial", msSecuencial, true});

    for (int hilos : modosDeHilos()) {
        vector<long long> obtenido(tareas.size());
        double ms = medirMejorTiempo(REPETICIONES, [&] {
            vector<thread> trabajadores;
            for (int h = 0; h < hilos; ++h) {
                trabajadores.emplace_back([&, h] {
                    for (size_t i = static_cast<size_t>(h); i < tareas.size(); i += static_cast<size_t>(hilos))
                        obtenido[i] = huella(indexador.buscarContenido(tareas[i]));
                });
            }
            for (thread& t : trabajadores) t.join();
        });
        busqueda.push_back({nombreModo(hilos), ms, obtenido == esperado});
    }
    imprimirTabla("2) BUSQUEDA: " + to_string(tareas.size()) + " consultas sobre el arbol", busqueda);

    return 0;
}
