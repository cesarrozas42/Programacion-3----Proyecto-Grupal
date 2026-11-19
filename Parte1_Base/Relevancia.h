#ifndef RELEVANCIA_H
#define RELEVANCIA_H

#include <string>
#include <vector>

#include "Pelicula.h"
#include "Indexador.h"

using namespace std;

struct ResultadoBusqueda {
    int idPelicula;
    double score;
};

class Relevancia {
private:
    const vector<Pelicula>& catalogo;
    const Indexador& indexador;

    vector<string> separarPalabras(const string& consulta) const;
    string normalizarSimple(const string& texto) const;
    double calcularScore(int idPelicula,
                         const vector<string>& palabras,
                         const string& consultaNormalizada) const;

public:
    Relevancia(const vector<Pelicula>& catalogo, const Indexador& indexador);

    vector<ResultadoBusqueda> buscar(const string& consulta) const;

    vector<int> obtenerPagina(const vector<ResultadoBusqueda>& resultados,
                              int pagina,
                              int tamanoPagina = 5) const;
};

#endif
