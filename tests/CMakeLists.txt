# Configuration des tests

# Ajout de la bibliothèque pour les tests
find_package(HDF5 REQUIRED COMPONENTS C)

# Création des exécutables de test
add_executable(test_init test_init.c)
add_executable(test_text test_text.c)
add_executable(test_array test_array.c)
add_executable(test_image test_image.c)
add_executable(test_limits test_limits.c)

# Lier avec la bibliothèque hdf5_logger
target_link_libraries(test_init hdf5_logger ${HDF5_LIBRARIES})
target_link_libraries(test_text hdf5_logger ${HDF5_LIBRARIES})
target_link_libraries(test_array hdf5_logger ${HDF5_LIBRARIES})
target_link_libraries(test_image hdf5_logger ${HDF5_LIBRARIES})
target_link_libraries(test_limits hdf5_logger ${HDF5_LIBRARIES})

# Ajouter les tests à CTest
add_test(NAME TestInit COMMAND test_init)
add_test(NAME TestText COMMAND test_text)
add_test(NAME TestArray COMMAND test_array)
add_test(NAME TestImage COMMAND test_image)
add_test(NAME TestLimits COMMAND test_limits)
