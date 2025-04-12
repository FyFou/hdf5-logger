/**
 * @file hdf5_logger.c
 * @brief Implémentation des fonctions principales de HDF5 Logger
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "hdf5.h"
#include "../include/hdf5_logger.h"

/* Version de la bibliothèque */
#define HDF5_LOGGER_VERSION "0.1.0"

/* Définition de la structure interne du logger */
struct hdf5_logger_s {
    hid_t file_id;            /* ID du fichier HDF5 */
    char* filename;           /* Nom du fichier */
    int is_open;              /* Indicateur si le fichier est ouvert */
};

/**
 * @brief Crée un groupe HDF5 s'il n'existe pas déjà
 * @param file_id ID du fichier HDF5
 * @param group_path Chemin du groupe à créer
 * @return ID du groupe ou négatif en cas d'erreur
 */
hid_t create_group_if_not_exists(hid_t file_id, const char* group_path);

hdf5_logger_t* hdf5_logger_init(const char* filename) {
    if (filename == NULL || filename[0] == '\0') {
        return NULL;
    }
    
    hdf5_logger_t* logger = (hdf5_logger_t*)malloc(sizeof(hdf5_logger_t));
    if (logger == NULL) {
        return NULL;
    }
    
    /* Allouer la mémoire pour le nom du fichier */
    logger->filename = strdup(filename);
    if (logger->filename == NULL) {
        free(logger);
        return NULL;
    }
    
    /* Créer/ouvrir le fichier HDF5 */
    hid_t file_id;
    if (H5Fis_hdf5(filename) > 0) {
        /* Le fichier existe et est au format HDF5, l'ouvrir */
        file_id = H5Fopen(filename, H5F_ACC_RDWR, H5P_DEFAULT);
    } else {
        /* Créer un nouveau fichier */
        file_id = H5Fcreate(filename, H5F_ACC_TRUNC, H5P_DEFAULT, H5P_DEFAULT);
    }
    
    if (file_id < 0) {
        free(logger->filename);
        free(logger);
        return NULL;
    }
    
    logger->file_id = file_id;
    logger->is_open = 1;
    
    /* Créer les groupes de base s'ils n'existent pas */
    hid_t group_id;
    
    group_id = create_group_if_not_exists(file_id, "/text_logs");
    if (group_id >= 0) H5Gclose(group_id);
    
    group_id = create_group_if_not_exists(file_id, "/numeric_data");
    if (group_id >= 0) H5Gclose(group_id);
    
    group_id = create_group_if_not_exists(file_id, "/images");
    if (group_id >= 0) H5Gclose(group_id);
    
    return logger;
}

int hdf5_logger_close(hdf5_logger_t* logger) {
    if (logger == NULL) {
        return -1;
    }
    
    int status = 0;
    
    if (logger->is_open) {
        status = H5Fclose(logger->file_id);
        logger->is_open = 0;
    }
    
    free(logger->filename);
    free(logger);
    
    return status;
}

const char* hdf5_logger_version() {
    return HDF5_LOGGER_VERSION;
}

int hdf5_logger_set_time_limit(hdf5_logger_t* logger, const char* group_path, double max_time_seconds) {
    if (logger == NULL || !logger->is_open || group_path == NULL) {
        return -1;
    }
    
    /* Créer le groupe s'il n'existe pas */
    hid_t group_id = create_group_if_not_exists(logger->file_id, group_path);
    if (group_id < 0) {
        return -1;
    }
    
    /* Ajouter ou mettre à jour l'attribut de limite de temps */
    hid_t attr_id;
    herr_t status;
    
    /* Vérifier si l'attribut existe déjà */
    htri_t attr_exists = H5Aexists(group_id, "max_time_seconds");
    
    if (attr_exists > 0) {
        /* L'attribut existe, l'ouvrir */
        attr_id = H5Aopen(group_id, "max_time_seconds", H5P_DEFAULT);
    } else {
        /* Créer un nouvel attribut */
        hid_t dataspace_id = H5Screate(H5S_SCALAR);
        attr_id = H5Acreate2(group_id, "max_time_seconds", H5T_NATIVE_DOUBLE, 
                           dataspace_id, H5P_DEFAULT, H5P_DEFAULT);
        H5Sclose(dataspace_id);
    }
    
    if (attr_id < 0) {
        H5Gclose(group_id);
        return -1;
    }
    
    /* Écrire la valeur de l'attribut */
    status = H5Awrite(attr_id, H5T_NATIVE_DOUBLE, &max_time_seconds);
    
    H5Aclose(attr_id);
    H5Gclose(group_id);
    
    return (status < 0) ? -1 : 0;
}

