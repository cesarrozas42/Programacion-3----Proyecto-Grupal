#ifndef PROYECT_PRGR3_AVANCE_TRIE_H
#define PROYECT_PRGR3_AVANCE_TRIE_H

#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

using namespace std;

class NodoTrie {
private:
    char caracter;
    bool finPalabra;
    unordered_map<char, NodoTrie*> hijos;
    unordered_set<int> peliculas;

public:
    explicit NodoTrie(char caracter);

    char getCaracter() const;
    bool esFinPalabra() const;

    unordered_map<char, NodoTrie*>& getHijos();
    const unordered_map<char, NodoTrie*>& getHijos() const;

    const unordered_set<int>& getPeliculas() const;

    void marcarFinPalabra();
    void agregarPelicula(int idPelicula);
};

class Trie {
private:
    NodoTrie* raiz;

    void liberar(NodoTrie* nodo);
    void normalizar(const string& texto, vector<string>& palabras) const;
    void buscarDesdeNodo(NodoTrie* nodo,
                         const string& consulta,
                         int posicion,
                         unordered_set<int>& resultados) const;
    void recorrerParaSubcadena(NodoTrie* nodo,
                               const string& consulta,
                               unordered_set<int>& resultados) const;

public:
    Trie();
    ~Trie();

    Trie(const Trie&) = delete;
    Trie& operator=(const Trie&) = delete;

    void insertar(const string& texto, int idPelicula);

    vector<int> buscarPrefijo(const string& prefijo) const;
    vector<int> buscarSubcadena(const string& texto) const;
    bool contiene(const string& palabra) const;

    void limpiar();
};

#endif
