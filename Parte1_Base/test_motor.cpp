#include <iostream>
#include <cassert>
#include "CSVReader.h"
#include "Motor_de_busqueda.h"

using namespace std;
void ejecutar_pruebas_motor(const MotorDeBusqueda& motor) {
    cout << " PRUEBAS DEL MOTOR DE BÚSQUEDA \n";

    auto res1 = motor.buscarPorSubcadena("bar");
    cout << "[TEST 1] Subcadena 'bar': " << res1.size() << " resultados.\n";

    auto res2 = motor.buscarPorSubcadena("ghost ship");
    cout << "[TEST 2] Frase 'ghost ship': " << res2.size() << " resultados.\n";

    auto res3_lower = motor.buscarPorSubcadena("action");
    auto res3_upper = motor.buscarPorSubcadena("ACTION");
    assert(res3_lower.size() == res3_upper.size());
    cout << "[TEST 3] Sensibilidad Case-Insensitive: PASADO\n";

    cout << "**************************************************\n\n";
}
