# Verificación contra el GitHub del grupo

Repositorio: https://github.com/cesarrozas42/Programacion-3----Proyecto-Grupal (rama `main`, revisado el 17/11/2026).
Se comparó cada archivo `.h`, `.cpp` y `CMakeLists.txt` del GitHub con el proyecto integrado (sin contar diferencias de fin de línea).

## Iguales al GitHub (20 archivos)
| Archivo | Responsable |
|---|---|
| `Trie.h`, `Trie.cpp`, `Indexador.h`, `Indexador.cpp`, `test_arbol.cpp`, `demo_arbol.cpp` | Integrante 1 – Árbol |
| `EstrategiaBusqueda.h`, `EstrategiaBusqueda.cpp`, `UsuarioObserver.h` | Integrante 4 – Strategy y Observer |
| `CSVReader.h`, `Pelicula.*`, `Tag.*`, `Motor_de_busqueda.*`, `Relevancia.*`, `Recomendador.*`, `test_motor.cpp` | Parte 1 |

## Distintos al GitHub, y por qué (8 archivos)
| Archivo | Motivo |
|---|---|
| `InterfazCLI.h`, `InterfazCLI.cpp` | **Integrante 5.** Es el menú de la parte 1, mejorado. Agrega autocompletado con el árbol, tiempos de cada búsqueda, la lista de Likes, el selector de modo de búsqueda (Strategy) y hace de observador del Usuario (Observer). |
| `main.cpp` | **Integrante 5.** El del GitHub es el de la primera versión, con el menú dentro de `main` y una llamada a `InterfazCLI::iniciar()` que no existe. Este es el de la versión final de la parte 1, y además mide los tiempos de carga y activa los colores. |
| `CMakeLists.txt` | **Integrante 5 (CMake final).** Usa la librería `streaming_core`, agrega `EstrategiaBusqueda.cpp` y los hilos, y registra las pruebas con `ctest`. |
| `Usuario.h`, `Usuario.cpp` | **Integración del Observer.** El GitHub trae la interfaz `UsuarioObserver`, pero el Usuario todavía no avisaba a nadie. Se agregaron `agregarObservador`, `quitarObservador` y `notificar`, y se llama a `notificar` cuando cambia un Like o Ver más tarde. |
| `CSVReader.cpp` | Versión final de la parte 1: usa `size_t` en lugar de `int` para recorrer los textos, lo que corrige advertencias del compilador. El resultado es el mismo: 34,886 películas y 0 filas descartadas. |
| `test_relevancia_recomendador.cpp` | Versión final de la parte 1: tiene una prueba extra que verifica que buscar por director no mezcla resultados de actores. |

## Resultado
- Compila sin errores ni advertencias con el compilador de CLion (MinGW).
- `tests_proyecto`: todas las pruebas pasan.
- `tests_arbol`: 15 de 15 pasan.
- El menú se probó completo con el dataset real: búsquedas en los modos Árbol, Secuencial y Paralela, autocompletado, Like y Ver más tarde, que avisan por Observer y se guardan.
