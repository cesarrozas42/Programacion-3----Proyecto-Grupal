#include "Pelicula.h"

Pelicula::Pelicula() {
    anio = 0;
}

Pelicula::Pelicula(int anio,
                   string titulo,
                   string origen,
                   string director,
                   string casting,
                   string genero,
                   string wikiPage,
                   string sinopsis) {

    this->anio = anio;
    this->titulo = titulo;
    this->origen = origen;
    this->director = director;
    this->casting = casting;
    this->genero = genero;
    this->wikiPage = wikiPage;
    this->sinopsis = sinopsis;

    generarTags();
}

void Pelicula::generarTags() {

    if (!director.empty()) {
        tags.push_back(Tag("director", director));
    }

    if (!casting.empty()) {
        tags.push_back(Tag("casting", casting));
    }

    if (!genero.empty()) {
        tags.push_back(Tag("genero", genero));
    }

    if (!origen.empty()) {
        tags.push_back(Tag("origen", origen));
    }
}

int Pelicula::getAnio() const {
    return anio;
}

string Pelicula::getTitulo() const {
    return titulo;
}

string Pelicula::getOrigen() const {
    return origen;
}

string Pelicula::getDirector() const {
    return director;
}

string Pelicula::getCasting() const {
    return casting;
}

string Pelicula::getGenero() const {
    return genero;
}

string Pelicula::getWikiPage() const {
    return wikiPage;
}

string Pelicula::getSinopsis() const {
    return sinopsis;
}

string Pelicula::getTituloNormalizado() const {
    return tituloNormalizado;
}

string Pelicula::getSinopsisNormalizada() const {
    return sinopsisNormalizada;
}

vector<Tag> Pelicula::getTags() const {
    return tags;
}

string Pelicula::getCast() const {
    return casting;
}

void Pelicula::setTituloNormalizado(string titulo) {
    tituloNormalizado = titulo;
}

void Pelicula::setSinopsisNormalizada(string sinopsis) {
    sinopsisNormalizada = sinopsis;
}