/**
 * @file test_text.c
 * @brief Test des fonctions de log texte
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "../include/hdf5_logger.h"

int main() {
    printf("Test des logs texte\n");
    
    // Initialisation
    hdf5_logger_t* logger = hdf5_logger_init("test_text.h5");
    assert(logger != NULL && "L'initialisation du logger a échoué");
    
    // Test des logs texte avec différents niveaux
    int status;
    
    status = hdf5_log_text(logger, HDF5_LOG_DEBUG, "Message de debug");
    assert(status == 0 && "Log de debug a échoué");
    
    status = hdf5_log_text(logger, HDF5_LOG_INFO, "Message d'information");
    assert(status == 0 && "Log d'info a échoué");
    
    status = hdf5_log_text(logger, HDF5_LOG_WARNING, "Attention!");
    assert(status == 0 && "Log d'avertissement a échoué");
    
    status = hdf5_log_text(logger, HDF5_LOG_ERROR, "Une erreur s'est produite");
    assert(status == 0 && "Log d'erreur a échoué");
    
    status = hdf5_log_text(logger, HDF5_LOG_CRITICAL, "Erreur critique!");
    assert(status == 0 && "Log critique a échoué");
    
    // Test de log texte dans un groupe spécifique
    status = hdf5_log_text_to_group(logger, "/custom_logs/application", 
                                 HDF5_LOG_INFO, "Message dans un groupe personnalisé");
    assert(status == 0 && "Log dans un groupe personnalisé a échoué");
    
    // Test avec un message vide
    status = hdf5_log_text(logger, HDF5_LOG_INFO, "");
    assert(status == 0 && "Log avec message vide a échoué");
    
    // Test avec un message long
    char long_message[2048];
    memset(long_message, 'A', sizeof(long_message) - 1);
    long_message[sizeof(long_message) - 1] = '\0';
    
    status = hdf5_log_text(logger, HDF5_LOG_INFO, long_message);
    assert(status == 0 && "Log avec message long a échoué");
    
    // Fermeture
    status = hdf5_logger_close(logger);
    assert(status == 0 && "La fermeture du logger a échoué");
    
    printf("Tests de logs texte réussis!\n");
    return 0;
}