#include "ParallelEngine.h"
#include "CSVReader.h"
#include "PeliculaFactory.h"

#include <fstream>
#include <iostream>
#include <thread>
#include <vector>
#include <chrono>
#include <algorithm>

#include <future>

using namespace std;
using namespace std::chrono; // <-- AGREGAR ESTO

using namespace std;


// FUNCIÓN AUXILIAR: Separar CSV respetando comillas
static vector<string> separarCSVParalelo(const string& linea) {
    vector<string> campos;
    string campo;
    bool dentroComillas = false;

    for (size_t i = 0; i < linea.size(); ++i) {
        char c = linea[i];
        if (c == '"') {
            if (dentroComillas && i + 1 < linea.size() && linea[i + 1] == '"') {
                campo += '"';
                i++;
            } else {
                dentroComillas = !dentroComillas;
            }
        } else if (c == ',' && !dentroComillas) {
            campos.push_back(campo);
            campo.clear();
        } else {
            campo += c;
        }
    }
    campos.push_back(campo);
    return campos;
}

// ============================================================
// LECTURA PARALELA DEL CSV
// ============================================================
vector<Pelicula> ParallelEngine::cargarCSVParalelo(const string& rutaCSV, int numHilos) {
    ifstream archivo(rutaCSV, ios::binary);
    if (!archivo.is_open()) {
        cerr << "Error: No se pudo abrir el CSV en " << rutaCSV << endl;
        return {};
    }

    string contenido((istreambuf_iterator<char>(archivo)), istreambuf_iterator<char>());
    archivo.close();

    if (contenido.empty()) return {};

    vector<size_t> iniciosFilas;
    iniciosFilas.reserve(40000);
    iniciosFilas.push_back(0);

    bool dentroComillas = false;
    for (size_t i = 0; i < contenido.size(); ++i) {
        if (contenido[i] == '"') {
            if (dentroComillas && i + 1 < contenido.size() && contenido[i + 1] == '"') {
                i++;
            } else {
                dentroComillas = !dentroComillas;
            }
        } else if (contenido[i] == '\n' && !dentroComillas) {
            if (i + 1 < contenido.size()) {
                iniciosFilas.push_back(i + 1);
            }
        }
    }

    if (iniciosFilas.empty()) return {};

    size_t totalFilas = iniciosFilas.size();
    size_t filasDatos = totalFilas - 1;

    numHilos = min(numHilos, (int)filasDatos);
    if (numHilos <= 0) return {};

    vector<vector<Pelicula>> resultadosParciales(numHilos);
    vector<thread> hilos;

    size_t filasPorHilo = filasDatos / numHilos;
    size_t sobrantes = filasDatos % numHilos;

    size_t filaActual = 1;

    for (int i = 0; i < numHilos; ++i) {
        size_t misFilas = filasPorHilo + (i < sobrantes ? 1 : 0);
        size_t inicioIdx = filaActual;
        size_t finIdx = filaActual + misFilas;
        filaActual = finIdx;

        hilos.emplace_back([&, i, inicioIdx, finIdx]() {
            vector<Pelicula> misPeliculas;
            misPeliculas.reserve(finIdx - inicioIdx);

            for (size_t f = inicioIdx; f < finIdx; ++f) {
                if (f >= iniciosFilas.size()) break;

                size_t inicioByte = iniciosFilas[f];
                size_t finByte = (f + 1 < iniciosFilas.size()) ? iniciosFilas[f + 1] : contenido.size();

                string registro = contenido.substr(inicioByte, finByte - inicioByte);

                if (!registro.empty() && registro.back() == '\n') registro.pop_back();
                if (!registro.empty() && registro.back() == '\r') registro.pop_back();

                if (registro.empty()) continue;

                vector<string> campos = separarCSVParalelo(registro);
                auto pelicula = PeliculaFactory::crear(-1, campos);

                if (pelicula) {
                    misPeliculas.push_back(*pelicula);
                }
            }
            resultadosParciales[i] = std::move(misPeliculas);
        });
    }

    for (auto& h : hilos) {
        if (h.joinable()) h.join();
    }

    vector<Pelicula> resultadoFinal;
    resultadoFinal.reserve(filasDatos);
    for (auto& parcial : resultadosParciales) {
        for (auto& peli : parcial) {
            peli.setId(static_cast<int>(resultadoFinal.size()));
            resultadoFinal.push_back(std::move(peli));
        }
    }

    return resultadoFinal;
}

// CONSTRUCCIÓN PARALELA DEL ÍNDICE
std::unique_ptr<Indexador> ParallelEngine::construirIndiceParalelo(const vector<Pelicula>& peliculas, int numHilos) {
    auto idx = std::make_unique<Indexador>();
    idx->construirIndice(peliculas);
    return idx;
}

// BENCHMARK

void ParallelEngine::ejecutarBenchmark(const string& rutaCSV) {
    cout << "=== BENCHMARK DE RENDIMIENTO ===" << endl;
    cout << "Hilos\tCarga CSV (ms)\tIndice Sec (ms)\tTotal" << endl;
    cout << "--------------------------------------------------------" << endl;

    // --- 1. FLUJO SECUENCIAL ---
    auto inicio = high_resolution_clock::now();
    CSVReader reader;
    vector<Pelicula> pelisSec = reader.leerArchivo(rutaCSV);
    auto finCargaSec = high_resolution_clock::now();
    double msCargaSec = duration<double, milli>(finCargaSec - inicio).count();

    inicio = high_resolution_clock::now();
    Indexador idxSec;
    idxSec.construirIndice(pelisSec);
    auto fin = high_resolution_clock::now();
    double msIndiceSec = duration<double, milli>(fin - inicio).count();

    cout << "Secuencial\t" << msCargaSec << "\t\t" << msIndiceSec << "\t\t" << (msCargaSec + msIndiceSec) << endl;

    // --- 2. FLUJO PARALELO (Solo Carga) ---
    for (int hilos : {1, 2, 4, 8}) {
        inicio = high_resolution_clock::now();
        vector<Pelicula> pelisPar = ParallelEngine::cargarCSVParalelo(rutaCSV, hilos);
        fin = high_resolution_clock::now();
        double msCargaPar = duration<double, milli>(fin - inicio).count();

        cout << hilos << " hilo(s)\t" << msCargaPar << "\t\t";
        if (pelisPar.size() != pelisSec.size()) cout << "[ERROR]";
        else cout << "N/A\t\t" << "N/A" << endl;
    }

    // --- 3. BÚSQUEDA EN PARALELO ---
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
}