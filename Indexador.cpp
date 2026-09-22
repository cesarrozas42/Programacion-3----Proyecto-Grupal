#include "Indexador.h"

#include <algorithm>
#include <cctype>
#include <unordered_set>

using namespace std;

Indexador::Indexador() = default;

void Indexador::construirIndice(const vector<Pelicula>& peliculas) {
    limpiar();

    for (int id = 0; id < static_cast<int>(peliculas.size()); ++id) {
        const Pelicula& pelicula = peliculas[id];

        indiceContenido.insertar(pelicula.getTituloNormalizado(), id);
        indiceContenido.insertar(pelicula.getSinopsisNormalizada(), id);

        indiceTags.insertar(pelicula.getDirector(), id);
        indiceTags.insertar(pelicula.getCasting(), id);
        indiceTags.insertar(pelicula.getGenero(), id);
        indiceTags.insertar(pelicula.getOrigen(), id);

        for (const Tag& tag : pelicula.getTags()) {
            indiceTags.insertar(tag.getTipo(), id);
            indiceTags.insertar(tag.getValor(), id);
        }
    }
}

vector<int> Indexador::buscarContenido(const string& consulta) const {
    return indiceContenido.buscarSubcadena(consulta);
}

vector<int> Indexador::buscarTag(const string& tipo,
                                 const string& valor) const {
    vector<int> porTipo = indiceTags.buscarSubcadena(tipo);
    vector<int> porValor = indiceTags.buscarSubcadena(valor);

    unordered_set<int> valores(porValor.begin(), porValor.end());
    vector<int> resultados;

    for (int id : porTipo) {
        if (valores.find(id) != valores.end()) {
            resultados.push_back(id);
        }
    }

    sort(resultados.begin(), resultados.end());
    return resultados;
}

const Trie& Indexador::getIndiceContenido() const {
    return indiceContenido;
}

const Trie& Indexador::getIndiceTags() const {
    return indiceTags;
}

void Indexador::limpiar() {
    indiceContenido.limpiar();
    indiceTags.limpiar();
}
