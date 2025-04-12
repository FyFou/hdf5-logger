/**
 * @file simple_logger.c
 * @brief Exemple simple d'utilisation de HDF5 Logger
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "../include/hdf5_logger.h"

int main() {
    printf("Exemple simple d'utilisation de HDF5 Logger\n");
    
    // Initialiser le logger
    hdf5_logger_t* logger = hdf5_logger_init("simple_log.h5");
    if (!logger) {
        fprintf(stderr, "Erreur: Impossible d'initialiser le logger\n");
        return 1;
    }
    
    printf("Version de HDF5 Logger: %s\n", hdf5_logger_version());
    
    // Enregistrer des logs texte
    printf("Enregistrement de logs texte...\n");
    hdf5_log_text(logger, HDF5_LOG_INFO, "Démarrage de l'application");
    hdf5_log_text(logger, HDF5_LOG_DEBUG, "Initialisation des ressources");
    hdf5_log_text(logger, HDF5_LOG_WARNING, "Configuration par défaut utilisée");
    
    // Enregistrer des logs texte dans un groupe personnalisé
    hdf5_log_text_to_group(logger, "/app/module1", HDF5_LOG_INFO, "Module 1 initialisé");
    hdf5_log_text_to_group(logger, "/app/module2", HDF5_LOG_ERROR, "Module 2 en échec");
    
    // Enregistrer des données numériques 1D
    printf("Enregistrement de données numériques 1D...\n");
    float sine_wave[100];
    for (int i = 0; i < 100; i++) {
        sine_wave[i] = sinf(i * 0.1f);
    }
    
    hdf5_log_array_1d(logger, "/data/signals", "sine_wave", sine_wave, 100, 0);
    
    // Enregistrer des données numériques 2D
    printf("Enregistrement de données numériques 2D...\n");
    float matrix[10][20];
    for (int i = 0; i < 10; i++) {
        for (int j = 0; j < 20; j++) {
            matrix[i][j] = (float)(i * j) / 10.0f;
        }
    }
    
    hdf5_log_array_2d(logger, "/data/matrices", "gradient", matrix, 10, 20, 0);
    
    // Enregistrer une image simple (dégradé)
    printf("Enregistrement d'une image...\n");
    int width = 200;
    int height = 100;
    int channels = 3;  // RGB
    
    unsigned char* image_data = (unsigned char*)malloc(width * height * channels);
    if (image_data) {
        // Créer un dégradé simple R-G-B
        for (int y = 0; y < height; y++) {
            for (int x = 0; x < width; x++) {
                int idx = (y * width + x) * channels;
                image_data[idx] = (unsigned char)(x * 255 / width);        // R
                image_data[idx + 1] = (unsigned char)(y * 255 / height);    // G
                image_data[idx + 2] = (unsigned char)((x+y) * 255 / (width+height)); // B
            }
        }
        
        hdf5_log_image(logger, "/images", "gradient", image_data, width, height, channels);
        free(image_data);
    }
    
    // Définir des limites
    printf("Configuration des limites...\n");
    hdf5_logger_set_size_limit(logger, "/app/module1", 100);  // Max 100 entrées
    hdf5_logger_set_time_limit(logger, "/app/module2", 3600.0);  // Max 1 heure
    
    // Ajouter des attributs
    printf("Ajout d'attributs...\n");
    const char* desc = "Signal sinusoïdal de test";
    hdf5_add_attribute(logger, "/data/signals/sine_wave", "description", desc, 1);
    
    double sampling_rate = 10.0;  // Hz
    hdf5_add_attribute(logger, "/data/signals/sine_wave", "sampling_rate", &sampling_rate, 0);
    
    // Fermer le logger
    printf("Fermeture du logger...\n");
    hdf5_logger_close(logger);
    
    printf("Exemple terminé avec succès. Fichier de log créé: simple_log.h5\n");
    return 0;
}