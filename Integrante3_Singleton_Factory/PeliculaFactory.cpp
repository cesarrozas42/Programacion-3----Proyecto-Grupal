#include "PeliculaFactory.h"
#include "CSVReader.h"

using namespace std;

optional<Pelicula> PeliculaFactory::crear(int id, const vector<string>& campos) {
    if (campos.size() != 8) {
        return nullopt;
    }

    string anioTexto = CSVReader::limpiarTexto(campos[0]);
    if (anioTexto.empty()) {
        return nullopt;
    }

    int anio;
    try {
        anio = stoi(anioTexto);
    } catch (...) {
        return nullopt;
    }

    string titulo   = CSVReader::limpiarTexto(campos[1]);
    string origen   = CSVReader::limpiarTexto(campos[2]);
    string director = CSVReader::limpiarTexto(campos[3]);
    string casting  = CSVReader::limpiarTexto(campos[4]);
    string genero   = CSVReader::limpiarTexto(campos[5]);
    string wikiPage = CSVReader::limpiarTexto(campos[6]);
    string sinopsis = CSVReader::limpiarTexto(campos[7]);

    Pelicula pelicula(anio, titulo, origen, director,
                      casting, genero, wikiPage, sinopsis);

    pelicula.setId(id);
    pelicula.setTituloNormalizado(CSVReader::normalizarTexto(titulo));
    pelicula.setSinopsisNormalizada(CSVReader::normalizarTexto(sinopsis));

    return pelicula;
}
