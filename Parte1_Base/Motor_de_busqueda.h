#ifndef MOTOR_DE_BUSQUEDA_H
#define MOTOR_DE_BUSQUEDA_H

#include <string>
#include <vector>

#include "Indexador.h"

using namespace std;

// Version adaptada: en vez de recorrer todo el catalogo con .find(),
// usa el Indexador (Trie) construido en la parte 2 y devuelve IDs
// (posicion en el vector<Pelicula>) en vez de copias de Pelicula.
class MotorDeBusqueda {
private:
    const Indexador& indexador;

public:
    explicit MotorDeBusqueda(const Indexador& indexador);

    // Busqueda por subcadena en titulo + sinopsis (usa indiceContenido)
    vector<int> buscarPorSubcadena(const string& consulta) const;

    // Busqueda por tags (usa indiceTags)
    vector<int> buscarPorDirector(const string& director) const;
    vector<int> buscarPorGenero(const string& genero) const;
    vector<int> buscarPorActor(const string& actor) const;
};

#endif