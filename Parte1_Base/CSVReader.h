#ifndef PROYECT_PRGR3_AVANCE_CSVREADER_H
#define PROYECT_PRGR3_AVANCE_CSVREADER_H

#include <string>
#include <vector>
#include <fstream>
#include "Pelicula.h"
using namespace std;

class CSVReader {
private:

    vector<string> separarCSV(string linea);
    string leerRegistroCSV(ifstream &archivo);

public:
    vector<Pelicula> leerArchivo(string nombreArchivo);

    // <-- CAMBIO: ahora public static (los usa PeliculaFactory)
    static string limpiarTexto(string texto);
    static string normalizarTexto(string texto);
};

#endif