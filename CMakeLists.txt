cmake_minimum_required(VERSION 3.10)
project(hdf5_logger VERSION 0.1.0 LANGUAGES C)

# Options
option(BUILD_SHARED_LIBS "Build shared libraries" OFF)
option(BUILD_TESTS "Build tests" ON)
option(BUILD_EXAMPLES "Build examples" ON)

# Trouver la bibliothèque HDF5
find_package(HDF5 REQUIRED COMPONENTS C)

# Configuration pour la détection de la plateforme
if(WIN32)
    add_definitions(-DHDF5_LOGGER_WINDOWS)
    # Configuration spécifique à Windows si nécessaire
elseif(UNIX AND NOT APPLE)
    add_definitions(-DHDF5_LOGGER_LINUX)
    # Configuration spécifique à Linux si nécessaire
endif()

# Répertoires d'inclusion
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include ${HDF5_INCLUDE_DIRS})

# Sources de la bibliothèque
set(SOURCES
    src/hdf5_logger.c
    src/hdf5_logger_text.c
    src/hdf5_logger_array.c
    src/hdf5_logger_image.c
    src/hdf5_logger_utils.c
)

# Création de la bibliothèque
if(BUILD_SHARED_LIBS)
    add_library(hdf5_logger SHARED ${SOURCES})
else()
    add_library(hdf5_logger STATIC ${SOURCES})
endif()

# Liens avec HDF5
target_link_libraries(hdf5_logger PRIVATE ${HDF5_LIBRARIES})

# Installation
install(TARGETS hdf5_logger
        LIBRARY DESTINATION lib
        ARCHIVE DESTINATION lib)
install(DIRECTORY include/ DESTINATION include)

# Tests
if(BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()

# Exemples
if(BUILD_EXAMPLES)
    add_subdirectory(examples)
endif()

# Documentation des versions
set_target_properties(hdf5_logger PROPERTIES
    VERSION ${PROJECT_VERSION}
    SOVERSION ${PROJECT_VERSION_MAJOR})

# Affichage du résumé de la configuration
message(STATUS "Configuration de ${PROJECT_NAME} ${PROJECT_VERSION}")
message(STATUS "  HDF5 trouvé: ${HDF5_FOUND}")
message(STATUS "  HDF5 version: ${HDF5_VERSION}")
message(STATUS "  Créer les tests: ${BUILD_TESTS}")
message(STATUS "  Créer les exemples: ${BUILD_EXAMPLES}")
message(STATUS "  Créer des bibliothèques partagées: ${BUILD_SHARED_LIBS}")
