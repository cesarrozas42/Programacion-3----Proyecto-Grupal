#include <cassert>
#include <iostream>
#include <string>
#include <thread>
#include <vector>
#include "PeliculaFactory.h"
#include "CatalogoPeliculas.h"

using namespace std;

static vector<string> filaValida(const string& titulo = "The Matrix") {
    return {"1999", titulo, "American", "Wachowski", "Keanu Reeves",
            "sci-fi", "https://wiki/x", "A hacker discovers reality"};
}

void probarFactory() {
    auto p = PeliculaFactory::crear(7, filaValida());
    assert(p.has_value());
    assert(p->getId() == 7);
    assert(p->getAnio() == 1999);
    assert(p->getTituloNormalizado() == "the matrix");
    assert(p->getSinopsisNormalizada() == "a hacker discovers reality");
    assert(p->getTags().size() == 4);

    auto pocos = filaValida();
    pocos.pop_back();
    assert(!PeliculaFactory::crear(0, pocos).has_value());

    auto anioMalo = filaValida();
    anioMalo[0] = "abcd";
    assert(!PeliculaFactory::crear(0, anioMalo).has_value());

    auto anioVacio = filaValida();
    anioVacio[0] = "";
    assert(!PeliculaFactory::crear(0, anioVacio).has_value());

    cout << "[TEST] Factory: PASADO\n";
}

void probarSingleton() {
    CatalogoPeliculas& a = CatalogoPeliculas::getInstancia();
    CatalogoPeliculas& b = CatalogoPeliculas::getInstancia();
    assert(&a == &b);

    vector<CatalogoPeliculas*> vistas(8, nullptr);
    vector<thread> hilos;
    for (int i = 0; i < 8; ++i) {
        hilos.emplace_back([&vistas, i] { vistas[i] = &CatalogoPeliculas::getInstancia(); });
    }
    for (auto& h : hilos) h.join();
    for (auto* p : vistas) assert(p == &a);

    assert(!a.estaCargado());

    vector<Pelicula> v;
    for (const string t : {"The Matrix", "Ghost Ship", "Forrest Gump"}) {
        auto p = PeliculaFactory::crear(99, filaValida(t));
        assert(p.has_value());
        v.push_back(*p);
    }

    assert(a.cargarDesdeVector(v));
    assert(a.estaCargado());
    assert(a.tamano() == 3);

    for (int i = 0; i < 3; ++i) {
        assert(a.obtener(i) != nullptr);
        assert(a.obtener(i)->getId() == i);
    }
    assert(a.obtener(3) == nullptr);
    assert(a.obtener(-1) == nullptr);

    assert(!a.getIndexador().buscarContenido("matrix").empty());

    assert(a.cargarDesdeVector(vector<Pelicula>{}));
    assert(a.tamano() == 3);

    a.reiniciar();
    assert(!a.estaCargado());
    assert(a.tamano() == 0);

    cout << "[TEST] Singleton: PASADO\n";
}

int main() {
    probarFactory();
    probarSingleton();
    cout << "TODAS LAS PRUEBAS DE PATRONES PASARON\n";
    return 0;
}
