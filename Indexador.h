#ifndef PROYECT_PRGR3_AVANCE_INDEXADOR_H
#define PROYECT_PRGR3_AVANCE_INDEXADOR_H

#include <string>
#include <vector>

#include "Pelicula.h"
#include "Trie.h"

using namespace std;

class Indexador {
private:
    Trie indiceContenido;
    Trie indiceTags;

public:
    Indexador();

    void construirIndice(const vector<Pelicula>& peliculas);

    vector<int> buscarContenido(const string& consulta) const;
    vector<int> buscarTag(const string& tipo, const string& valor) const;

    const Trie& getIndiceContenido() const;
    const Trie& getIndiceTags() const;

    void limpiar();
};

#endif
