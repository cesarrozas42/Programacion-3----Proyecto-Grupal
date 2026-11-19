#include "CatalogoPeliculas.h"
#include "CSVReader.h"

#include <utility>

using namespace std;

CatalogoPeliculas& CatalogoPeliculas::getInstancia() {
    // Inicializacion estatica local: thread-safe garantizado desde C++11.
    static CatalogoPeliculas instancia;
    return instancia;
}

bool CatalogoPeliculas::instalar(vector<Pelicula>&& nuevas) {
    if (nuevas.empty()) {
        return false;
    }

    // El Indexador usa la posicion como id, asi que el id de cada pelicula
    // debe coincidir con su posicion final (aunque el cargador haya descartado filas).
    for (size_t i = 0; i < nuevas.size(); ++i) {
        nuevas[i].setId(static_cast<int>(i));
    }

    peliculas = move(nuevas);
    indexador.construirIndice(peliculas);
    cargado = true;
    return true;
}

bool CatalogoPeliculas::cargar(const string& rutaCSV) {
    lock_guard<mutex> lock(mtx);
    if (cargado) {
        return true;
    }

    CSVReader lector;
    return instalar(lector.leerArchivo(rutaCSV));
}

bool CatalogoPeliculas::cargarDesdeVector(vector<Pelicula> nuevas) {
    lock_guard<mutex> lock(mtx);
    if (cargado) {
        return true;
    }
    return instalar(move(nuevas));
}

void CatalogoPeliculas::reiniciar() {
    lock_guard<mutex> lock(mtx);
    peliculas.clear();
    indexador.limpiar();
    cargado = false;
}

bool CatalogoPeliculas::estaCargado() const {
    lock_guard<mutex> lock(mtx);
    return cargado;
}

size_t CatalogoPeliculas::tamano() const {
    return peliculas.size();
}

const Pelicula* CatalogoPeliculas::obtener(int id) const {
    if (id < 0 || id >= static_cast<int>(peliculas.size())) {
        return nullptr;
    }
    return &peliculas[id];
}

const vector<Pelicula>& CatalogoPeliculas::getPeliculas() const {
    return peliculas;
}

const Indexador& CatalogoPeliculas::getIndexador() const {
    return indexador;
}