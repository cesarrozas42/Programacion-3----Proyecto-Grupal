#include <cassert>
#include <functional>
#include <iostream>
#include <random>
#include <set>
#include <string>
#include <vector>

#include "Indexador.h"
#include "Pelicula.h"
#include "Trie.h"

using namespace std;

namespace {

int pasadas = 0;
int fallidas = 0;

void verificar(bool condicion, const string& mensaje) {
    if (!condicion) throw runtime_error(mensaje);
}

void prueba(const string& nombre, const function<void()>& cuerpo) {
    try {
        cuerpo();
        pasadas++;
        cout << "[OK]    " << nombre << '\n';
    } catch (const exception& e) {
        fallidas++;
        cout << "[FALLA] " << nombre << " -> " << e.what() << '\n';
    }
}

using Ids = vector<int>;

void cargarEjemplo(Trie& t) {
    t.insertar("The Matrix", 0);
    t.insertar("The Matrix Reloaded", 1);
    t.insertar("Ghost Ship", 2);
    t.insertar("Shipwrecked ghosts", 3);
    t.insertar("Casablanca", 4);
}

Pelicula pelicula(const string& titulo, const string& director,
                  const string& casting, const string& genero, const string& sinopsis) {
    Pelicula p(2000, titulo, "American", director, casting, genero, "wiki", sinopsis);
    string t, s;
    for (unsigned char c : titulo) t += static_cast<char>(tolower(c));
    for (unsigned char c : sinopsis) s += static_cast<char>(tolower(c));
    p.setTituloNormalizado(t);
    p.setSinopsisNormalizada(s);
    return p;
}

}

