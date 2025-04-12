/**
 * @file test_init.c
 * @brief Test d'initialisation et de fermeture du logger
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "../include/hdf5_logger.h"

int main() {
    printf("Test d'initialisation du logger\n");
    
    // Test d'initialisation
    hdf5_logger_t* logger = hdf5_logger_init("test_init.h5");
    assert(logger != NULL && "L'initialisation du logger a échoué");
    
    // Test de fermeture
    int status = hdf5_logger_close(logger);
    assert(status == 0 && "La fermeture du logger a échoué");
    
    // Test d'initialisation avec un chemin invalide
    hdf5_logger_t* invalid_logger = hdf5_logger_init("");
    assert(invalid_logger == NULL && "L'initialisation avec un chemin invalide devrait échouer");
    
    // Vérification de la version
    const char* version = hdf5_logger_version();
    assert(version != NULL && strlen(version) > 0 && "La version devrait être non nulle");
    printf("Version du logger: %s\n", version);
    
    printf("Tests d'initialisation réussis!\n");
    return 0;
}