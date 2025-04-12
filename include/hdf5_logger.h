/**
 * @file hdf5_logger.h
 * @brief Interface principale de la bibliothèque HDF5 Logger
 */

#ifndef HDF5_LOGGER_H
#define HDF5_LOGGER_H

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Définition des types de logs */
typedef enum {
    HDF5_LOG_DEBUG = 0,
    HDF5_LOG_INFO = 1,
    HDF5_LOG_WARNING = 2,
    HDF5_LOG_ERROR = 3,
    HDF5_LOG_CRITICAL = 4
} hdf5_log_level_t;

/* Structure principale du logger (opaque) */
typedef struct hdf5_logger_s hdf5_logger_t;

/**
 * @brief Initialise un nouveau logger HDF5
 * @param filename Nom du fichier HDF5 à créer/ouvrir
 * @return Pointeur vers le logger ou NULL en cas d'erreur
 */
hdf5_logger_t* hdf5_logger_init(const char* filename);

/**
 * @brief Ferme le logger et libère les ressources
 * @param logger Pointeur vers le logger
 * @return 0 en cas de succès, code d'erreur sinon
 */
int hdf5_logger_close(hdf5_logger_t* logger);

/**
 * @brief Définit la limite de temps pour la conservation des logs
 * @param logger Pointeur vers le logger
 * @param group_path Chemin du groupe à limiter dans le fichier HDF5
 * @param max_time_seconds Durée maximale de conservation en secondes
 * @return 0 en cas de succès, code d'erreur sinon
 */
int hdf5_logger_set_time_limit(hdf5_logger_t* logger, const char* group_path, double max_time_seconds);

/**
 * @brief Définit la limite de taille pour la conservation des logs
 * @param logger Pointeur vers le logger
 * @param group_path Chemin du groupe à limiter dans le fichier HDF5
 * @param max_entries Nombre maximal d'entrées à conserver
 * @return 0 en cas de succès, code d'erreur sinon
 */
int hdf5_logger_set_size_limit(hdf5_logger_t* logger, const char* group_path, size_t max_entries);

/**
 * @brief Ajoute un log texte
 * @param logger Pointeur vers le logger
 * @param level Niveau du log
 * @param message Message de log
 * @return 0 en cas de succès, code d'erreur sinon
 */
int hdf5_log_text(hdf5_logger_t* logger, hdf5_log_level_t level, const char* message);

/**
 * @brief Ajoute un log texte dans un groupe spécifique
 * @param logger Pointeur vers le logger
 * @param group_path Chemin du groupe dans le fichier HDF5
 * @param level Niveau du log
 * @param message Message de log
 * @return 0 en cas de succès, code d'erreur sinon
 */
int hdf5_log_text_to_group(hdf5_logger_t* logger, const char* group_path, 
                          hdf5_log_level_t level, const char* message);

/**
 * @brief Ajoute un tableau à une dimension
 * @param logger Pointeur vers le logger
 * @param group_path Chemin du groupe dans le fichier HDF5
 * @param dataset_name Nom du dataset pour ce tableau
 * @param data Données à enregistrer
 * @param size Taille du tableau
 * @param is_double 1 si les données sont des doubles, 0 si flottants
 * @return 0 en cas de succès, code d'erreur sinon
 */
int hdf5_log_array_1d(hdf5_logger_t* logger, const char* group_path, const char* dataset_name,
                     const void* data, size_t size, int is_double);

/**
 * @brief Ajoute un tableau à deux dimensions
 * @param logger Pointeur vers le logger
 * @param group_path Chemin du groupe dans le fichier HDF5
 * @param dataset_name Nom du dataset pour ce tableau
 * @param data Données à enregistrer
 * @param rows Nombre de lignes
 * @param cols Nombre de colonnes
 * @param is_double 1 si les données sont des doubles, 0 si flottants
 * @return 0 en cas de succès, code d'erreur sinon
 */
int hdf5_log_array_2d(hdf5_logger_t* logger, const char* group_path, const char* dataset_name,
                     const void* data, size_t rows, size_t cols, int is_double);

/**
 * @brief Ajoute un tableau à trois dimensions
 * @param logger Pointeur vers le logger
 * @param group_path Chemin du groupe dans le fichier HDF5
 * @param dataset_name Nom du dataset pour ce tableau
 * @param data Données à enregistrer
 * @param dim1 Première dimension
 * @param dim2 Deuxième dimension
 * @param dim3 Troisième dimension
 * @param is_double 1 si les données sont des doubles, 0 si flottants
 * @return 0 en cas de succès, code d'erreur sinon
 */
int hdf5_log_array_3d(hdf5_logger_t* logger, const char* group_path, const char* dataset_name,
                     const void* data, size_t dim1, size_t dim2, size_t dim3, int is_double);

/**
 * @brief Ajoute une image
 * @param logger Pointeur vers le logger
 * @param group_path Chemin du groupe dans le fichier HDF5
 * @param image_name Nom du dataset pour cette image
 * @param pixel_data Données de pixels de l'image
 * @param width Largeur de l'image
 * @param height Hauteur de l'image
 * @param channels Nombre de canaux (1 pour grayscale, 3 pour RGB, 4 pour RGBA)
 * @return 0 en cas de succès, code d'erreur sinon
 */
int hdf5_log_image(hdf5_logger_t* logger, const char* group_path, const char* image_name,
                  const unsigned char* pixel_data, size_t width, size_t height, size_t channels);

/**
 * @brief Ajoute un attribut à un groupe ou dataset
 * @param logger Pointeur vers le logger
 * @param path Chemin complet du groupe ou dataset
 * @param attr_name Nom de l'attribut
 * @param attr_value Valeur de l'attribut
 * @param is_string 1 si la valeur est une chaîne, 0 si c'est un double
 * @return 0 en cas de succès, code d'erreur sinon
 */
int hdf5_add_attribute(hdf5_logger_t* logger, const char* path, const char* attr_name,
                      const void* attr_value, int is_string);

/**
 * @brief Récupère la version de la bibliothèque
 * @return Chaîne de caractères décrivant la version
 */
const char* hdf5_logger_version();

#ifdef __cplusplus
}
#endif

#endif /* HDF5_LOGGER_H */
