#ifndef PELICULA_FACTORY_H
#define PELICULA_FACTORY_H

#include <optional>
#include <string>
#include <vector>
#include "Pelicula.h"

class PeliculaFactory {
public:
    PeliculaFactory() = delete;

    static std::optional<Pelicula> crear(int id,
                                         const std::vector<std::string>& campos);
};

#endif
