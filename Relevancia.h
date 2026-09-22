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

// Toma los IDs candidatos que entrega el Indexador (Trie) y los
// ordena por relevancia: coincidencias en titulo pesan mas que en
// sinopsis, y una coincidencia de frase completa pesa mas que
// coincidencias sueltas de palabras.
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

    // Devuelve todos los resultados candidatos ordenados por score
    // descendente (mayor relevancia primero).
    vector<ResultadoBusqueda> buscar(const string& consulta) const;

    // Devuelve solo los IDs de una pagina (1-indexada) de tamano
    // "tamanoPagina" (5 por defecto: primeros 5, siguientes 5, etc.).
    vector<int> obtenerPagina(const vector<ResultadoBusqueda>& resultados,
                              int pagina,
                              int tamanoPagina = 5) const;
};

#endif
