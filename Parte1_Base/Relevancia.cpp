#include "Relevancia.h"

#include <algorithm>
#include <cctype>
#include <unordered_set>

using namespace std;

Relevancia::Relevancia(const vector<Pelicula>& catalogo, const Indexador& indexador)
    : catalogo(catalogo), indexador(indexador) {}

string Relevancia::normalizarSimple(const string& texto) const {
    // Misma idea que CSVReader::normalizarTexto: minusculas y solo
    // caracteres alfanumericos (el resto se vuelve espacio), para que
    // coincida con tituloNormalizado / sinopsisNormalizada.
    string resultado;
    for (unsigned char c : texto) {
        if (isalnum(c)) {
            resultado += static_cast<char>(tolower(c));
        } else {
            resultado += ' ';
        }
    }
    return resultado;
}

vector<string> Relevancia::separarPalabras(const string& consulta) const {
    vector<string> palabras;
    string palabra;

    for (char c : consulta) {
        if (c == ' ') {
            if (!palabra.empty()) {
                palabras.push_back(palabra);
                palabra.clear();
            }
        } else {
            palabra += c;
        }
    }

    if (!palabra.empty()) {
        palabras.push_back(palabra);
    }

    return palabras;
}

double Relevancia::calcularScore(int idPelicula,
                                 const vector<string>& palabras,
                                 const string& consultaNormalizada) const {
    const Pelicula& pelicula = catalogo[idPelicula];
    const string& titulo = pelicula.getTituloNormalizado();
    const string& sinopsis = pelicula.getSinopsisNormalizada();

    double score = 0.0;

    for (const string& palabra : palabras) {
        if (titulo.find(palabra) != string::npos) {
            score += 10.0;
        }
        if (sinopsis.find(palabra) != string::npos) {
            score += 1.0;
        }
    }

    if (palabras.size() > 1) {
        if (titulo.find(consultaNormalizada) != string::npos) {
            score += 50.0;
        } else if (sinopsis.find(consultaNormalizada) != string::npos) {
            score += 10.0;
        }
    }

    return score;
}

vector<ResultadoBusqueda> Relevancia::buscar(const string& consulta) const {
    string consultaNormalizada = normalizarSimple(consulta);
    vector<string> palabras = separarPalabras(consultaNormalizada);

    if (palabras.empty()) {
        return {};
    }

    // El Trie solo procesa la primera palabra de lo que se le pase,
    // asi que consultamos el indice una vez por cada palabra y unimos
    // los candidatos.
    unordered_set<int> candidatos;
    for (const string& palabra : palabras) {
        vector<int> parciales = indexador.buscarContenido(palabra);
        candidatos.insert(parciales.begin(), parciales.end());
    }

    vector<ResultadoBusqueda> resultados;
    resultados.reserve(candidatos.size());

    for (int id : candidatos) {
        double score = calcularScore(id, palabras, consultaNormalizada);
        resultados.push_back({id, score});
    }

    sort(resultados.begin(), resultados.end(),
         [](const ResultadoBusqueda& a, const ResultadoBusqueda& b) {
             if (a.score != b.score) {
                 return a.score > b.score;
             }
             return a.idPelicula < b.idPelicula;
         });

    return resultados;
}

vector<int> Relevancia::obtenerPagina(const vector<ResultadoBusqueda>& resultados,
                                      int pagina,
                                      int tamanoPagina) const {
    if (pagina < 1 || tamanoPagina <= 0) {
        return {};
    }

    int inicio = (pagina - 1) * tamanoPagina;
    if (inicio >= static_cast<int>(resultados.size())) {
        return {};
    }

    int fin = min(inicio + tamanoPagina, static_cast<int>(resultados.size()));

    vector<int> pagina_ids;
    for (int i = inicio; i < fin; ++i) {
        pagina_ids.push_back(resultados[i].idPelicula);
    }

    return pagina_ids;
}
