#ifndef PROYECT_PRGR3_AVANCE_PELICULA_H
#define PROYECT_PRGR3_AVANCE_PELICULA_H

#include <string>
#include <vector>
#include "Tag.h"
using namespace std;

class Pelicula {
private:
    int anio;
    string titulo;
    string origen;
    string director;
    string casting;
    string genero;
    string wikiPage;
    string sinopsis;

    string tituloNormalizado;
    string sinopsisNormalizada;

    vector<Tag> tags;

public:
    Pelicula();

    Pelicula(int anio,
             string titulo,
             string origen,
             string director,
             string casting,
             string genero,
             string wikiPage,
             string sinopsis);

    void generarTags();

    int getAnio() const;
    string getTitulo() const;
    string getOrigen() const;
    string getDirector() const;
    string getCasting() const;
    string getCast() const; // <-- Agregado para compatibilidad
    string getGenero() const;
    string getWikiPage() const;
    string getSinopsis() const;

    string getTituloNormalizado() const;
    string getSinopsisNormalizada() const;

    vector<Tag> getTags() const;

    void setTituloNormalizado(string titulo);
    void setSinopsisNormalizada(string sinopsis);
};

#endif