#include <iostream>
#include <vector>
#include <chrono>
#include <future>
#include "ParallelEngine.h"
#include "CSVReader.h"
#include "Indexador.h"

using namespace std;
using namespace std::chrono;

int main() {
    string ruta = "wiki_movie_plots_deduped.csv";

    cout << "=== BENCHMARK DE RENDIMIENTO ===" << endl;
    cout << "Hilos\tCarga CSV (ms)\tIndice Sec (ms)\tTotal" << endl;
    cout << "--------------------------------------------------------" << endl;

    // 1. FLUJO SECUENCIAL
    auto inicio = high_resolution_clock::now();
    CSVReader reader;
    vector<Pelicula> pelisSec = reader.leerArchivo(ruta);
    auto finCargaSec = high_resolution_clock::now();
    double msCargaSec = duration<double, milli>(finCargaSec - inicio).count();

    inicio = high_resolution_clock::now();
    Indexador idxSec;
    idxSec.construirIndice(pelisSec);
    auto fin = high_resolution_clock::now();
    double msIndiceSec = duration<double, milli>(fin - inicio).count();

    cout << "Secuencial\t" << msCargaSec << "\t\t" << msIndiceSec << "\t\t" << (msCargaSec + msIndiceSec) << endl;
    // 2. FLUJO PARALELO (Solo Carga)
    for (int hilos : {1, 2, 4, 8}) {
        inicio = high_resolution_clock::now();
        vector<Pelicula> pelisPar = ParallelEngine::cargarCSVParalelo(ruta, hilos);
        fin = high_resolution_clock::now();
        double msCargaPar = duration<double, milli>(fin - inicio).count();

        cout << hilos << " hilo(s)\t" << msCargaPar << "\t\t";
        if (pelisPar.size() != pelisSec.size()) cout << "[ERROR]";
        else cout << "N/A\t\t" << "N/A" << endl;
    }

    // 3. BÚSQUEDA EN PARALELO
    cout << "\n=== BENCHMARK DE BUSQUEDA EN PARALELO ===\n";
    vector<string> consultas = {"matrix", "ghost", "ship", "action", "drama", "love", "war", "star"};

    inicio = high_resolution_clock::now();
    for (const string& q : consultas) {
        idxSec.buscarContenido(q);
    }
    fin = high_resolution_clock::now();
    double msBusqSec = duration<double, milli>(fin - inicio).count();
    cout << "Secuencial: " << msBusqSec << " ms para " << consultas.size() << " busquedas.\n";

    inicio = high_resolution_clock::now();
    vector<future<vector<int>>> tareas;
    for (const string& q : consultas) {
        tareas.push_back(std::async(std::launch::async, [&idxSec, q]() {
            return idxSec.buscarContenido(q);
        }));
    }
    for (auto& t : tareas) {
        t.get();
    }
    fin = high_resolution_clock::now();
    double msBusqPar = duration<double, milli>(fin - inicio).count();
    cout << "Paralela (std::async): " << msBusqPar << " ms para " << consultas.size() << " busquedas.\n";
    cout << "Mejora en busqueda: " << (msBusqSec / msBusqPar) << "x veces mas rapido.\n";

    return 0;
}