#include "EstrategiaBusqueda.h"
#include <algorithm>
#include <cctype>
#include <future>
#include <thread>

using namespace std;

namespace {
string normalizar(string texto) {
    transform(texto.begin(), texto.end(), texto.begin(), [](unsigned char c) {
        return static_cast<char>(tolower(c));
    });
    return texto;
}

bool coincide(const Pelicula& pelicula, TipoBusqueda tipo, const string& consulta) {
    string valor;
    if (tipo == TipoBusqueda::SUBCADENA)
        valor = pelicula.getTituloNormalizado() + " " + pelicula.getSinopsisNormalizada();
    else if (tipo == TipoBusqueda::DIRECTOR)
        valor = pelicula.getDirector();
    else if (tipo == TipoBusqueda::GENERO)
        valor = pelicula.getGenero();
    else
        valor = pelicula.getCasting();
    return normalizar(valor).find(normalizar(consulta)) != string::npos;
}

vector<int> buscarRango(const vector<Pelicula>& catalogo, TipoBusqueda tipo,
                        const string& consulta, size_t inicio, size_t fin) {
    vector<int> resultado;
    for (size_t i = inicio; i < fin; ++i)
        if (coincide(catalogo[i], tipo, consulta)) resultado.push_back(static_cast<int>(i));
    return resultado;
}
}

EstrategiaConcretaBusqueda::EstrategiaConcretaBusqueda(
    const vector<Pelicula>& catalogo, const Indexador& indexador,
    TipoBusqueda tipo, ModoBusqueda modo)
    : catalogo(catalogo), indexador(indexador), tipo(tipo), modo(modo) {}

vector<int> EstrategiaConcretaBusqueda::buscar(const string& consulta) const {
    if (modo == ModoBusqueda::ARBOL) {
        if (tipo == TipoBusqueda::SUBCADENA) return indexador.buscarContenido(consulta);
        if (tipo == TipoBusqueda::DIRECTOR) return indexador.buscarTag("director", consulta);
        if (tipo == TipoBusqueda::GENERO) return indexador.buscarTag("genero", consulta);
        return indexador.buscarTag("casting", consulta);
    }

    if (modo == ModoBusqueda::SECUENCIAL)
        return buscarRango(catalogo, tipo, consulta, 0, catalogo.size());

    if (catalogo.empty()) return {};
    unsigned int cantidadHilos = max(1u, thread::hardware_concurrency());
    cantidadHilos = min(cantidadHilos, static_cast<unsigned int>(catalogo.size()));
    size_t bloque = (catalogo.size() + cantidadHilos - 1) / cantidadHilos;
    vector<future<vector<int>>> tareas;
    for (unsigned int i = 0; i < cantidadHilos; ++i) {
        size_t inicio = i * bloque;
        size_t fin = min(inicio + bloque, catalogo.size());
        if (inicio < fin) {
            tareas.push_back(async(launch::async, [&catalogo = catalogo, tipo = tipo, consulta, inicio, fin]() {
                return buscarRango(catalogo, tipo, consulta, inicio, fin);
            }));
        }
    }
    vector<int> resultado;
    for (auto& tarea : tareas) {
        vector<int> parcial = tarea.get();
        resultado.insert(resultado.end(), parcial.begin(), parcial.end());
    }
    sort(resultado.begin(), resultado.end());
    return resultado;
}
