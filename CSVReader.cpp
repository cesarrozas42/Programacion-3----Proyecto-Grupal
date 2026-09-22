#include "CSVReader.h"

#include <fstream>
#include <iostream>
#include <cctype>

using namespace std;


vector<string> CSVReader::separarCSV(string linea) {

    vector<string> campos;
    string campo;
    bool dentroComillas = false;

    for (int i = 0; i < linea.size(); i++) {

        char c = linea[i];

        if (c == '"') {

            if (dentroComillas &&
                i + 1 < linea.size() &&
                linea[i + 1] == '"') {

                campo += '"';
                i++;

            } else {
                dentroComillas = !dentroComillas;
            }

        } else if (c == ',' && !dentroComillas) {

            campos.push_back(campo);
            campo.clear();

        } else {

            campo += c;
        }
    }

    campos.push_back(campo);

    return campos;
}


string CSVReader::limpiarTexto(string texto) {

    int inicio = 0;

    while (inicio < texto.size() &&
           isspace(static_cast<unsigned char>(texto[inicio]))) {
        inicio++;
    }

    int fin = texto.size() - 1;

    while (fin >= inicio &&
           isspace(static_cast<unsigned char>(texto[fin]))) {
        fin--;
    }

    if (inicio > fin) {
        return "";
    }

    return texto.substr(inicio, fin - inicio + 1);
}

string reemplazarTodos(string texto, string buscar, string reemplazo) {
    size_t posicion = 0;

    while ((posicion = texto.find(buscar, posicion)) != string::npos) {
        texto.replace(posicion, buscar.size(), reemplazo);
        posicion += reemplazo.size();
    }

    return texto;
}

string CSVReader::normalizarTexto(string texto) {
    texto = limpiarTexto(texto);

    // Vocales con tilde
    texto = reemplazarTodos(texto, "á", "a");
    texto = reemplazarTodos(texto, "é", "e");
    texto = reemplazarTodos(texto, "í", "i");
    texto = reemplazarTodos(texto, "ó", "o");
    texto = reemplazarTodos(texto, "ú", "u");

    texto = reemplazarTodos(texto, "Á", "A");
    texto = reemplazarTodos(texto, "É", "E");
    texto = reemplazarTodos(texto, "Í", "I");
    texto = reemplazarTodos(texto, "Ó", "O");
    texto = reemplazarTodos(texto, "Ú", "U");

    // Ñ
    texto = reemplazarTodos(texto, "ñ", "n");
    texto = reemplazarTodos(texto, "Ñ", "N");

    // Caracteres frecuentes en nombres extranjeros
    texto = reemplazarTodos(texto, "ç", "c");
    texto = reemplazarTodos(texto, "Ç", "C");

    texto = reemplazarTodos(texto, "ö", "o");
    texto = reemplazarTodos(texto, "Ö", "O");

    texto = reemplazarTodos(texto, "ü", "u");
    texto = reemplazarTodos(texto, "Ü", "U");

    texto = reemplazarTodos(texto, "ş", "s");
    texto = reemplazarTodos(texto, "Ş", "S");

    texto = reemplazarTodos(texto, "ğ", "g");
    texto = reemplazarTodos(texto, "Ğ", "G");

    texto = reemplazarTodos(texto, "ı", "i");
    texto = reemplazarTodos(texto, "İ", "I");

    // Pasar a minúsculas y eliminar caracteres especiales
    string resultado;

    for (char c : texto) {
        c = static_cast<char>(
                tolower(static_cast<unsigned char>(c))
        );

        if (isalnum(static_cast<unsigned char>(c))) {
            resultado += c;
        } else {
            resultado += ' ';
        }
    }

    return resultado;
}

string CSVReader::leerRegistroCSV(ifstream &archivo) {
    string registro;
    string linea;
    bool dentroComillas = false;
    while (getline(archivo, linea)){
        registro += linea;
        for (int i = 0; i < linea.size(); i++){
            if (linea[i] == '"'){
                // Si encontramos dos comillas seguidas,
                // representan una comilla dentro del texto.
                if (i + 1 < linea.size() && linea[i + 1] == '"') {
                    i++;
                }
                else {
                    dentroComillas = !dentroComillas;
                }
            }
        }
        // Si ya no estamos dentro de comillas,
        // significa que terminó el registro.
        if (!dentroComillas) {
            return registro;
        }
        // El registro continúa en la siguiente línea
        registro  += "\n";
    }
    return registro;
}

vector<Pelicula> CSVReader::leerArchivo(string nombreArchivo) {

    vector<Pelicula> peliculas;

    int descartadas = 0;

    ifstream archivo(nombreArchivo);

    if (!archivo.is_open()) {
        cout << "No se pudo abrir el archivo." << endl;
        return peliculas;
    }

    string linea;

    // Saltar encabezado
    getline(archivo, linea);

    while (true) {

        string registro = leerRegistroCSV(archivo);

        if (registro.empty()) {
            break;
        }

        vector<string> campos = separarCSV(registro);

        if (campos.size() != 8) {
            descartadas++;
            continue;
        }

        string anioTexto = limpiarTexto(campos[0]);

        if (anioTexto.empty()) {
            descartadas++;
            continue;
        }

        int anio;

        try {
            anio = stoi(anioTexto);
        }
        catch (...) {
            descartadas++;
            continue;
        }

        string titulo = limpiarTexto(campos[1]);
        string origen = limpiarTexto(campos[2]);
        string director = limpiarTexto(campos[3]);
        string casting = limpiarTexto(campos[4]);
        string genero = limpiarTexto(campos[5]);
        string wikiPage = limpiarTexto(campos[6]);
        string sinopsis = limpiarTexto(campos[7]);

        Pelicula pelicula(
                anio,
                titulo,
                origen,
                director,
                casting,
                genero,
                wikiPage,
                sinopsis
        );

        pelicula.setTituloNormalizado(
                normalizarTexto(titulo)
        );

        pelicula.setSinopsisNormalizada(
                normalizarTexto(sinopsis)
        );

        peliculas.push_back(pelicula);
    }
    cout << "Peliculas cargadas: " << peliculas.size() << endl;
    cout << "Filas descartadas: " << descartadas << endl;

    archivo.close();

    return peliculas;
}