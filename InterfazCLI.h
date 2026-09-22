#ifndef INTERFAZ_CLI_H
#define INTERFAZ_CLI_H

#include <string>
#include <vector>
#include "Motor_de_busqueda.h"
#include "Pelicula.h"
#include "Recomendador.h"
#include "Relevancia.h"
#include "Usuario.h"

class InterfazCLI {

private:
    const std::vector<Pelicula>& catalogo;
    const Relevancia& relevancia;
    const Recomendador& recomendador;
    const MotorDeBusqueda& motor;
    Usuario& usuario;
    std::string archivoEstado;

    static std::string leerLinea(const std::string& mensaje);
    static int leerEntero(const std::string& texto, int minimo, int maximo);
    static void pausar();
    bool idValido(int idPelicula) const;
    void guardarEstado() const;
    void mostrarEncabezado() const;
    void mostrarResumen(int idPelicula, int numero = 0) const;
    void mostrarColeccion(const std::string& titulo,
                          const std::vector<int>& ids,
                          std::size_t limite = 5) const;
    void mostrarInicio() const;
    void verDetalle(int idPelicula);
    void navegarResultados(const std::string& titulo,
                           const std::vector<int>& ids);
    void buscarPorTexto();
    void buscarPorTag(const std::string& tipo);
    void mostrarRecomendadas();
    void mostrarVerMasTarde();

public:
    InterfazCLI();

    // 2. Declarar el método iniciar
    void iniciar();
    InterfazCLI(const std::vector<Pelicula>& catalogo,
                const Relevancia& relevancia,
                const Recomendador& recomendador,
                const MotorDeBusqueda& motor,
                Usuario& usuario,
                std::string archivoEstado);
    void ejecutar();
};

#endif
