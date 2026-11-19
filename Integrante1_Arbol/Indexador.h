#ifndef PROYECT_PRGR3_AVANCE_INDEXADOR_H
#define PROYECT_PRGR3_AVANCE_INDEXADOR_H

#include <map>
#include <string>
#include <vector>

#include "Pelicula.h"
#include "Trie.h"

using namespace std;

class Indexador {
private:
    Trie indiceContenido;
    map<string, Trie> indicesTags;

    static string tipoCanonico(const string& tipo);
    static vector<string> separarValores(const string& valor);

public:
    Indexador();

    void construirIndice(const vector<Pelicula>& peliculas);

    vector<int> buscarContenido(const string& consulta) const;

    vector<int> buscarTag(const string& tipo, const string& valor) const;

    vector<string> autocompletar(const string& prefijo, size_t limite = 10) const;

    const Trie& getIndiceContenido() const;
    const Trie* getIndiceTag(const string& tipo) const;

    void limpiar();
};

#endif
