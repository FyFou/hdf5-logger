/**
 * @file test_image.c
 * @brief Test des fonctions de log d'images
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "../include/hdf5_logger.h"

int main() {
    printf("Test des logs d'images\n");
    
    // Initialisation
    hdf5_logger_t* logger = hdf5_logger_init("test_image.h5");
    assert(logger != NULL && "L'initialisation du logger a échoué");
    
    // Création d'une petite image test (gradient)
    int width = 100;
    int height = 80;
    int channels = 3;  // RGB
    
    unsigned char* image_data = (unsigned char*)malloc(width * height * channels);
    assert(image_data != NULL && "Allocation d'image a échoué");
    
    // Remplir avec un dégradé simple
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            int idx = (y * width + x) * channels;
            image_data[idx] = (unsigned char)(x * 255 / width);       // R
            image_data[idx + 1] = (unsigned char)(y * 255 / height);  // G
            image_data[idx + 2] = 128;                                // B
        }
    }
    
    // Enregistrer l'image
    int status = hdf5_log_image(logger, "/images", "gradient", 
                              image_data, width, height, channels);
    assert(status == 0 && "Log d'image a échoué");
    
    // Tester une image grayscale
    unsigned char* gray_image = (unsigned char*)malloc(width * height);
    assert(gray_image != NULL && "Allocation d'image grayscale a échoué");
    
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            gray_image[y * width + x] = (unsigned char)((x + y) * 255 / (width + height));
        }
    }
    
    status = hdf5_log_image(logger, "/images", "grayscale", 
                          gray_image, width, height, 1);
    assert(status == 0 && "Log d'image grayscale a échoué");
    
    // Ajouter un attribut à l'image
    const char* desc = "Image de test avec dégradé";
    status = hdf5_add_attribute(logger, "/images/gradient", "description", desc, 1);
    assert(status == 0 && "Ajout d'attribut à l'image a échoué");
    
    // Libérer la mémoire
    free(image_data);
    free(gray_image);
    
    // Fermeture
    status = hdf5_logger_close(logger);
    assert(status == 0 && "La fermeture du logger a échoué");
    
    printf("Tests de logs d'images réussis!\n");
    return 0;
}