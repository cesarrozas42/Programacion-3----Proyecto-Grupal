#include <iostream>
#include <cassert>
#include <vector>

#include "Pelicula.h"
#include "Indexador.h"
#include "Relevancia.h"
#include "Recomendador.h"
#include "Usuario.h"

using namespace std;

// Construye un catalogo pequeño y controlado para poder predecir
// exactamente que deberia devolver cada busqueda/recomendacion.
vector<Pelicula> construirCatalogoDePrueba() {
    vector<Pelicula> peliculas;

    // id 0
    Pelicula p0(1999, "The Matrix", "USA", "Wachowski",
                "Keanu Reeves", "accion", "wiki/matrix",
                "A hacker discovers the ghost ship of reality itself");
    p0.setTituloNormalizado("the matrix");
    p0.setSinopsisNormalizada("a hacker discovers the ghost ship of reality itself");
    peliculas.push_back(p0);

    // id 1
    Pelicula p1(2003, "The Matrix Reloaded", "USA", "Wachowski",
                "Keanu Reeves", "accion", "wiki/matrix2",
                "The war against the machines continues");
    p1.setTituloNormalizado("the matrix reloaded");
    p1.setSinopsisNormalizada("the war against the machines continues");
    peliculas.push_back(p1);

    // id 2
    Pelicula p2(2010, "Ghost Ship", "USA", "Steve Beck",
                "Julianna Margulies", "terror", "wiki/ghostship",
                "A salvage crew finds an abandoned ocean liner");
    p2.setTituloNormalizado("ghost ship");
    p2.setSinopsisNormalizada("a salvage crew finds an abandoned ocean liner");
    peliculas.push_back(p2);

    // id 3 (no deberia matchear "matrix" ni "ghost")
    Pelicula p3(1994, "Forrest Gump", "USA", "Robert Zemeckis",
                "Tom Hanks", "drama", "wiki/forrestgump",
                "A slow witted man recounts his extraordinary life");
    p3.setTituloNormalizado("forrest gump");
    p3.setSinopsisNormalizada("a slow witted man recounts his extraordinary life");
    peliculas.push_back(p3);

    return peliculas;
}

//  Se recibe por referencia para evitar el retorno por valor de un objeto no copiable (Trie)
void construirIndiceDePrueba(const vector<Pelicula>& peliculas, Indexador& indexador) {
    indexador.construirIndice(peliculas);
}

void probarRelevanciaBusquedaSimple(const vector<Pelicula>& peliculas,
                                    const Indexador& indexador) {
    Relevancia relevancia(peliculas, indexador);

    auto resultados = relevancia.buscar("matrix");

    cout << "[TEST] buscar('matrix'): " << resultados.size()
         << " resultados.\n";

    // id 0 y 1 tienen "matrix" en el titulo, deberian aparecer
    // primero y con score alto (10 c/u).
    assert(resultados.size() == 2);
    assert(resultados[0].score == 10.0);
    assert(resultados[1].score == 10.0);
    cout << "  -> PASADO: solo matchea peliculas con 'matrix' en el titulo.\n";
}

void probarRelevanciaFraseCompleta(const vector<Pelicula>& peliculas,
                                   const Indexador& indexador) {
    Relevancia relevancia(peliculas, indexador);

    auto resultados = relevancia.buscar("ghost ship");

    cout << "[TEST] buscar('ghost ship'): " << resultados.size()
         << " resultados.\n";

    // id 2 ("Ghost Ship") deberia salir primero: matchea ambas
    // palabras en el titulo (2x10) + bonus de frase completa en
    // titulo (+50) = 70.
    // id 0 tiene "ghost" y "ship" solo en la sinopsis (2x1 = 2).
    assert(!resultados.empty());
    assert(resultados[0].idPelicula == 2);
    assert(resultados[0].score == 70.0);
    cout << "  -> PASADO: la coincidencia de titulo + frase completa "
         << "queda primera.\n";
}

void probarPaginacion(const vector<Pelicula>& peliculas,
                      const Indexador& indexador) {
    Relevancia relevancia(peliculas, indexador);

    auto resultados = relevancia.buscar("matrix");
    auto pagina1 = relevancia.obtenerPagina(resultados, 1, 1);
    auto pagina2 = relevancia.obtenerPagina(resultados, 2, 1);
    auto pagina3 = relevancia.obtenerPagina(resultados, 3, 1);

    cout << "[TEST] paginacion de a 1: pagina1=" << pagina1.size()
         << " pagina2=" << pagina2.size()
         << " pagina3=" << pagina3.size() << "\n";

    assert(pagina1.size() == 1);
    assert(pagina2.size() == 1);
    assert(pagina3.empty()); // ya no hay mas resultados
    cout << "  -> PASADO: pagina vacia cuando ya no hay mas resultados.\n";
}

void probarRecomendador(const vector<Pelicula>& peliculas,
                        const Indexador& indexador) {
    Recomendador recomendador(peliculas, indexador);

    Usuario usuario(1, "Edi");
    usuario.agregarLike(0); // le gusto "The Matrix" (Wachowski, Keanu Reeves)

    auto recomendadas = recomendador.recomendar(usuario, 5);

    cout << "[TEST] recomendar() tras like a 'The Matrix': "
         << recomendadas.size() << " recomendaciones.\n";

    // id 1 comparte director (Wachowski), actor (Keanu Reeves) y
    // genero (accion) con id 0, deberia ser la mas recomendada.
    // id 0 nunca deberia aparecer (ya esta likeada).
    assert(!recomendadas.empty());
    assert(recomendadas[0] == 1);
    for (int id : recomendadas) {
        assert(id != 0);
    }
    cout << "  -> PASADO: recomienda por tags en comun y excluye "
         << "lo ya likeado.\n";
}

void probarUsuarioSinDuplicados() {
    Usuario usuario(1, "Edi");

    bool primeraVez = usuario.agregarLike(5);
    bool segundaVez = usuario.agregarLike(5);

    cout << "[TEST] agregarLike duplicado: primeraVez=" << primeraVez
         << " segundaVez=" << segundaVez << "\n";

    assert(primeraVez == true);
    assert(segundaVez == false);
    assert(usuario.getLikes().size() == 1);

    bool quitado = usuario.quitarLike(5);
    bool quitadoDeNuevo = usuario.quitarLike(5);

    assert(quitado == true);
    assert(quitadoDeNuevo == false);
    assert(usuario.getLikes().empty());
    cout << "  -> PASADO: sin duplicados, y quitarLike funciona.\n";
}

int main() {
    cout << "PRUEBAS DE RELEVANCIA Y RECOMENDADOR \n\n";

    vector<Pelicula> peliculas = construirCatalogoDePrueba();

    // CORREGIDO: Se instancia localmente el objeto y se pasa para llenarse
    Indexador indexador;
    construirIndiceDePrueba(peliculas, indexador);

    probarRelevanciaBusquedaSimple(peliculas, indexador);
    probarRelevanciaFraseCompleta(peliculas, indexador);
    probarPaginacion(peliculas, indexador);
    probarRecomendador(peliculas, indexador);
    probarUsuarioSinDuplicados();

    cout << "\n TODAS LAS PRUEBAS PASARON \n";
    return 0;
}


