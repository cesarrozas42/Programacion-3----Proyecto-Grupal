#ifndef PROYECT_PRGR3_AVANCE_USUARIO_H
#define PROYECT_PRGR3_AVANCE_USUARIO_H

#include <string>
#include <vector>

#include "UsuarioObserver.h"

using namespace std;

class Usuario {
private:
    int id;
    string nombre;

    vector<int> likes;
    vector<int> verMasTarde;

    vector<UsuarioObserver*> observadores;
    void notificar(TipoEventoUsuario evento, int idPelicula) const;

public:
    Usuario();
    Usuario(int id, string nombre);

    int getId() const;
    string getNombre() const;

    bool agregarLike(int idPelicula);
    bool agregarVerMasTarde(int idPelicula);
    bool quitarLike(int idPelicula);
    bool quitarVerMasTarde(int idPelicula);

    bool tieneLike(int idPelicula) const;
    bool tieneVerMasTarde(int idPelicula) const;

    vector<int> getLikes() const;
    vector<int> getVerMasTarde() const;

    void guardar(const string& archivo) const;
    void cargar(const string& archivo);

    void agregarObservador(UsuarioObserver* observador);
    void quitarObservador(UsuarioObserver* observador);
};

#endif
