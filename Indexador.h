#ifndef PROYECT_PRGR3_AVANCE_INDEXADOR_H
#define PROYECT_PRGR3_AVANCE_INDEXADOR_H

#include <map>
#include <string>
#include <vector>

#include "Pelicula.h"
#include "Trie.h"

using namespace std;

// Construye los arboles de busqueda a partir del catalogo:
//  - indiceContenido: palabras del titulo y la sinopsis.
//  - indicesTags: UN arbol por tipo de tag (director, casting, genero,
//    origen). Asi buscar el actor "Nolan" no devuelve peliculas donde
//    Nolan solo es director.
class Indexador {
private:
    Trie indiceContenido;
    map<string, Trie> indicesTags;

    static string tipoCanonico(const string& tipo);
    static vector<string> separarValores(const string& valor);

public:
    Indexador();

    void construirIndice(const vector<Pelicula>& peliculas);

    // Peliculas cuyo titulo o sinopsis contiene todas las palabras de la consulta.
    vector<int> buscarContenido(const string& consulta) const;

    // Peliculas cuyo tag del tipo indicado contiene el valor.
    // tipo: "director", "casting" (o "actor"), "genero", "origen".
    // Si el valor trae varios elementos separados por comas
    // ("Keanu Reeves, Carrie-Anne Moss") devuelve las que tienen alguno.
    vector<int> buscarTag(const string& tipo, const string& valor) const;

    // Sugerencias de palabras para el autocompletado de la interfaz.
    vector<string> autocompletar(const string& prefijo, size_t limite = 10) const;

    const Trie& getIndiceContenido() const;
    // Devuelve nullptr si el tipo de tag no existe.
    const Trie* getIndiceTag(const string& tipo) const;

    void limpiar();
};

#endif
