#ifndef INTERFAZ_CLI_H
#define INTERFAZ_CLI_H

#include <cstddef>
#include <functional>
#include <string>
#include <vector>

#include "EstrategiaBusqueda.h"
#include "Indexador.h"
#include "Pelicula.h"
#include "Recomendador.h"
#include "Relevancia.h"
#include "Usuario.h"
#include "UsuarioObserver.h"

// =====================================================================
//  INTERFAZ + INTEGRACION (Integrante 5)
//
//  Es el mismo menu de la parte 1 (mismas opciones y el mismo flujo),
//  mejorado con lo que agrega la parte 2:
//   - Autocompletado al buscar, usando el arbol (Indexador::autocompletar).
//   - Tiempo que tarda cada busqueda y tiempos de carga del catalogo.
//   - Lista "Mis Likes" y marcas [L] / [V] en los resultados.
//   - Paginas con anterior/siguiente y sinopsis con formato legible.
//   - STRATEGY (integrante 4): elegir como buscar: Arbol, Secuencial o Paralela.
//   - OBSERVER (integrante 4): el menu es observador del Usuario; cuando
//     cambia un Like o Ver mas tarde, recibe el aviso y guarda el estado.
//  La interfaz no repite logica: llama a los modulos y muestra el resultado.
// =====================================================================

// Tiempos de carga que mide main.cpp, para mostrarlos en el menu.
struct TiemposCarga {
    double msLecturaCSV = 0.0;
    double msIndice = 0.0;
};

class InterfazCLI : public UsuarioObserver {
private:
    struct RegistroBusqueda {
        std::string tipo;
        std::string consulta;
        std::size_t resultados;
        double ms;
    };

    const std::vector<Pelicula>& catalogo;
    const Indexador& indexador;
    const Relevancia& relevancia;
    const Recomendador& recomendador;
    Usuario& usuario;
    std::string archivoEstado;
    TiemposCarga tiemposCarga;
    std::vector<RegistroBusqueda> historial;
    ModoBusqueda modo = ModoBusqueda::ARBOL;  // Strategy elegida en el menu
    bool entradaTerminada = false;

    // --- Entrada ---
    std::string leerLinea(const std::string& mensaje);
    static int leerEntero(const std::string& texto, int minimo, int maximo);
    void pausar();

    // --- Utilidades (parte 1) ---
    bool idValido(int idPelicula) const;
    void guardarEstado() const;
    void mostrarEncabezado() const;
    void mostrarResumen(int idPelicula, int numero = 0, const std::string& extra = "") const;
    void mostrarColeccion(const std::string& titulo, const std::vector<int>& ids,
                          std::size_t limite = 5) const;
    void mostrarInicio() const;
    void mostrarAviso(const std::string& texto, bool ok = true) const;
    std::string marcas(int idPelicula) const;

    // --- Pantallas (parte 1, mejoradas) ---
    void verDetalle(int idPelicula);
    // "recargar": para las listas del usuario, que pueden cambiar en el detalle.
    void navegarResultados(const std::string& titulo, std::vector<int> ids,
                           const std::vector<double>& puntajes = {}, double ms = -1.0,
                           const std::function<std::vector<int>()>& recargar = nullptr);
    void buscarPorTexto();
    void buscarPorTag(const std::string& tipo);
    void mostrarRecomendadas();
    void mostrarVerMasTarde();

    // --- Nuevo en la parte 2 ---
    std::string pedirConAutocompletado();
    void mostrarLikes();
    void mostrarTiempos() const;
    void registrar(const std::string& tipo, const std::string& consulta,
                   std::size_t resultados, double ms);
    void cambiarModo();
    std::string nombreModo() const;

public:
    InterfazCLI(const std::vector<Pelicula>& catalogo,
                const Indexador& indexador,
                const Relevancia& relevancia,
                const Recomendador& recomendador,
                Usuario& usuario,
                std::string archivoEstado,
                TiemposCarga tiemposCarga = {});
    ~InterfazCLI() override;
    InterfazCLI(const InterfazCLI&) = delete;
    InterfazCLI& operator=(const InterfazCLI&) = delete;

    void ejecutar();

    // OBSERVER: el Usuario llama a este metodo cuando cambia un Like o Ver mas tarde.
    void actualizar(const Usuario& usuario, TipoEventoUsuario evento, int idPelicula) override;
};

#endif
