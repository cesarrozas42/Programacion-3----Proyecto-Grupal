# Avance de la semana 8

## Objetivo demostrado

El avance integra en una sola aplicación C++ la carga del CSV, la indexación en Trie, la búsqueda parcial, el ranking de resultados, la vista de sinopsis y el estado básico del usuario.

## Aporte del Integrante 5

- Creación de `InterfazCLI` como módulo separado de la lógica del negocio.
- Pantalla inicial con `Ver más tarde` y recomendaciones.
- Menú de búsquedas por texto, director, género y reparto.
- Presentación de cinco resultados por página, navegación siguiente/anterior y selección local del 1 al 5.
- Vista completa de una película con acciones para agregar o quitar Like y Ver más tarde.
- Guardado inmediato del estado del usuario y recarga en la siguiente ejecución.
- Integración de todos los módulos desde un `main.cpp` pequeño.
- Configuración reproducible con CMake, pruebas y documentación.

## Flujo para la demostración presencial

1. Iniciar la aplicación y señalar `Ver más tarde` y `Recomendadas`.
2. Buscar la subcadena `bar` y mostrar que coincide dentro de palabras.
3. Avanzar a la segunda página para demostrar grupos de cinco resultados.
4. Seleccionar una película y enseñar su sinopsis.
5. Dar Like y agregarla a `Ver más tarde`.
6. Volver a recomendaciones para mostrar el efecto del Like.
7. Cerrar y abrir el programa para demostrar la persistencia.
8. Buscar por director y por actor para evidenciar que los tags no se mezclan.

## Explicación corta de la integración

`main.cpp` funciona como punto de composición: crea el catálogo, el índice y los servicios. Luego inyecta esas dependencias a `InterfazCLI`. La interfaz solo coordina las acciones; no vuelve a implementar los algoritmos de los demás integrantes. Esta separación permite que cada módulo se pruebe o cambie sin reescribir el menú.

## Estado actual y trabajo hacia la semana 16

### Completado

- Ingesta del CSV.
- Trie de contenido y tags.
- Búsqueda por texto y tags.
- Ranking y páginas de cinco.
- Like, Ver más tarde, recomendaciones y persistencia.
- Interfaz CLI integrada.

### Próximas mejoras

- Medir formalmente tiempo de carga, tiempo de consulta y memoria.
- Agregar más pruebas con el dataset completo y casos con tildes.
- Refinar el score con frecuencia o rareza de términos.
- Permitir perfiles de varios usuarios.
- Incorporar una GUI solo si el alcance del equipo lo permite.

## Posibles preguntas del profesor

**¿Por qué la interfaz está separada de `main.cpp`?**  
Porque `main` debe construir y conectar los componentes, mientras que `InterfazCLI` controla únicamente la interacción. Esto reduce acoplamiento y facilita las pruebas.

**¿Dónde se cumple el requisito de cinco resultados?**  
En `InterfazCLI::navegarResultados`, que calcula páginas de tamaño constante cinco y permite avanzar o retroceder.

**¿Cómo permanece Ver más tarde al reiniciar?**  
`Usuario::guardar` escribe los IDs en `estado_usuario.txt`; `Usuario::cargar` los recupera al inicio.

**¿Qué pasa con una entrada incorrecta?**  
Las opciones se leen como texto y solo se convierten si representan un número dentro del rango visible. Una entrada inválida no termina el programa.
