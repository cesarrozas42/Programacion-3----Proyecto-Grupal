#include "Trie.h"

#include <algorithm>
#include <cctype>

NodoTrie::NodoTrie(char caracter) : caracter(caracter), finPalabra(false) {}

char NodoTrie::getCaracter() const {
    return caracter;
}

bool NodoTrie::esFinPalabra() const {
    return finPalabra;
}

unordered_map<char, NodoTrie*>& NodoTrie::getHijos() {
    return hijos;
}

const unordered_map<char, NodoTrie*>& NodoTrie::getHijos() const {
    return hijos;
}

const unordered_set<int>& NodoTrie::getPeliculas() const {
    return peliculas;
}

void NodoTrie::marcarFinPalabra() {
    finPalabra = true;
}

void NodoTrie::agregarPelicula(int idPelicula) {
    peliculas.insert(idPelicula);
}

Trie::Trie() : raiz(new NodoTrie('\0')) {}

Trie::~Trie() {
    liberar(raiz);
}

void Trie::liberar(NodoTrie* nodo) {
    if (nodo == nullptr) {
        return;
    }

    for (auto& par : nodo->getHijos()) {
        liberar(par.second);
    }

    delete nodo;
}

void Trie::limpiar() {
    liberar(raiz);
    raiz = new NodoTrie('\0');
}

void Trie::normalizar(const string& texto, vector<string>& palabras) const {
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
}

void Trie::insertar(const string& texto, int idPelicula) {
    vector<string> palabras;
    normalizar(texto, palabras);

    for (const string& palabra : palabras) {
        NodoTrie* actual = raiz;

        for (char c : palabra) {
            auto it = actual->getHijos().find(c);

            if (it == actual->getHijos().end()) {
                actual->getHijos()[c] = new NodoTrie(c);
                it = actual->getHijos().find(c);
            }

            actual = it->second;
            actual->agregarPelicula(idPelicula);
        }

        actual->marcarFinPalabra();
    }
}

vector<int> Trie::buscarPrefijo(const string& prefijo) const {
    vector<string> palabras;
    normalizar(prefijo, palabras);

    if (palabras.empty()) {
        return {};
    }

    const string& consulta = palabras[0];
    NodoTrie* actual = raiz;

    for (char c : consulta) {
        auto it = actual->getHijos().find(c);

        if (it == actual->getHijos().end()) {
            return {};
        }

        actual = it->second;
    }

    vector<int> resultados(actual->getPeliculas().begin(),
                           actual->getPeliculas().end());
    sort(resultados.begin(), resultados.end());
    return resultados;
}

void Trie::buscarDesdeNodo(NodoTrie* nodo,
                           const string& consulta,
                           int posicion,
                           unordered_set<int>& resultados) const {
    if (nodo == nullptr) {
        return;
    }

    if (posicion == static_cast<int>(consulta.size())) {
        for (int id : nodo->getPeliculas()) {
            resultados.insert(id);
        }
        return;
    }

    auto it = nodo->getHijos().find(consulta[posicion]);

    if (it != nodo->getHijos().end()) {
        buscarDesdeNodo(it->second, consulta, posicion + 1, resultados);
    }
}

void Trie::recorrerParaSubcadena(NodoTrie* nodo,
                                 const string& consulta,
                                 unordered_set<int>& resultados) const {
    if (nodo == nullptr) {
        return;
    }

    buscarDesdeNodo(nodo, consulta, 0, resultados);

    for (auto& par : nodo->getHijos()) {
        recorrerParaSubcadena(par.second, consulta, resultados);
    }
}

vector<int> Trie::buscarSubcadena(const string& texto) const {
    vector<string> palabras;
    normalizar(texto, palabras);

    if (palabras.empty()) {
        return {};
    }

    const string& consulta = palabras[0];
    unordered_set<int> resultados;

    recorrerParaSubcadena(raiz, consulta, resultados);

    vector<int> salida(resultados.begin(), resultados.end());
    sort(salida.begin(), salida.end());
    return salida;
}

bool Trie::contiene(const string& palabra) const {
    vector<int> resultados = buscarPrefijo(palabra);
    return !resultados.empty();
}
