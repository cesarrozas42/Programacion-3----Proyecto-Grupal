#ifndef PROYECT_PRGR3_AVANCE_TRIE_H
#define PROYECT_PRGR3_AVANCE_TRIE_H

#include <cstddef>
#include <map>
#include <memory>
#include <string>
#include <vector>

using namespace std;

// =====================================================================
//  ARBOL DE BUSQUEDA (Integrante 1 - Parte 2)
//
//  El Trie guarda DOS arboles que comparten el mismo tipo de nodo:
//
//   1. Trie de PALABRAS (prefijos):  c -> a -> s -> a (*)
//      Cada palabra distinta del catalogo se inserta una sola vez.
//      El nodo donde termina guarda el id de la palabra, y con ese id
//      se obtiene la lista de peliculas que la contienen (indice invertido).
//
//   2. Trie de SUFIJOS: por cada palabra nueva se insertan todos sus
//      sufijos ("casa", "asa", "sa", "a"). Una subcadena de una palabra
//      siempre es el prefijo de alguno de sus sufijos, asi que buscar
//      "asa" es solo bajar 3 nodos en este arbol: ya no se recorre todo.
//
//  Costos (L = largo de la consulta, k = tamano del resultado):
//   - insertar una palabra          O(L) (+ O(L^2) la primera vez, por los sufijos)
//   - buscarPalabra / contiene      O(L)
//   - buscarPrefijo                 O(L + subarbol del prefijo + k)
//   - buscarSubcadena               O(L + subarbol de la subcadena + k)
//
//  Consultas de varias palabras ("ghost ship"): se busca cada palabra y
//  se INTERSECTAN los resultados (peliculas que contienen todas).
//
//  NO es thread-safe al insertar. Las consultas (metodos const) si
//  pueden ejecutarse en paralelo una vez construido el indice.
// =====================================================================

class NodoTrie {
public:
    // Hijos ordenados por caracter (std::map): los recorridos salen en
    // orden alfabetico. unique_ptr libera la memoria sola (sin delete).
    map<char, unique_ptr<NodoTrie>> hijos;

    // Trie de palabras: id de la palabra que termina aqui (-1 si ninguna).
    int idPalabra = -1;

    // Trie de sufijos: ids de las palabras que tienen como sufijo
    // el camino raiz -> este nodo.
    vector<int> palabrasConEsteSufijo;

    NodoTrie* hijo(char c) const;
};

class Trie {
private:
    unique_ptr<NodoTrie> raizPalabras;
    unique_ptr<NodoTrie> raizSufijos;

    // Indice invertido: peliculasPorPalabra[idPalabra] = ids de peliculas
    // (ordenados y sin repetir) que contienen esa palabra.
    vector<vector<int>> peliculasPorPalabra;

    size_t numNodos = 2;      // las dos raices
    int maxIdPelicula = -1;   // para dimensionar la union de resultados

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

    // Separa el texto en palabras (minusculas, solo letras y numeros) y
    // asocia cada una con la pelicula.
    void insertar(const string& texto, int idPelicula);

    // Peliculas que tienen una palabra que EMPIEZA con cada palabra de la consulta.
    vector<int> buscarPrefijo(const string& prefijo) const;

    // Peliculas que tienen una palabra que CONTIENE cada palabra de la consulta.
    vector<int> buscarSubcadena(const string& texto) const;

    // Peliculas que tienen exactamente esa palabra.
    vector<int> buscarPalabra(const string& palabra) const;

    // ¿Existe exactamente esa palabra en el arbol?
    bool contiene(const string& palabra) const;

    // ¿Alguna palabra empieza con ese prefijo?
    bool existePrefijo(const string& prefijo) const;

    // Autocompletado: hasta "limite" palabras que empiezan con el prefijo,
    // de la que aparece en mas peliculas a la que menos.
    vector<string> autocompletar(const string& prefijo, size_t limite = 10) const;

    size_t cantidadPalabras() const;
    size_t cantidadNodos() const;

    // Dibujo en texto del trie de palabras (usar solo con arboles pequenos).
    string dibujar() const;

    void limpiar();
};

#endif
