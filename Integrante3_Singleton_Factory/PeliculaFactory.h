#ifndef PELICULA_FACTORY_H
#define PELICULA_FACTORY_H

#include <optional>
#include <string>
#include <vector>
#include "Pelicula.h"

// Patron Factory Method (version estatica).
// Centraliza la creacion de objetos Pelicula a partir de los campos del CSV.
//
// IMPORTANTE para la Parte 2 (hilos): esta clase NO tiene estado
// (sin contadores ni variables estaticas), por lo que crear() puede
// llamarse desde varios hilos a la vez sin mutex.
class PeliculaFactory {
public:
    PeliculaFactory() = delete;   // clase utilitaria: no se instancia

    // Orden esperado de los 8 campos del CSV:
    // anio, titulo, origen, director, casting, genero, wikiPage, sinopsis.
    // Devuelve std::nullopt si la fila es invalida.
    static std::optional<Pelicula> crear(int id,
                                         const std::vector<std::string>& campos);
};

#endif