cmake_minimum_required(VERSION 3.20)
project(proyecto_buscador_peliculas)

set(CMAKE_CXX_STANDARD 20)

find_package(Threads REQUIRED)   # NUEVO (std::mutex del Singleton)

# Ejecutable principal de la aplicacion (sin los tests)
add_executable(proyecto_buscador_peliculas
        main.cpp
        CSVReader.cpp
        Indexador.cpp
        Motor_de_busqueda.cpp
        Pelicula.cpp
        Recomendador.cpp
        Relevancia.cpp
        Tag.cpp
        Usuario.cpp
        Trie.cpp
        InterfazCLI.cpp
        PeliculaFactory.cpp       # NUEVO
        CatalogoPeliculas.cpp)    # NUEVO
target_link_libraries(proyecto_buscador_peliculas Threads::Threads)   # NUEVO

# Ejecutable separado solo para los tests
add_executable(tests_proyecto
        test_relevancia_recomendador.cpp
        test_motor.cpp
        CSVReader.cpp
        Indexador.cpp
        Motor_de_busqueda.cpp
        Pelicula.cpp
        Recomendador.cpp
        Relevancia.cpp
        Tag.cpp
        Usuario.cpp
        Trie.cpp
        PeliculaFactory.cpp)      # NUEVO (CSVReader.cpp ahora lo necesita)

# Arbol (Integrante 1 - Parte 2): pruebas unitarias y demo para la exposicion
add_executable(tests_arbol
        test_arbol.cpp
        Indexador.cpp
        Pelicula.cpp
        Tag.cpp
        Trie.cpp)

add_executable(demo_arbol
        demo_arbol.cpp
        CSVReader.cpp
        Indexador.cpp
        Pelicula.cpp
        Tag.cpp
        Trie.cpp
        PeliculaFactory.cpp)      # NUEVO (CSVReader.cpp ahora lo necesita)

# Patrones A (Integrante 3 - Parte 2): Singleton + Factory
add_executable(test_patrones      # NUEVO
        test_patrones.cpp
        CSVReader.cpp
        Indexador.cpp
        Pelicula.cpp
        Tag.cpp
        Trie.cpp
        PeliculaFactory.cpp
        CatalogoPeliculas.cpp)
target_link_libraries(test_patrones Threads::Threads)
