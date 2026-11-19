// Demo del arbol para la exposicion (Integrante 1 - Parte 2).
// Uso: demo_arbol [ruta_al_csv]   (por defecto wiki_movie_plots_deduped.csv)
#include <chrono>
#include <cstdio>
#include <iostream>
#include <string>
#include <vector>

#include "CSVReader.h"
#include "Indexador.h"
#include "Trie.h"

using namespace std;
using reloj = chrono::steady_clock;

namespace {

double msDesde(reloj::time_point inicio) {
    return chrono::duration<double, milli>(reloj::now() - inicio).count();
}

void titulo(const string& t) {
    cout << "\n=== " << t << " ===\n";
}

// Lo que hacia el motor de la parte 1: recorrer todo el catalogo con find().
vector<int> busquedaLineal(const vector<Pelicula>& peliculas, const string& consulta) {
    vector<int> ids;
    for (int id = 0; id < static_cast<int>(peliculas.size()); ++id) {
        if (peliculas[id].getTituloNormalizado().find(consulta) != string::npos ||
            peliculas[id].getSinopsisNormalizada().find(consulta) != string::npos) {
            ids.push_back(id);
        }
    }
    return ids;
}

void demoPequena() {
    titulo("1. Arbol pequeno: como se ve por dentro (*n = en cuantas peliculas aparece)");
    Trie t;
    t.insertar("casa casco", 0);
    t.insertar("cama", 1);
    t.insertar("casa", 2);
    cout << t.dibujar();
    cout << "palabras = " << t.cantidadPalabras() << ", nodos (palabras + sufijos) = "
         << t.cantidadNodos() << '\n';
    cout << "buscarPrefijo(\"cas\")   -> " << t.buscarPrefijo("cas").size() << " peliculas (0 y 2)\n";
    cout << "buscarSubcadena(\"asc\") -> " << t.buscarSubcadena("asc").size() << " pelicula (casco)\n";
}

} // namespace

int main(int argc, char** argv) {
#ifdef _WIN32
    system("chcp 65001 > nul");
#endif
    demoPequena();

    string ruta = argc > 1 ? argv[1] : "wiki_movie_plots_deduped.csv";
    titulo("2. Construir el indice con el catalogo real");
    auto inicio = reloj::now();
    CSVReader lector;
    vector<Pelicula> peliculas = lector.leerArchivo(ruta);
    if (peliculas.empty()) {
        cout << "No se encontro el CSV en: " << ruta << '\n';
        return 1;
    }
    printf("Cargar CSV:        %8.0f ms\n", msDesde(inicio));

    inicio = reloj::now();
    Indexador indexador;
    indexador.construirIndice(peliculas);
    printf("Construir indice:  %8.0f ms\n", msDesde(inicio));

    const Trie& contenido = indexador.getIndiceContenido();
    printf("Palabras distintas: %zu   Nodos: %zu\n", contenido.cantidadPalabras(),
           contenido.cantidadNodos());

    titulo("3. Arbol vs busqueda lineal de la parte 1 (promedio de 20 repeticiones)");
    printf("%-10s %10s %14s %14s %9s\n", "consulta", "resultados", "lineal (ms)", "arbol (ms)", "mejora");
    for (string q : {"matrix", "ghost", "bar", "zombie", "xqzw"}) {
        const int repeticiones = 20;
        vector<int> lineal, arbol;

        inicio = reloj::now();
        for (int i = 0; i < repeticiones; ++i) lineal = busquedaLineal(peliculas, q);
        double msLineal = msDesde(inicio) / repeticiones;

        inicio = reloj::now();
        for (int i = 0; i < repeticiones; ++i) arbol = indexador.buscarContenido(q);
        double msArbol = msDesde(inicio) / repeticiones;

        // La busqueda lineal tambien encuentra frases que cruzan palabras;
        // para una sola palabra ambos deben coincidir.
        printf("%-10s %10zu %14.3f %14.3f %8.0fx %s\n", q.c_str(), arbol.size(), msLineal, msArbol,
               msLineal / max(msArbol, 0.001), lineal == arbol ? "" : "(difiere!)");
    }

    titulo("4. Autocompletado (para la interfaz grafica)");
    for (string p : {"star", "zom", "detec"}) {
        cout << p << " -> ";
        for (const string& s : indexador.autocompletar(p, 6)) cout << s << "  ";
        cout << '\n';
    }

    titulo("5. Busqueda por tags: un arbol por tipo");
    auto mostrar = [&](const string& tipo, const string& valor) {
        vector<int> ids = indexador.buscarTag(tipo, valor);
        cout << tipo << "(\"" << valor << "\") -> " << ids.size() << " peliculas";
        if (!ids.empty()) cout << ", ej: " << peliculas[ids[0]].getTitulo();
        cout << '\n';
    };
    mostrar("director", "Christopher Nolan");
    mostrar("actor", "Christopher Nolan");
    mostrar("genero", "drama");
    mostrar("actor", "Keanu Reeves");
    return 0;
}
