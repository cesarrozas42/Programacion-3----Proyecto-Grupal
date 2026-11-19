#ifndef RECOMENDADOR_H
#define RECOMENDADOR_H

#include <vector>

#include "Pelicula.h"
#include "Usuario.h"
#include "Indexador.h"

using namespace std;

class Recomendador {
private:
    const vector<Pelicula>& catalogo;
    const Indexador& indexador;

public:
    Recomendador(const vector<Pelicula>& catalogo, const Indexador& indexador);

    vector<int> recomendar(const Usuario& usuario, int cantidad = 5) const;
};

#endif
