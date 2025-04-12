# HDF5 Logger

Une bibliothèque d'interface C pour la création de logs au format HDF5, compatible avec Linux et Windows.

## Description

HDF5 Logger est une bibliothèque simple et légère pour l'enregistrement de logs dans le format HDF5. Elle fournit une interface uniforme pour stocker plusieurs types de données de logs :

- Logs texte (avec différents niveaux de gravité)
- Tableaux de données multidimensionnels
- Images

La bibliothèque est conçue pour fonctionner de manière identique sur Linux et Windows.

## Fonctionnalités

- Interface C simple
- Support multiplateforme (Linux/Windows)
- Gestion de différents types de logs
- Limite configurable sur la durée/quantité des logs
- Organisation hiérarchique des logs
- Compression des données

## Prérequis

- Compilateur C (GCC, Clang ou MSVC)
- Bibliothèque HDF5 (développement)
- CMake (>= 3.10)

## Compilation

### Linux

```bash
mkdir build && cd build
cmake ..
make
```

### Windows

```bash
mkdir build && cd build
cmake .. -G "Visual Studio 16 2019" -A Win32
cmake --build . --config Release
```

## Tests

Pour exécuter les tests :

```bash
cd build
ctest
```

## Utilisation

Exemple simple :

```c
#include "hdf5_logger.h"

int main() {
    // Initialiser un fichier de logs
    hdf5_logger_t* logger = hdf5_logger_init("mon_fichier_log.h5");
    
    // Ajouter un log texte
    hdf5_log_text(logger, HDF5_LOG_ERROR, "Une erreur s'est produite");
    
    // Fermer le logger et libérer les ressources
    hdf5_logger_close(logger);
    
    return 0;
}
```

## Licence

Ce projet est sous licence MIT. Voir le fichier LICENSE pour plus de détails.
