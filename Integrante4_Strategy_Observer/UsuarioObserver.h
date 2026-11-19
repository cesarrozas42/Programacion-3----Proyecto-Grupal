#ifndef USUARIO_OBSERVER_H
#define USUARIO_OBSERVER_H

class Usuario;

enum class TipoEventoUsuario { LIKE_AGREGADO, LIKE_ELIMINADO, VER_MAS_TARDE_AGREGADO, VER_MAS_TARDE_ELIMINADO };

class UsuarioObserver {
public:
    virtual ~UsuarioObserver() = default;
    virtual void actualizar(const Usuario& usuario, TipoEventoUsuario evento, int idPelicula) = 0;
};

#endif
