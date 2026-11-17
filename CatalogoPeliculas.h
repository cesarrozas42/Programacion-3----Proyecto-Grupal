#ifndef CATALOGO_PELICULAS_H
#define CATALOGO_PELICULAS_H

#include <cstddef>
#include <mutex>
#include <string>
#include <vector>
#include "Pelicula.h"
#include "Indexador.h"

// Patron Singleton: una unica instancia que posee las peliculas y el
// indice construido a partir de ellas (id de pelicula == posicion en el vector).
//
// Contrato con la Parte 2 (hilos):
//  - cargar() y cargarDesdeVector() estan protegidos con mutex.
//  - Despues de cargar, los getters son solo lectura: se pueden usar desde
//    muchos hilos sin bloqueo (ideal para "buscar varias palabras en paralelo").
//  - cargarDesdeVector() es el "enchufe" para el cargador paralelo: recibe las
//    peliculas ya parseadas, las renumera (id = posicion) y construye el indice.
class CatalogoPeliculas {
public:
    static CatalogoPeliculas& getInstancia();

    CatalogoPeliculas(const CatalogoPeliculas&) = delete;
    CatalogoPeliculas& operator=(const CatalogoPeliculas&) = delete;

    // Carga secuencial (CSVReader + PeliculaFactory). Si ya esta cargado no hace nada.
    bool cargar(const std::string& rutaCSV);

    // Para el cargador paralelo de la Parte 2.
    bool cargarDesdeVector(std::vector<Pelicula> nuevas);

    // Solo para benchmarks (tabla 1/2/4/8 hilos). Invalida las referencias
    // que otros objetos (Relevancia, Recomendador) tengan al catalogo.
    void reiniciar();

    bool estaCargado() const;
    std::size_t tamano() const;
    const Pelicula* obtener(int id) const;   // nullptr si el id es invalido
    const std::vector<Pelicula>& getPeliculas() const;
    const Indexador& getIndexador() const;

private:
    CatalogoPeliculas() = default;
    bool instalar(std::vector<Pelicula>&& nuevas);   // asume mutex tomado

    mutable std::mutex mtx;
    std::vector<Pelicula> peliculas;
    Indexador indexador;
    bool cargado = false;
};

#endif