#ifndef PROYECT_PRGR3_AVANCE_USUARIO_H
#define PROYECT_PRGR3_AVANCE_USUARIO_H

#include <string>
#include <vector>

using namespace std;

class Usuario {
private:
    int id;
    string nombre;

    vector<int> likes;
    vector<int> verMasTarde;

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
};

#endif