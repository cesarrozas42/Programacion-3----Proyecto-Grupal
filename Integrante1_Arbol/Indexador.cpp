#include "Indexador.h"

#include <algorithm>
#include <cctype>
#include <iterator>

using namespace std;

Indexador::Indexador() = default;

string Indexador::tipoCanonico(const string& tipo) {
    string t;
    for (unsigned char c : tipo) {
        t += static_cast<char>(tolower(c));
    }
    if (t == "actor" || t == "reparto") {
        return "casting";
    }
    return t;
}

vector<string> Indexador::separarValores(const string& valor) {
    vector<string> valores;
    string actual;
    for (char c : valor) {
        if (c == ',') {
            valores.push_back(actual);
            actual.clear();
        } else {
            actual += c;
        }
    }
    valores.push_back(actual);
    return valores;
}

void Indexador::construirIndice(const vector<Pelicula>& peliculas) {
    limpiar();

    for (int id = 0; id < static_cast<int>(peliculas.size()); ++id) {
        const Pelicula& pelicula = peliculas[id];

        indiceContenido.insertar(pelicula.getTituloNormalizado(), id);
        indiceContenido.insertar(pelicula.getSinopsisNormalizada(), id);

        for (const Tag& tag : pelicula.getTags()) {
            indicesTags[tipoCanonico(tag.getTipo())].insertar(tag.getValor(), id);
        }
    }
}

vector<int> Indexador::buscarContenido(const string& consulta) const {
    return indiceContenido.buscarSubcadena(consulta);
}

vector<int> Indexador::buscarTag(const string& tipo, const string& valor) const {
    const Trie* indice = getIndiceTag(tipo);
    if (indice == nullptr) {
        return {};
    }

    vector<int> resultado;
    for (const string& elemento : separarValores(valor)) {
        vector<int> ids = indice->buscarSubcadena(elemento);
        vector<int> unidos;
        set_union(resultado.begin(), resultado.end(), ids.begin(), ids.end(),
                  back_inserter(unidos));
        resultado = move(unidos);
    }
    return resultado;
}

vector<string> Indexador::autocompletar(const string& prefijo, size_t limite) const {
    return indiceContenido.autocompletar(prefijo, limite);
}

const Trie& Indexador::getIndiceContenido() const {
    return indiceContenido;
}

const Trie* Indexador::getIndiceTag(const string& tipo) const {
    auto it = indicesTags.find(tipoCanonico(tipo));
    return it == indicesTags.end() ? nullptr : &it->second;
}

void Indexador::limpiar() {
    indiceContenido.limpiar();
    indicesTags.clear();
}
