/**
 * @file hdf5_logger_image.c
 * @brief Implémentation des fonctions de log d'images
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "hdf5.h"
#include "../include/hdf5_logger.h"

/* Fonction interne pour créer un groupe s'il n'existe pas */
extern hid_t create_group_if_not_exists(hid_t file_id, const char* group_path);

int hdf5_log_image(hdf5_logger_t* logger, const char* group_path, const char* image_name,
                  const unsigned char* pixel_data, size_t width, size_t height, size_t channels) {
    if (logger == NULL || !logger->is_open || group_path == NULL || image_name == NULL ||
        pixel_data == NULL || width == 0 || height == 0 || channels == 0 || channels > 4) {
        return -1;
    }
    
    herr_t status;
    hid_t group_id, dataset_id, dataspace_id;
    
    /* Créer le groupe s'il n'existe pas */
    group_id = create_group_if_not_exists(logger->file_id, group_path);
    if (group_id < 0) {
        return -1;
    }
    
    /* Créer les dimensions pour l'image */
    hsize_t dims[3];
    int rank;
    
    if (channels == 1) {
        /* Image grayscale (2D) */
        dims[0] = height;
        dims[1] = width;
        rank = 2;
    } else {
        /* Image couleur (3D) */
        dims[0] = height;
        dims[1] = width;
        dims[2] = channels;
        rank = 3;
    }
    
    /* Créer l'espace de données pour l'image */
    dataspace_id = H5Screate_simple(rank, dims, NULL);
    if (dataspace_id < 0) {
        H5Gclose(group_id);
        return -1;
    }
    
    /* Configurer la compression pour l'image */
    hid_t plist_id = H5Pcreate(H5P_DATASET_CREATE);
    
    /* Définir les dimensions de chunk */
    hsize_t chunk_dims[3];
    
    if (rank == 2) {
        chunk_dims[0] = (height > 128) ? 128 : height;
        chunk_dims[1] = (width > 128) ? 128 : width;
    } else {
        chunk_dims[0] = (height > 128) ? 128 : height;
        chunk_dims[1] = (width > 128) ? 128 : width;
        chunk_dims[2] = channels;
    }
    
    status = H5Pset_chunk(plist_id, rank, chunk_dims);
    
    /* Activer la compression GZIP niveau 7 (plus agressive pour les images) */
    status = H5Pset_deflate(plist_id, 7);
    
    /* Vérifier si le dataset existe déjà */
    if (H5Lexists(group_id, image_name, H5P_DEFAULT) > 0) {
        H5Ldelete(group_id, image_name, H5P_DEFAULT);  /* Supprimer l'ancien dataset */
    }
    
    /* Créer le dataset */
    dataset_id = H5Dcreate2(group_id, image_name, H5T_NATIVE_UCHAR, dataspace_id,
                          H5P_DEFAULT, plist_id, H5P_DEFAULT);
    if (dataset_id < 0) {
        H5Pclose(plist_id);
        H5Sclose(dataspace_id);
        H5Gclose(group_id);
        return -1;
    }
    
    /* Écrire les données de l'image */
    status = H5Dwrite(dataset_id, H5T_NATIVE_UCHAR, H5S_ALL, H5S_ALL, H5P_DEFAULT, pixel_data);
    
    /* Ajouter des attributs pour les métadonnées de l'image */
    hid_t attr_space = H5Screate(H5S_SCALAR);
    
    /* Width */
    hid_t width_attr = H5Acreate2(dataset_id, "width", H5T_NATIVE_UINT, attr_space,
                                 H5P_DEFAULT, H5P_DEFAULT);
    unsigned int w = (unsigned int)width;
    H5Awrite(width_attr, H5T_NATIVE_UINT, &w);
    H5Aclose(width_attr);
    
    /* Height */
    hid_t height_attr = H5Acreate2(dataset_id, "height", H5T_NATIVE_UINT, attr_space,
                                  H5P_DEFAULT, H5P_DEFAULT);
    unsigned int h = (unsigned int)height;
    H5Awrite(height_attr, H5T_NATIVE_UINT, &h);
    H5Aclose(height_attr);
    
    /* Channels */
    hid_t channels_attr = H5Acreate2(dataset_id, "channels", H5T_NATIVE_UINT, attr_space,
                                    H5P_DEFAULT, H5P_DEFAULT);
    unsigned int c = (unsigned int)channels;
    H5Awrite(channels_attr, H5T_NATIVE_UINT, &c);
    H5Aclose(channels_attr);
    
    /* Timestamp */
    hid_t timestamp_attr = H5Acreate2(dataset_id, "timestamp", H5T_NATIVE_DOUBLE, attr_space,
                                     H5P_DEFAULT, H5P_DEFAULT);
    double timestamp = (double)time(NULL);
    H5Awrite(timestamp_attr, H5T_NATIVE_DOUBLE, &timestamp);
    H5Aclose(timestamp_attr);
    
    /* Nettoyage */
    H5Sclose(attr_space);
    H5Dclose(dataset_id);
    H5Pclose(plist_id);
    H5Sclose(dataspace_id);
    H5Gclose(group_id);
    
    return (status < 0) ? -1 : 0;
}