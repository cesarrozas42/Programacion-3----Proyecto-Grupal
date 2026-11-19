#ifndef PROYECT_PRGR3_AVANCE_TRIE_H
#define PROYECT_PRGR3_AVANCE_TRIE_H

#include <cstddef>
#include <map>
#include <memory>
#include <string>
#include <vector>

using namespace std;

class NodoTrie {
public:
    map<char, unique_ptr<NodoTrie>> hijos;

    int idPalabra = -1;

    vector<int> palabrasConEsteSufijo;

    NodoTrie* hijo(char c) const;
};

class Trie {
private:
    unique_ptr<NodoTrie> raizPalabras;
    unique_ptr<NodoTrie> raizSufijos;

    vector<vector<int>> peliculasPorPalabra;

    size_t numNodos = 2;
    int maxIdPelicula = -1;

    static vector<string> normalizar(const string& texto);
    static void agregarOrdenado(vector<int>& ids, int id);
    static vector<int> intersectar(const vector<int>& a, const vector<int>& b);
    static const NodoTrie* bajar(const NodoTrie* desde, const string& camino);

    NodoTrie* hijoOCrear(NodoTrie* nodo, char c);
    void registrarPalabra(const string& palabra, int idPelicula);
    void insertarSufijos(const string& palabra, int idPalabra);

    vector<int> unirPeliculas(const vector<int>& idsPalabras) const;
    vector<int> peliculasConPrefijo(const string& palabra) const;
    vector<int> peliculasConSubcadena(const string& palabra) const;

    template <typename BuscarUnaPalabra>
    vector<int> buscarTodas(const string& texto, BuscarUnaPalabra buscarUna) const;

    static void palabrasDelSubarbol(const NodoTrie* nodo, vector<int>& salida);
    static void palabrasDelSubarbolSufijos(const NodoTrie* nodo, vector<int>& salida);
    void sugerencias(const NodoTrie* nodo, string& camino,
                     vector<pair<int, string>>& salida) const;
    void dibujar(const NodoTrie* nodo, const string& sangria, string& salida) const;

public:
    Trie();

    Trie(const Trie&) = delete;
    Trie& operator=(const Trie&) = delete;

    void insertar(const string& texto, int idPelicula);

    vector<int> buscarPrefijo(const string& prefijo) const;

    vector<int> buscarSubcadena(const string& texto) const;

    vector<int> buscarPalabra(const string& palabra) const;

    bool contiene(const string& palabra) const;

    bool existePrefijo(const string& prefijo) const;

    vector<string> autocompletar(const string& prefijo, size_t limite = 10) const;

    size_t cantidadPalabras() const;
    size_t cantidadNodos() const;

    string dibujar() const;

    void limpiar();
};

#endif
