/**
 * @file test_array.c
 * @brief Test des fonctions de log de tableaux
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "../include/hdf5_logger.h"

int main() {
    printf("Test des logs de tableaux\n");
    
    // Initialisation
    hdf5_logger_t* logger = hdf5_logger_init("test_array.h5");
    assert(logger != NULL && "L'initialisation du logger a échoué");
    
    // Test de tableau 1D (float)
    float data_1d[10];
    for (int i = 0; i < 10; i++) {
        data_1d[i] = (float)i * 1.5f;
    }
    
    int status = hdf5_log_array_1d(logger, "/arrays/float", "data_1d", 
                                 data_1d, 10, 0);
    assert(status == 0 && "Log de tableau 1D (float) a échoué");
    
    // Test de tableau 1D (double)
    double data_1d_double[10];
    for (int i = 0; i < 10; i++) {
        data_1d_double[i] = (double)i * 1.5;
    }
    
    status = hdf5_log_array_1d(logger, "/arrays/double", "data_1d", 
                             data_1d_double, 10, 1);
    assert(status == 0 && "Log de tableau 1D (double) a échoué");
    
    // Test de tableau 2D (float)
    float data_2d[5][4];
    for (int i = 0; i < 5; i++) {
        for (int j = 0; j < 4; j++) {
            data_2d[i][j] = (float)(i * 10 + j);
        }
    }
    
    status = hdf5_log_array_2d(logger, "/arrays/float", "data_2d", 
                             data_2d, 5, 4, 0);
    assert(status == 0 && "Log de tableau 2D (float) a échoué");
    
    // Test de tableau 3D (float)
    float data_3d[3][4][2];
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 4; j++) {
            for (int k = 0; k < 2; k++) {
                data_3d[i][j][k] = (float)(i * 100 + j * 10 + k);
            }
        }
    }
    
    status = hdf5_log_array_3d(logger, "/arrays/float", "data_3d", 
                             data_3d, 3, 4, 2, 0);
    assert(status == 0 && "Log de tableau 3D (float) a échoué");
    
    // Ajouter un attribut à un tableau
    double attr_value = 123.456;
    status = hdf5_add_attribute(logger, "/arrays/float/data_2d", "sampling_rate", 
                              &attr_value, 0);
    assert(status == 0 && "Ajout d'attribut numérique a échoué");
    
    const char* str_attr = "Données de capteur";
    status = hdf5_add_attribute(logger, "/arrays/float/data_2d", "description", 
                              str_attr, 1);
    assert(status == 0 && "Ajout d'attribut texte a échoué");
    
    // Fermeture
    status = hdf5_logger_close(logger);
    assert(status == 0 && "La fermeture du logger a échoué");
    
    printf("Tests de logs de tableaux réussis!\n");
    return 0;
}