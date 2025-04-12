# Configuration des exemples

# Trouver la bibliothèque HDF5
find_package(HDF5 REQUIRED COMPONENTS C)

# Inclure les répertoires nécessaires
include_directories(${CMAKE_SOURCE_DIR}/include ${HDF5_INCLUDE_DIRS})

# Simple Logger
add_executable(simple_logger simple_logger.c)
target_link_libraries(simple_logger hdf5_logger ${HDF5_LIBRARIES})

# Installer les exemples
install(TARGETS simple_logger
        RUNTIME DESTINATION bin/examples)

# Copier les fichiers source des exemples
install(FILES simple_logger.c
        DESTINATION share/hdf5_logger/examples)
