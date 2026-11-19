#include "ParallelEngine.h"
#include "CSVReader.h"
#include "PeliculaFactory.h"

#include <fstream>
#include <iostream>
#include <thread>
#include <vector>
#include <algorithm>

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

    contenido.erase(remove(contenido.begin(), contenido.end(), '\r'), contenido.end());

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
        size_t misFilas = filasPorHilo + (static_cast<size_t>(i) < sobrantes ? 1 : 0);
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
std::unique_ptr<Indexador> ParallelEngine::construirIndiceParalelo(const vector<Pelicula>& peliculas, [[maybe_unused]] int numHilos) {
    auto idx = std::make_unique<Indexador>();
    idx->construirIndice(peliculas);
    return idx;
}
