#include <iostream>
#include <cassert>
#include "CSVReader.h"
#include "Motor_de_busqueda.h" // Corregido: M mayúscula si tu archivo físico es Motor_de_busqueda.h

using namespace std;
// Prueba 1: Subcadena parcial ("bar" -> "Barco", "Barber", etc.)
void ejecutar_pruebas_motor(const MotorDeBusqueda& motor) {
    cout << " PRUEBAS DEL MOTOR DE BÚSQUEDA \n";

    //  buscar_por_subcadena -> buscarPorSubcadena
    auto res1 = motor.buscarPorSubcadena("bar");
    cout << "[TEST 1] Subcadena 'bar': " << res1.size() << " resultados.\n";

    // Prueba 2: Frase completa ("ghost ship")
    //  buscar_por_subcadena -> buscarPorSubcadena
    auto res2 = motor.buscarPorSubcadena("ghost ship");
    cout << "[TEST 2] Frase 'ghost ship': " << res2.size() << " resultados.\n";

    // Prueba 3: Insensibilidad a mayúsculas/minúsculas
    // buscar_por_subcadena -> buscarPorSubcadena
    auto res3_lower = motor.buscarPorSubcadena("action");
    auto res3_upper = motor.buscarPorSubcadena("ACTION");
    assert(res3_lower.size() == res3_upper.size());
    cout << "[TEST 3] Sensibilidad Case-Insensitive: PASADO\n";

    cout << "**************************************************\n\n";
}
