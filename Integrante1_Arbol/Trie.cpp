#include "Trie.h"

#include <algorithm>
#include <cctype>
#include <iterator>

NodoTrie* NodoTrie::hijo(char c) const {
    auto it = hijos.find(c);
    return it == hijos.end() ? nullptr : it->second.get();
}

Trie::Trie()
    : raizPalabras(make_unique<NodoTrie>()),
      raizSufijos(make_unique<NodoTrie>()) {}

void Trie::limpiar() {
    raizPalabras = make_unique<NodoTrie>();
    raizSufijos = make_unique<NodoTrie>();
    peliculasPorPalabra.clear();
    numNodos = 2;
    maxIdPelicula = -1;
}

NodoTrie* Trie::hijoOCrear(NodoTrie* nodo, char c) {
    unique_ptr<NodoTrie>& hijo = nodo->hijos[c];
    if (!hijo) {
        hijo = make_unique<NodoTrie>();
        numNodos++;
    }
    return hijo.get();
}

vector<string> Trie::normalizar(const string& texto) {
    vector<string> palabras;
    string palabra;

    for (unsigned char c : texto) {
        if (isalnum(c)) {
            palabra += static_cast<char>(tolower(c));
        } else if (!palabra.empty()) {
            palabras.push_back(palabra);
            palabra.clear();
        }
    }
    if (!palabra.empty()) {
        palabras.push_back(palabra);
    }
    return palabras;
}

void Trie::agregarOrdenado(vector<int>& ids, int id) {
    if (ids.empty() || ids.back() < id) {
        ids.push_back(id);
        return;
    }
    auto it = lower_bound(ids.begin(), ids.end(), id);
    if (it == ids.end() || *it != id) {
        ids.insert(it, id);
    }
}

void Trie::insertar(const string& texto, int idPelicula) {
    if (idPelicula < 0) {
        return;
    }
    maxIdPelicula = max(maxIdPelicula, idPelicula);

    for (const string& palabra : normalizar(texto)) {
        registrarPalabra(palabra, idPelicula);
    }
}

void Trie::registrarPalabra(const string& palabra, int idPelicula) {
    NodoTrie* actual = raizPalabras.get();
    for (char c : palabra) {
        actual = hijoOCrear(actual, c);
    }

    if (actual->idPalabra == -1) {
        actual->idPalabra = static_cast<int>(peliculasPorPalabra.size());
        peliculasPorPalabra.emplace_back();
        insertarSufijos(palabra, actual->idPalabra);
    }

    agregarOrdenado(peliculasPorPalabra[actual->idPalabra], idPelicula);
}

void Trie::insertarSufijos(const string& palabra, int idPalabra) {
    for (size_t inicio = 0; inicio < palabra.size(); ++inicio) {
        NodoTrie* actual = raizSufijos.get();
        for (size_t i = inicio; i < palabra.size(); ++i) {
            actual = hijoOCrear(actual, palabra[i]);
        }
        actual->palabrasConEsteSufijo.push_back(idPalabra);
    }
}

const NodoTrie* Trie::bajar(const NodoTrie* desde, const string& camino) {
    const NodoTrie* actual = desde;
    for (char c : camino) {
        actual = actual->hijo(c);
        if (actual == nullptr) {
            return nullptr;
        }
    }
    return actual;
}

void Trie::palabrasDelSubarbol(const NodoTrie* nodo, vector<int>& salida) {
    if (nodo->idPalabra != -1) {
        salida.push_back(nodo->idPalabra);
    }
    for (const auto& [c, hijo] : nodo->hijos) {
        palabrasDelSubarbol(hijo.get(), salida);
    }
}

void Trie::palabrasDelSubarbolSufijos(const NodoTrie* nodo, vector<int>& salida) {
    salida.insert(salida.end(),
                  nodo->palabrasConEsteSufijo.begin(),
                  nodo->palabrasConEsteSufijo.end());
    for (const auto& [c, hijo] : nodo->hijos) {
        palabrasDelSubarbolSufijos(hijo.get(), salida);
    }
}

vector<int> Trie::unirPeliculas(const vector<int>& idsPalabras) const {
    if (idsPalabras.size() == 1) {
        return peliculasPorPalabra[idsPalabras[0]];
    }
    vector<char> marcada(static_cast<size_t>(maxIdPelicula + 1), 0);
    for (int idPalabra : idsPalabras) {
        for (int idPelicula : peliculasPorPalabra[idPalabra]) {
            marcada[idPelicula] = 1;
        }
    }
    vector<int> resultado;
    for (int id = 0; id <= maxIdPelicula; ++id) {
        if (marcada[id]) {
            resultado.push_back(id);
        }
    }
    return resultado;
}

vector<int> Trie::peliculasConPrefijo(const string& palabra) const {
    const NodoTrie* nodo = bajar(raizPalabras.get(), palabra);
    if (nodo == nullptr) {
        return {};
    }
    vector<int> idsPalabras;
    palabrasDelSubarbol(nodo, idsPalabras);
    return unirPeliculas(idsPalabras);
}

