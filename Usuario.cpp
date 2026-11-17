#include "Usuario.h"

#include <algorithm>

Usuario::Usuario() {
    id = 0;
    nombre = "";
}

Usuario::Usuario(int id, string nombre) {
    this->id = id;
    this->nombre = nombre;
}

int Usuario::getId() const {
    return id;
}

string Usuario::getNombre() const {
    return nombre;
}

bool Usuario::tieneLike(int idPelicula) const {
    return find(likes.begin(), likes.end(), idPelicula) != likes.end();
}

bool Usuario::tieneVerMasTarde(int idPelicula) const {
    return find(verMasTarde.begin(), verMasTarde.end(), idPelicula) != verMasTarde.end();
}

bool Usuario::agregarLike(int idPelicula) {
    if (tieneLike(idPelicula)) {
        return false;
    }
    likes.push_back(idPelicula);
    notificar(TipoEventoUsuario::LIKE_AGREGADO, idPelicula);
    return true;
}

bool Usuario::agregarVerMasTarde(int idPelicula) {
    if (tieneVerMasTarde(idPelicula)) {
        return false;
    }
    verMasTarde.push_back(idPelicula);
    notificar(TipoEventoUsuario::VER_MAS_TARDE_AGREGADO, idPelicula);
    return true;
}

bool Usuario::quitarLike(int idPelicula) {
    auto it = find(likes.begin(), likes.end(), idPelicula);
    if (it == likes.end()) {
        return false;
    }
    likes.erase(it);
    notificar(TipoEventoUsuario::LIKE_ELIMINADO, idPelicula);
    return true;
}

bool Usuario::quitarVerMasTarde(int idPelicula) {
    auto it = find(verMasTarde.begin(), verMasTarde.end(), idPelicula);
    if (it == verMasTarde.end()) {
        return false;
    }
    verMasTarde.erase(it);
    notificar(TipoEventoUsuario::VER_MAS_TARDE_ELIMINADO, idPelicula);
    return true;
}

// --- Observer ---
void Usuario::agregarObservador(UsuarioObserver* observador) {
    if (observador != nullptr &&
        find(observadores.begin(), observadores.end(), observador) == observadores.end()) {
        observadores.push_back(observador);
    }
}

void Usuario::quitarObservador(UsuarioObserver* observador) {
    observadores.erase(remove(observadores.begin(), observadores.end(), observador),
                       observadores.end());
}

void Usuario::notificar(TipoEventoUsuario evento, int idPelicula) const {
    const vector<UsuarioObserver*> copia = observadores;  // por si alguien se desuscribe al recibir el aviso
    for (UsuarioObserver* observador : copia) {
        observador->actualizar(*this, evento, idPelicula);
    }
}
// Usuario.cpp - agregar
#include <fstream>

void Usuario::guardar(const string& archivo) const {
    ofstream out(archivo);
    if (!out.is_open()) return;
    for (int id : likes) out << "LIKE " << id << "\n";
    for (int id : verMasTarde) out << "VER " << id << "\n";
}

void Usuario::cargar(const string& archivo) {
    ifstream in(archivo);
    if (!in.is_open()) return;
    string tipo;
    int id;
    while (in >> tipo >> id) {
        if (tipo == "LIKE") agregarLike(id);
        else if (tipo == "VER") agregarVerMasTarde(id);
    }
}

vector<int> Usuario::getLikes() const {
    return likes;
}

vector<int> Usuario::getVerMasTarde() const {
    return verMasTarde;
}
