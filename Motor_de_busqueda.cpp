#include "Motor_de_busqueda.h"

MotorDeBusqueda::MotorDeBusqueda(const Indexador& indexador)
    : indexador(indexador) {}

vector<int> MotorDeBusqueda::buscarPorSubcadena(const string& consulta) const {
    return indexador.buscarContenido(consulta);
}

vector<int> MotorDeBusqueda::buscarPorDirector(const string& director) const {
    return indexador.buscarTag("director", director);
}

vector<int> MotorDeBusqueda::buscarPorGenero(const string& genero) const {
    return indexador.buscarTag("genero", genero);
}

vector<int> MotorDeBusqueda::buscarPorActor(const string& actor) const {
    return indexador.buscarTag("casting", actor);
}