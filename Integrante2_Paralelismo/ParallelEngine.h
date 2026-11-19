#ifndef PROYECTO_BUSCADOR_PELICULAS_PARALLELENGINE_H
#define PROYECTO_BUSCADOR_PELICULAS_PARALLELENGINE_H

#include <string>
#include <vector>
#include <memory>
#include "Pelicula.h"
#include "Indexador.h"

class ParallelEngine {
public:
    static std::vector<Pelicula> cargarCSVParalelo(const std::string& rutaCSV, int numHilos);

    // CAMBIO AQUI: ahora devuelve un puntero inteligente
    static std::unique_ptr<Indexador> construirIndiceParalelo(const std::vector<Pelicula>& peliculas, int numHilos);
};

#endif //PROYECTO_BUSCADOR_PELICULAS_PARALLELENGINE_H
