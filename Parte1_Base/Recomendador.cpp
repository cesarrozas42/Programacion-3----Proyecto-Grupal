#include "Recomendador.h"

#include <algorithm>
#include <unordered_map>
#include <unordered_set>

using namespace std;

Recomendador::Recomendador(const vector<Pelicula>& catalogo, const Indexador& indexador)
    : catalogo(catalogo), indexador(indexador) {}

vector<int> Recomendador::recomendar(const Usuario& usuario, int cantidad) const {
    vector<int> likes = usuario.getLikes();

    if (likes.empty()) {
        return {};
    }

    unordered_set<int> yaLikeados(likes.begin(), likes.end());
    unordered_map<int, int> puntaje;

    for (int idLikeado : likes) {
        if (idLikeado < 0 || idLikeado >= static_cast<int>(catalogo.size())) {
            continue;
        }

        for (const Tag& tag : catalogo[idLikeado].getTags()) {
            vector<int> candidatos = indexador.buscarTag(tag.getTipo(), tag.getValor());

            for (int idCandidato : candidatos) {
                if (yaLikeados.find(idCandidato) != yaLikeados.end()) {
                    continue;
                }
                puntaje[idCandidato]++;
            }
        }
    }

    vector<pair<int, int>> ranking(puntaje.begin(), puntaje.end());

    sort(ranking.begin(), ranking.end(),
         [](const pair<int, int>& a, const pair<int, int>& b) {
             if (a.second != b.second) {
                 return a.second > b.second;
             }
             return a.first < b.first;
         });

    vector<int> recomendaciones;
    for (int i = 0; i < static_cast<int>(ranking.size()) && i < cantidad; ++i) {
        recomendaciones.push_back(ranking[i].first);
    }

    return recomendaciones;
}
