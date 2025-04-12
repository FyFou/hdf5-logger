/**
 * @file hdf5_logger_text.c
 * @brief Implémentation des fonctions de log texte
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "hdf5.h"
#include "../include/hdf5_logger.h"

/* Structure pour les entrées de log texte */
typedef struct {
    int log_level;       /* Niveau de log */
    double timestamp;    /* Horodatage */
    char message[1024];  /* Message (taille fixe pour simplifier) */
} text_log_entry_t;

/* Fonction interne pour créer un groupe s'il n'existe pas */
extern hid_t create_group_if_not_exists(hid_t file_id, const char* group_path);

/* Implémentation interne de l'ajout de log texte */
static int add_text_log_entry(hid_t file_id, const char* group_path, 
                             hdf5_log_level_t level, const char* message) {
    if (file_id < 0 || group_path == NULL || message == NULL) {
        return -1;
    }
    
    herr_t status;
    hid_t group_id, dataset_id, dataspace_id, datatype_id;
    hsize_t dims[1], new_dims[1];
    
    /* Créer le groupe s'il n'existe pas */
    group_id = create_group_if_not_exists(file_id, group_path);
    if (group_id < 0) {
        return -1;
    }
    
    /* Créer un type composé pour l'entrée de log */
    datatype_id = H5Tcreate(H5T_COMPOUND, sizeof(text_log_entry_t));
    H5Tinsert(datatype_id, "log_level", HOFFSET(text_log_entry_t, log_level), H5T_NATIVE_INT);
    H5Tinsert(datatype_id, "timestamp", HOFFSET(text_log_entry_t, timestamp), H5T_NATIVE_DOUBLE);
    
    /* Pour le message, créer un type chaîne */
    hid_t string_type = H5Tcopy(H5T_C_S1);
    H5Tset_size(string_type, sizeof(((text_log_entry_t*)0)->message));
    H5Tinsert(datatype_id, "message", HOFFSET(text_log_entry_t, message), string_type);
    
    /* Déterminer s'il faut créer ou ouvrir le dataset */
    const char* dataset_name = "log_entries";
    htri_t dataset_exists = H5Lexists(group_id, dataset_name, H5P_DEFAULT);
    
    if (dataset_exists > 0) {
        /* Ouvrir le dataset existant */
        dataset_id = H5Dopen2(group_id, dataset_name, H5P_DEFAULT);
        dataspace_id = H5Dget_space(dataset_id);
        H5Sget_simple_extent_dims(dataspace_id, dims, NULL);
        H5Sclose(dataspace_id);
        
        /* Lire l'attribut de limite de taille si présent */
        htri_t size_limit_exists = H5Aexists(group_id, "max_entries");
        if (size_limit_exists > 0) {
            hid_t attr_id = H5Aopen(group_id, "max_entries", H5P_DEFAULT);
            hsize_t max_entries;
            H5Aread(attr_id, H5T_NATIVE_HSIZE, &max_entries);
            H5Aclose(attr_id);
            
            /* Si la limite est atteinte, supprimer l'entrée la plus ancienne */
            if (dims[0] >= max_entries) {
                /* Décaler les entrées (supprimer la plus ancienne) */
                if (dims[0] > 1) {
                    text_log_entry_t* buffer = malloc((dims[0] - 1) * sizeof(text_log_entry_t));
                    if (buffer) {
                        /* Lire toutes les entrées sauf la première */
                        hsize_t start[1] = {1};
                        hsize_t count[1] = {dims[0] - 1};
                        
                        dataspace_id = H5Dget_space(dataset_id);
                        status = H5Sselect_hyperslab(dataspace_id, H5S_SELECT_SET, 
                                                  start, NULL, count, NULL);
                        
                        hid_t mem_space = H5Screate_simple(1, count, NULL);
                        status = H5Dread(dataset_id, datatype_id, mem_space, dataspace_id, 
                                       H5P_DEFAULT, buffer);
                        
                        /* Écrire ces entrées au début */
                        start[0] = 0;
                        status = H5Sselect_hyperslab(dataspace_id, H5S_SELECT_SET, 
                                                  start, NULL, count, NULL);
                        status = H5Dwrite(dataset_id, datatype_id, mem_space, dataspace_id, 
                                        H5P_DEFAULT, buffer);
                        
                        H5Sclose(mem_space);
                        H5Sclose(dataspace_id);
                        free(buffer);
                    }
                }
            } else {
                /* Augmenter la taille pour la nouvelle entrée */
                new_dims[0] = dims[0] + 1;
                status = H5Dset_extent(dataset_id, new_dims);
            }
        } else {
            /* Pas de limite, simplement augmenter la taille */
            new_dims[0] = dims[0] + 1;
            status = H5Dset_extent(dataset_id, new_dims);
        }
    } else {
        /* Créer un nouveau dataset extensible */
        dims[0] = 0;
        hsize_t maxdims[1] = {H5S_UNLIMITED};
        dataspace_id = H5Screate_simple(1, dims, maxdims);
        
        /* Utiliser le chunking pour permettre les extensions */
        hid_t plist_id = H5Pcreate(H5P_DATASET_CREATE);
        hsize_t chunk_dims[1] = {64};  /* taille de chunk optimisée */
        H5Pset_chunk(plist_id, 1, chunk_dims);
        
        dataset_id = H5Dcreate2(group_id, dataset_name, datatype_id, dataspace_id, 
                              H5P_DEFAULT, plist_id, H5P_DEFAULT);
        
        H5Pclose(plist_id);
        H5Sclose(dataspace_id);
        
        new_dims[0] = 1;  /* Première entrée */
        status = H5Dset_extent(dataset_id, new_dims);
    }
    
    /* Préparer l'entrée de log */
    text_log_entry_t entry;
    entry.log_level = (int)level;
    entry.timestamp = (double)time(NULL);  /* Timestamp Unix (secondes) */
    strncpy(entry.message, message, sizeof(entry.message) - 1);
    entry.message[sizeof(entry.message) - 1] = '\0';  /* S'assurer que la chaîne est terminée */
    
    /* Lire l'attribut de limite de temps si présent */
    htri_t time_limit_exists = H5Aexists(group_id, "max_time_seconds");
    if (time_limit_exists > 0 && dims[0] > 0) {
        hid_t attr_id = H5Aopen(group_id, "max_time_seconds", H5P_DEFAULT);
        double max_time_seconds;
        H5Aread(attr_id, H5T_NATIVE_DOUBLE, &max_time_seconds);
        H5Aclose(attr_id);
        
        /* Obtenir le timestamp du premier élément pour vérifier la limite de temps */
        text_log_entry_t first_entry;
        hsize_t start[1] = {0};
        hsize_t count[1] = {1};
        
        dataspace_id = H5Dget_space(dataset_id);
        status = H5Sselect_hyperslab(dataspace_id, H5S_SELECT_SET, start, NULL, count, NULL);
        
        hid_t mem_space = H5Screate_simple(1, count, NULL);
        status = H5Dread(dataset_id, datatype_id, mem_space, dataspace_id, H5P_DEFAULT, &first_entry);
        
        H5Sclose(mem_space);
        H5Sclose(dataspace_id);
        
        /* Si la fenêtre de temps est dépassée, supprimer les anciennes entrées */
        if ((entry.timestamp - first_entry.timestamp) > max_time_seconds) {
            /* Dans cet exemple simplifié, on supprime tout et on recommence */
            /* Une implémentation plus sophistiquée analyserait chaque entrée */
            H5Dclose(dataset_id);
            H5Ldelete(group_id, dataset_name, H5P_DEFAULT);
            
            /* Recréer le dataset */
            dims[0] = 0;
            hsize_t maxdims[1] = {H5S_UNLIMITED};
            dataspace_id = H5Screate_simple(1, dims, maxdims);
            
            hid_t plist_id = H5Pcreate(H5P_DATASET_CREATE);
            hsize_t chunk_dims[1] = {64};
            H5Pset_chunk(plist_id, 1, chunk_dims);
            
            dataset_id = H5Dcreate2(group_id, dataset_name, datatype_id, dataspace_id, 
                                  H5P_DEFAULT, plist_id, H5P_DEFAULT);
            
            H5Pclose(plist_id);
            H5Sclose(dataspace_id);
            
            new_dims[0] = 1;  /* Nouvelle première entrée */
            status = H5Dset_extent(dataset_id, new_dims);
        }
    }
    
    /* Écrire l'entrée à la position appropriée */
    hsize_t start[1] = {new_dims[0] - 1};
    hsize_t count[1] = {1};
    
    dataspace_id = H5Dget_space(dataset_id);
    status = H5Sselect_hyperslab(dataspace_id, H5S_SELECT_SET, start, NULL, count, NULL);
    
    hid_t mem_space = H5Screate_simple(1, count, NULL);
    status = H5Dwrite(dataset_id, datatype_id, mem_space, dataspace_id, H5P_DEFAULT, &entry);
    
    /* Nettoyage */
    H5Sclose(mem_space);
    H5Sclose(dataspace_id);
    H5Dclose(dataset_id);
    H5Tclose(string_type);
    H5Tclose(datatype_id);
    H5Gclose(group_id);
    
    return (status < 0) ? -1 : 0;
}

/* Implémentation des fonctions publiques */

int hdf5_log_text(hdf5_logger_t* logger, hdf5_log_level_t level, const char* message) {
    if (logger == NULL || message == NULL) {
        return -1;
    }
    
    /* Déterminer le chemin du groupe en fonction du niveau de log */
    const char* group_path;
    switch (level) {
        case HDF5_LOG_DEBUG:
            group_path = "/text_logs/debug";
            break;
        case HDF5_LOG_INFO:
            group_path = "/text_logs/info";
            break;
        case HDF5_LOG_WARNING:
            group_path = "/text_logs/warnings";
            break;
        case HDF5_LOG_ERROR:
            group_path = "/text_logs/errors";
            break;
        case HDF5_LOG_CRITICAL:
            group_path = "/text_logs/critical";
            break;
        default:
            group_path = "/text_logs/unknown";
            break;
    }
    
    return add_text_log_entry(logger->file_id, group_path, level, message);
}

int hdf5_log_text_to_group(hdf5_logger_t* logger, const char* group_path, 
                          hdf5_log_level_t level, const char* message) {
    if (logger == NULL || group_path == NULL || message == NULL) {
        return -1;
    }
    
    return add_text_log_entry(logger->file_id, group_path, level, message);
}