int main() {
    cout << "PRUEBAS DEL ARBOL (TRIE)\n\n";

    prueba("buscar palabra exacta", [] {
        Trie t;
        cargarEjemplo(t);
        verificar(t.buscarPalabra("matrix") == Ids{0, 1}, "matrix -> 0,1");
        verificar(t.buscarPalabra("matri").empty(), "un prefijo no es palabra");
        verificar(t.contiene("ghost") && !t.contiene("gho"), "contiene exacto");
    });

    prueba("mayusculas y signos no importan", [] {
        Trie t;
        cargarEjemplo(t);
        verificar(t.buscarPalabra("MATRIX!") == t.buscarPalabra("matrix"), "case-insensitive");
        verificar(t.buscarSubcadena("ACTION") == t.buscarSubcadena("action"), "case-insensitive subcadena");
    });

    prueba("buscar por prefijo", [] {
        Trie t;
        cargarEjemplo(t);
        verificar(t.buscarPrefijo("ship") == Ids{2, 3}, "ship -> Ship, Shipwrecked");
        verificar(t.buscarPrefijo("ghost") == Ids{2, 3}, "ghost -> ghost, ghosts");
        verificar(t.buscarPrefijo("hip").empty(), "hip no es prefijo de ninguna palabra");
        verificar(t.existePrefijo("rel") && !t.existePrefijo("xyz"), "existePrefijo");
    });

    prueba("buscar por subcadena (trie de sufijos)", [] {
        Trie t;
        cargarEjemplo(t);
        verificar(t.buscarSubcadena("hip") == Ids{2, 3}, "hip esta dentro de ship");
        verificar(t.buscarSubcadena("atri") == Ids{0, 1}, "atri esta dentro de matrix");
        verificar(t.buscarSubcadena("blanc") == Ids{4}, "blanc esta dentro de casablanca");
        verificar(t.buscarSubcadena("zzz").empty(), "inexistente");
    });

    prueba("varias palabras se intersectan", [] {
        Trie t;
        cargarEjemplo(t);
        verificar(t.buscarSubcadena("ghost ship") == Ids{2, 3}, "ghost y ship");
        verificar(t.buscarSubcadena("matrix reloaded") == Ids{1}, "matrix y reloaded");
        verificar(t.buscarSubcadena("matrix ghost").empty(), "ninguna tiene ambas");
    });

    prueba("no repite peliculas", [] {
        Trie t;
        t.insertar("ship ship ship", 7);
        t.insertar("ship", 7);
        verificar(t.buscarPalabra("ship") == Ids{7}, "una sola vez");
        verificar(t.cantidadPalabras() == 1, "una sola palabra");
    });

    prueba("ids fuera de orden quedan ordenados", [] {
        Trie t;
        t.insertar("x", 5);
        t.insertar("x", 1);
        t.insertar("x", 3);
        verificar(t.buscarPalabra("x") == Ids{1, 3, 5}, "ordenados");
    });

    prueba("autocompletar por frecuencia", [] {
        Trie t;
        t.insertar("star", 0);
        t.insertar("star", 1);
        t.insertar("star", 2);
        t.insertar("starship", 1);
        t.insertar("stardust", 0);
        t.insertar("stardust", 2);
        verificar(t.autocompletar("sta", 2) == vector<string>{"star", "stardust"}, "top 2 por frecuencia");
        verificar(t.autocompletar("sta").size() == 3, "todas");
        verificar(t.autocompletar("q").empty(), "sin sugerencias");
    });

    prueba("cuenta nodos compartidos", [] {
        Trie t;
        t.insertar("casa", 0);
        size_t antes = t.cantidadNodos();
        t.insertar("casa", 1);
        verificar(t.cantidadNodos() == antes, "misma palabra no crea nodos");
        t.insertar("caso", 2);
        verificar(t.cantidadNodos() > antes, "palabra nueva crea nodos");
    });

    prueba("consultas vacias o solo signos", [] {
        Trie t;
        cargarEjemplo(t);
        verificar(t.buscarSubcadena("").empty() && t.buscarPrefijo("  ,.").empty(), "vacias");
        verificar(t.autocompletar("").empty(), "autocompletar vacio");
    });

    prueba("limpiar", [] {
        Trie t;
        cargarEjemplo(t);
        t.limpiar();
        verificar(t.cantidadPalabras() == 0 && t.cantidadNodos() == 2, "reiniciado");
        verificar(t.buscarSubcadena("matrix").empty(), "sin resultados");
    });

    prueba("aleatoria: igual que busqueda lineal (2000 consultas)", [] {
        mt19937 r(42);
        uniform_int_distribution<int> letra(0, 3), largo(1, 6), nPal(1, 5);
        auto palabra = [&] {
            string s;
            for (int i = 0, n = largo(r); i < n; ++i) s += static_cast<char>('a' + letra(r));
            return s;
        };

        vector<vector<string>> textos(300);
        Trie t;
        for (int id = 0; id < 300; ++id) {
            string texto;
            for (int k = 0, n = nPal(r); k < n; ++k) {
                textos[id].push_back(palabra());
                texto += textos[id].back() + " ";
            }
            t.insertar(texto, id);
        }

        for (int q = 0; q < 2000; ++q) {
            string consulta = palabra();
            Ids subcadena, prefijo, exacta;
            for (int id = 0; id < 300; ++id) {
                bool s = false, p = false, e = false;
                for (const string& w : textos[id]) {
                    s |= w.find(consulta) != string::npos;
                    p |= w.rfind(consulta, 0) == 0;
                    e |= w == consulta;
                }
                if (s) subcadena.push_back(id);
                if (p) prefijo.push_back(id);
                if (e) exacta.push_back(id);
            }
            verificar(t.buscarSubcadena(consulta) == subcadena, "subcadena " + consulta);
            verificar(t.buscarPrefijo(consulta) == prefijo, "prefijo " + consulta);
            verificar(t.buscarPalabra(consulta) == exacta, "exacta " + consulta);
        }
    });

    cout << "\nPRUEBAS DEL INDEXADOR\n\n";

    vector<Pelicula> catalogo = {
        pelicula("Inception", "Christopher Nolan", "Leonardo DiCaprio, Tom Hardy", "science fiction",
                 "A thief enters dreams"),
        pelicula("Memento", "Christopher Nolan", "Guy Pearce", "thriller", "A man without memory"),
        pelicula("Nolan Story", "Jane Doe", "Christopher Nolan, Tom Hanks", "drama", "An actor named Nolan"),
        pelicula("The Revenant", "Alejandro Inarritu", "Leonardo DiCaprio, Tom Hardy", "drama, western",
                 "A frontiersman survives"),
    };
    Indexador indexador;
    indexador.construirIndice(catalogo);

    prueba("tag respeta el tipo (director vs actor)", [&] {
        verificar(indexador.buscarTag("director", "nolan") == Ids{0, 1}, "Nolan director: 0,1");
        verificar(indexador.buscarTag("actor", "nolan") == Ids{2}, "Nolan actor: solo 2");
        verificar(indexador.buscarTag("casting", "nolan") == Ids{2}, "casting = actor");
    });

    prueba("tag con varios valores separados por coma", [&] {
        verificar(indexador.buscarTag("casting", "Guy Pearce, Tom Hanks") == Ids{1, 2}, "union");
        verificar(indexador.buscarTag("casting", "Leonardo DiCaprio") == Ids{0, 3}, "nombre completo");
        verificar(indexador.buscarTag("genero", "drama") == Ids{2, 3}, "genero");
        verificar(indexador.buscarTag("desconocido", "x").empty(), "tipo inexistente");
    });

    prueba("contenido: titulo y sinopsis", [&] {
        verificar(indexador.buscarContenido("dream") == Ids{0}, "sinopsis");
        verificar(indexador.buscarContenido("memento") == Ids{1}, "titulo");
        verificar(indexador.autocompletar("rev") == vector<string>{"revenant"}, "autocompletar");
    });

    cout << "\nResultado: " << pasadas << " pasadas, " << fallidas << " fallidas\n";
    return fallidas == 0 ? 0 : 1;
}