vector<int> Trie::peliculasConSubcadena(const string& palabra) const {
    const NodoTrie* nodo = bajar(raizSufijos.get(), palabra);
    if (nodo == nullptr) {
        return {};
    }
    vector<int> idsPalabras;
    palabrasDelSubarbolSufijos(nodo, idsPalabras);
    return unirPeliculas(idsPalabras);
}

vector<int> Trie::intersectar(const vector<int>& a, const vector<int>& b) {
    vector<int> resultado;
    set_intersection(a.begin(), a.end(), b.begin(), b.end(), back_inserter(resultado));
    return resultado;
}

template <typename BuscarUnaPalabra>
vector<int> Trie::buscarTodas(const string& texto, BuscarUnaPalabra buscarUna) const {
    vector<string> palabras = normalizar(texto);
    if (palabras.empty()) {
        return {};
    }
    vector<int> resultado = buscarUna(palabras[0]);
    for (size_t i = 1; i < palabras.size() && !resultado.empty(); ++i) {
        resultado = intersectar(resultado, buscarUna(palabras[i]));
    }
    return resultado;
}

vector<int> Trie::buscarPrefijo(const string& prefijo) const {
    return buscarTodas(prefijo, [this](const string& p) { return peliculasConPrefijo(p); });
}

vector<int> Trie::buscarSubcadena(const string& texto) const {
    return buscarTodas(texto, [this](const string& p) { return peliculasConSubcadena(p); });
}

vector<int> Trie::buscarPalabra(const string& palabra) const {
    return buscarTodas(palabra, [this](const string& p) -> vector<int> {
        const NodoTrie* nodo = bajar(raizPalabras.get(), p);
        if (nodo == nullptr || nodo->idPalabra == -1) {
            return {};
        }
        return peliculasPorPalabra[nodo->idPalabra];
    });
}

bool Trie::contiene(const string& palabra) const {
    vector<string> palabras = normalizar(palabra);
    if (palabras.size() != 1) {
        return false;
    }
    const NodoTrie* nodo = bajar(raizPalabras.get(), palabras[0]);
    return nodo != nullptr && nodo->idPalabra != -1;
}

bool Trie::existePrefijo(const string& prefijo) const {
    vector<string> palabras = normalizar(prefijo);
    return palabras.size() == 1 && bajar(raizPalabras.get(), palabras[0]) != nullptr;
}

void Trie::sugerencias(const NodoTrie* nodo, string& camino,
                       vector<pair<int, string>>& salida) const {
    if (nodo->idPalabra != -1) {
        int frecuencia = static_cast<int>(peliculasPorPalabra[nodo->idPalabra].size());
        salida.emplace_back(frecuencia, camino);
    }
    for (const auto& [c, hijo] : nodo->hijos) {
        camino.push_back(c);
        sugerencias(hijo.get(), camino, salida);
        camino.pop_back();
    }
}

vector<string> Trie::autocompletar(const string& prefijo, size_t limite) const {
    vector<string> palabras = normalizar(prefijo);
    if (palabras.size() != 1 || limite == 0) {
        return {};
    }
    const NodoTrie* nodo = bajar(raizPalabras.get(), palabras[0]);
    if (nodo == nullptr) {
        return {};
    }

    vector<pair<int, string>> candidatas;
    string camino = palabras[0];
    sugerencias(nodo, camino, candidatas);

    size_t n = min(limite, candidatas.size());
    partial_sort(candidatas.begin(), candidatas.begin() + static_cast<long>(n), candidatas.end(),
                 [](const auto& a, const auto& b) {
                     if (a.first != b.first) return a.first > b.first;
                     return a.second < b.second;
                 });

    vector<string> resultado;
    for (size_t i = 0; i < n; ++i) {
        resultado.push_back(candidatas[i].second);
    }
    return resultado;
}

size_t Trie::cantidadPalabras() const {
    return peliculasPorPalabra.size();
}

size_t Trie::cantidadNodos() const {
    return numNodos;
}

string Trie::dibujar() const {
    string salida = "(raiz)\n";
    dibujar(raizPalabras.get(), "", salida);
    return salida;
}

void Trie::dibujar(const NodoTrie* nodo, const string& sangria, string& salida) const {
    size_t i = 0;
    for (const auto& [c, hijo] : nodo->hijos) {
        bool ultimo = ++i == nodo->hijos.size();
        salida += sangria;
        salida += ultimo ? "`-- " : "|-- ";
        salida += c;
        if (hijo->idPalabra != -1) {
            salida += " *" + to_string(peliculasPorPalabra[hijo->idPalabra].size());
        }
        salida += '\n';
        dibujar(hijo.get(), sangria + (ultimo ? "    " : "|   "), salida);
    }
}
