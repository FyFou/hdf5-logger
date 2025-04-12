/**
 * @file test_limits.c
 * @brief Test des limites de temps et de taille pour les logs
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#ifdef _WIN32
#include <windows.h>
#define sleep(x) Sleep((x) * 1000)
#else
#include <unistd.h>  /* Pour sleep() */
#endif
#include "../include/hdf5_logger.h"

int main() {
    printf("Test des limites de logs\n");
    
    // Initialisation
    hdf5_logger_t* logger = hdf5_logger_init("test_limits.h5");
    assert(logger != NULL && "L'initialisation du logger a échoué");
    
    // Définir une limite de taille pour les logs texte
    int status = hdf5_logger_set_size_limit(logger, "/text_logs/limited_size", 5);
    assert(status == 0 && "Définition de limite de taille a échoué");
    
    // Tester la limite de taille en ajoutant plus de logs que la limite
    for (int i = 0; i < 10; i++) {
        char message[64];
        sprintf(message, "Message numéro %d", i);
        
        status = hdf5_log_text_to_group(logger, "/text_logs/limited_size", 
                                     HDF5_LOG_INFO, message);
        assert(status == 0 && "Log texte pour test de limite a échoué");
    }
    
    // Définir une limite de temps très courte (2 secondes)
    status = hdf5_logger_set_time_limit(logger, "/text_logs/limited_time", 2.0);
    assert(status == 0 && "Définition de limite de temps a échoué");
    
    // Ajouter un premier log
    status = hdf5_log_text_to_group(logger, "/text_logs/limited_time", 
                                 HDF5_LOG_INFO, "Premier message");
    assert(status == 0 && "Premier log pour test de temps a échoué");
    
    // Attendre un moment
    printf("Attente de 3 secondes...\n");
    sleep(3);  // Attendre 3 secondes
    
    // Ajouter un second log (devrait supprimer le premier car > 2 secondes)
    status = hdf5_log_text_to_group(logger, "/text_logs/limited_time", 
                                 HDF5_LOG_INFO, "Second message après délai");
    assert(status == 0 && "Second log pour test de temps a échoué");
    
    // Fermeture
    status = hdf5_logger_close(logger);
    assert(status == 0 && "La fermeture du logger a échoué");
    
    printf("Tests de limites de logs réussis!\n");
    return 0;
}