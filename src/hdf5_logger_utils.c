/**
 * @file hdf5_logger_utils.c
 * @brief Fonctions utilitaires pour HDF5 Logger
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "hdf5.h"
#include "../include/hdf5_logger.h"

/* Implémentation de la fonction interne create_group_if_not_exists */
hid_t create_group_if_not_exists(hid_t file_id, const char* group_path) {
    if (file_id < 0 || group_path == NULL) {
        return -1;
    }
    
    hid_t group_id;
    
    /* Vérifier si le chemin est vide ou racine */
    if (group_path[0] == '\0') {
        return -1;
    }
    
    if (strcmp(group_path, "/") == 0) {
        return file_id; /* Le groupe racine est déjà ouvert */
    }
    
    /* Vérifier si le groupe existe déjà */
    htri_t exists = H5Lexists(file_id, group_path, H5P_DEFAULT);
    if (exists > 0) {
        /* Le groupe existe, l'ouvrir simplement */
        group_id = H5Gopen2(file_id, group_path, H5P_DEFAULT);
    } else {
        /* Le groupe n'existe pas, le créer avec les groupes intermédiaires */
        hid_t lcpl_id = H5Pcreate(H5P_LINK_CREATE);
        H5Pset_create_intermediate_group(lcpl_id, 1);
        
        group_id = H5Gcreate2(file_id, group_path, lcpl_id, H5P_DEFAULT, H5P_DEFAULT);
        
        H5Pclose(lcpl_id);
    }
    
    return group_id;
}

/* Fonctions utilitaires supplémentaires pourraient être ajoutées ici */
