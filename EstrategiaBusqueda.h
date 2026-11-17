#ifndef ESTRATEGIA_BUSQUEDA_H
#define ESTRATEGIA_BUSQUEDA_H

#include <string>
#include <vector>
#include "Indexador.h"
#include "Pelicula.h"

enum class ModoBusqueda { SECUENCIAL, ARBOL, PARALELA };
enum class TipoBusqueda { SUBCADENA, DIRECTOR, GENERO, ACTOR };

class EstrategiaBusqueda {
public:
    virtual ~EstrategiaBusqueda() = default;
    virtual std::vector<int> buscar(const std::string& consulta) const = 0;
};

class EstrategiaConcretaBusqueda : public EstrategiaBusqueda {
private:
    const std::vector<Pelicula>& catalogo;
    const Indexador& indexador;
    TipoBusqueda tipo;
    ModoBusqueda modo;
public:
    EstrategiaConcretaBusqueda(const std::vector<Pelicula>& catalogo,
                              const Indexador& indexador,
                              TipoBusqueda tipo,
                              ModoBusqueda modo = ModoBusqueda::ARBOL);
    std::vector<int> buscar(const std::string& consulta) const override;
};

#endif
