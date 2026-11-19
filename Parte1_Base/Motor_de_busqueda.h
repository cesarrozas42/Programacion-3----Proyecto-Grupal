#ifndef MOTOR_DE_BUSQUEDA_H
#define MOTOR_DE_BUSQUEDA_H

#include <string>
#include <vector>

#include "Indexador.h"

using namespace std;

class MotorDeBusqueda {
private:
    const Indexador& indexador;

public:
    explicit MotorDeBusqueda(const Indexador& indexador);

    vector<int> buscarPorSubcadena(const string& consulta) const;

    vector<int> buscarPorDirector(const string& director) const;
    vector<int> buscarPorGenero(const string& genero) const;
    vector<int> buscarPorActor(const string& actor) const;
};

#endif
