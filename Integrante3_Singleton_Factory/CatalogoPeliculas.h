#ifndef CATALOGO_PELICULAS_H
#define CATALOGO_PELICULAS_H

#include <cstddef>
#include <mutex>
#include <string>
#include <vector>
#include "Pelicula.h"
#include "Indexador.h"

class CatalogoPeliculas {
public:
    static CatalogoPeliculas& getInstancia();

    CatalogoPeliculas(const CatalogoPeliculas&) = delete;
    CatalogoPeliculas& operator=(const CatalogoPeliculas&) = delete;

    bool cargar(const std::string& rutaCSV);

    bool cargarDesdeVector(std::vector<Pelicula> nuevas);

    void reiniciar();

    bool estaCargado() const;
    std::size_t tamano() const;
    const Pelicula* obtener(int id) const;
    const std::vector<Pelicula>& getPeliculas() const;
    const Indexador& getIndexador() const;

private:
    CatalogoPeliculas() = default;
    bool instalar(std::vector<Pelicula>&& nuevas);

    mutable std::mutex mtx;
    std::vector<Pelicula> peliculas;
    Indexador indexador;
    bool cargado = false;
};

#endif
