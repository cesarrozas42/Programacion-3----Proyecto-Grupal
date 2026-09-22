#ifndef RECOMENDADOR_H
#define RECOMENDADOR_H

#include <vector>

#include "Pelicula.h"
#include "Usuario.h"
#include "Indexador.h"

using namespace std;

// Recomendacion basada en contenido: mira los tags (director, casting,
// genero, origen) de las peliculas que el usuario likeo, y busca otras
// peliculas que compartan esos tags. Entre mas tags en comun con las
// peliculas likeadas, mas arriba sale en la recomendacion.
class Recomendador {
private:
    const vector<Pelicula>& catalogo;
    const Indexador& indexador;

public:
    Recomendador(const vector<Pelicula>& catalogo, const Indexador& indexador);

    // Devuelve hasta "cantidad" IDs de peliculas recomendadas para el
    // usuario, excluyendo las que ya likeo. Si el usuario no tiene
    // likes, devuelve un vector vacio.
    vector<int> recomendar(const Usuario& usuario, int cantidad = 5) const;
};

#endif
