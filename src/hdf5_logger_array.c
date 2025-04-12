/**
 * @file hdf5_logger_array.c
 * @brief Implémentation des fonctions de log de tableaux
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "hdf5.h"
#include "../include/hdf5_logger.h"

/* Fonction interne pour créer un groupe s'il n'existe pas */
extern hid_t create_group_if_not_exists(hid_t file_id, const char* group_path);

/* Implémentation interne pour les tableaux */
static int log_array_internal(hid_t file_id, const char* group_path, const char* dataset_name,
                            const void* data, int rank, hsize_t* dims, int is_double) {
    if (file_id < 0 || group_path == NULL || dataset_name == NULL || data == NULL || dims == NULL) {
        return -1;
    }
    
    herr_t status;
    hid_t group_id, dataset_id, dataspace_id, datatype_id;
    
    /* Créer le groupe s'il n'existe pas */
    group_id = create_group_if_not_exists(file_id, group_path);
    if (group_id < 0) {
        return -1;
    }
    
    /* Créer l'espace de données pour le tableau */
    dataspace_id = H5Screate_simple(rank, dims, NULL);
    if (dataspace_id < 0) {
        H5Gclose(group_id);
        return -1;
    }
    
    /* Déterminer le type de données (float ou double) */
    datatype_id = is_double ? H5T_NATIVE_DOUBLE : H5T_NATIVE_FLOAT;
    
    /* Créer le dataset avec compression */
    hid_t plist_id = H5Pcreate(H5P_DATASET_CREATE);
    
    /* Configurer la compression si le tableau est assez grand */
    size_t total_elements = 1;
    for (int i = 0; i < rank; i++) {
        total_elements *= dims[i];
    }
    
    if (total_elements > 100) {
        /* Configurer le chunking pour les tableaux volumineux */
        hsize_t chunk_dims[3] = {1, 1, 1};  /* Valeurs par défaut */
        
        /* Ajuster les dimensions de chunk selon la taille et le rang */
        for (int i = 0; i < rank && i < 3; i++) {
            chunk_dims[i] = (dims[i] > 20) ? 20 : dims[i];
        }
        
        status = H5Pset_chunk(plist_id, rank, chunk_dims);
        
        /* Activer la compression GZIP niveau 6 */
        status = H5Pset_deflate(plist_id, 6);
    }
    
    /* Vérifier si le dataset existe déjà */
    if (H5Lexists(group_id, dataset_name, H5P_DEFAULT) > 0) {
        H5Ldelete(group_id, dataset_name, H5P_DEFAULT);  /* Supprimer l'ancien dataset */
    }
    
    /* Créer le dataset */
    dataset_id = H5Dcreate2(group_id, dataset_name, datatype_id, dataspace_id,
                          H5P_DEFAULT, plist_id, H5P_DEFAULT);
    if (dataset_id < 0) {
        H5Pclose(plist_id);
        H5Sclose(dataspace_id);
        H5Gclose(group_id);
        return -1;
    }
    
    /* Écrire les données */
    status = H5Dwrite(dataset_id, datatype_id, H5S_ALL, H5S_ALL, H5P_DEFAULT, data);
    
    /* Ajouter un attribut pour l'horodatage */
    hid_t attr_space = H5Screate(H5S_SCALAR);
    hid_t attr_id = H5Acreate2(dataset_id, "timestamp", H5T_NATIVE_DOUBLE, attr_space,
                             H5P_DEFAULT, H5P_DEFAULT);
    
    double timestamp = (double)time(NULL);
    H5Awrite(attr_id, H5T_NATIVE_DOUBLE, &timestamp);
    
    /* Nettoyage */
    H5Aclose(attr_id);
    H5Sclose(attr_space);
    H5Dclose(dataset_id);
    H5Pclose(plist_id);
    H5Sclose(dataspace_id);
    H5Gclose(group_id);
    
    return (status < 0) ? -1 : 0;
}

/* Implémentation des fonctions publiques */

int hdf5_log_array_1d(hdf5_logger_t* logger, const char* group_path, const char* dataset_name,
                     const void* data, size_t size, int is_double) {
    if (logger == NULL || !logger->is_open || group_path == NULL || 
        dataset_name == NULL || data == NULL || size == 0) {
        return -1;
    }
    
    hsize_t dims[1] = {size};
    return log_array_internal(logger->file_id, group_path, dataset_name, data, 1, dims, is_double);
}

int hdf5_log_array_2d(hdf5_logger_t* logger, const char* group_path, const char* dataset_name,
                     const void* data, size_t rows, size_t cols, int is_double) {
    if (logger == NULL || !logger->is_open || group_path == NULL || 
        dataset_name == NULL || data == NULL || rows == 0 || cols == 0) {
        return -1;
    }
    
    hsize_t dims[2] = {rows, cols};
    return log_array_internal(logger->file_id, group_path, dataset_name, data, 2, dims, is_double);
}

int hdf5_log_array_3d(hdf5_logger_t* logger, const char* group_path, const char* dataset_name,
                     const void* data, size_t dim1, size_t dim2, size_t dim3, int is_double) {
    if (logger == NULL || !logger->is_open || group_path == NULL || 
        dataset_name == NULL || data == NULL || dim1 == 0 || dim2 == 0 || dim3 == 0) {
        return -1;
    }
    
    hsize_t dims[3] = {dim1, dim2, dim3};
    return log_array_internal(logger->file_id, group_path, dataset_name, data, 3, dims, is_double);
}