int hdf5_logger_set_size_limit(hdf5_logger_t* logger, const char* group_path, size_t max_entries) {
    if (logger == NULL || !logger->is_open || group_path == NULL) {
        return -1;
    }
    
    /* Créer le groupe s'il n'existe pas */
    hid_t group_id = create_group_if_not_exists(logger->file_id, group_path);
    if (group_id < 0) {
        return -1;
    }
    
    /* Ajouter ou mettre à jour l'attribut de limite de taille */
    hid_t attr_id;
    herr_t status;
    
    /* Vérifier si l'attribut existe déjà */
    htri_t attr_exists = H5Aexists(group_id, "max_entries");
    
    if (attr_exists > 0) {
        /* L'attribut existe, l'ouvrir */
        attr_id = H5Aopen(group_id, "max_entries", H5P_DEFAULT);
    } else {
        /* Créer un nouvel attribut */
        hid_t dataspace_id = H5Screate(H5S_SCALAR);
        attr_id = H5Acreate2(group_id, "max_entries", H5T_NATIVE_HSIZE, 
                           dataspace_id, H5P_DEFAULT, H5P_DEFAULT);
        H5Sclose(dataspace_id);
    }
    
    if (attr_id < 0) {
        H5Gclose(group_id);
        return -1;
    }
    
    /* Écrire la valeur de l'attribut */
    hsize_t hsize_max_entries = (hsize_t)max_entries;
    status = H5Awrite(attr_id, H5T_NATIVE_HSIZE, &hsize_max_entries);
    
    H5Aclose(attr_id);
    H5Gclose(group_id);
    
    return (status < 0) ? -1 : 0;
}

int hdf5_add_attribute(hdf5_logger_t* logger, const char* path, const char* attr_name,
                      const void* attr_value, int is_string) {
    if (logger == NULL || !logger->is_open || path == NULL || attr_name == NULL || attr_value == NULL) {
        return -1;
    }
    
    herr_t status;
    hid_t obj_id;
    
    /* Vérifier si l'objet existe */
    if (H5Lexists(logger->file_id, path, H5P_DEFAULT) <= 0) {
        return -1;
    }
    
    /* Ouvrir l'objet (groupe ou dataset) */
    H5O_info_t obj_info;
    status = H5Oget_info_by_name(logger->file_id, path, &obj_info, H5P_DEFAULT);
    if (status < 0) {
        return -1;
    }
    
    switch (obj_info.type) {
        case H5O_TYPE_GROUP:
            obj_id = H5Gopen2(logger->file_id, path, H5P_DEFAULT);
            break;
        case H5O_TYPE_DATASET:
            obj_id = H5Dopen2(logger->file_id, path, H5P_DEFAULT);
            break;
        default:
            return -1;
    }
    
    if (obj_id < 0) {
        return -1;
    }
    
    /* Créer ou remplacer l'attribut */
    hid_t attr_id;
    hid_t dataspace_id = H5Screate(H5S_SCALAR);
    
    /* Supprimer l'attribut s'il existe déjà */
    if (H5Aexists(obj_id, attr_name) > 0) {
        H5Adelete(obj_id, attr_name);
    }
    
    /* Créer l'attribut avec le type approprié */
    if (is_string) {
        hid_t str_type = H5Tcopy(H5T_C_S1);
        H5Tset_size(str_type, strlen((const char*)attr_value) + 1);
        attr_id = H5Acreate2(obj_id, attr_name, str_type, dataspace_id, H5P_DEFAULT, H5P_DEFAULT);
        status = H5Awrite(attr_id, str_type, attr_value);
        H5Tclose(str_type);
    } else {
        attr_id = H5Acreate2(obj_id, attr_name, H5T_NATIVE_DOUBLE, dataspace_id, H5P_DEFAULT, H5P_DEFAULT);
        status = H5Awrite(attr_id, H5T_NATIVE_DOUBLE, attr_value);
    }
    
    /* Nettoyage et fermeture */
    H5Aclose(attr_id);
    H5Sclose(dataspace_id);
    
    switch (obj_info.type) {
        case H5O_TYPE_GROUP:
            H5Gclose(obj_id);
            break;
        case H5O_TYPE_DATASET:
            H5Dclose(obj_id);
            break;
    }
    
    return (status < 0) ? -1 : 0;
}