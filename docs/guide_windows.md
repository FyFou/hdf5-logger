# Guide d'installation et d'utilisation sur Windows

## Prérequis

Pour compiler et utiliser HDF5 Logger sur Windows, vous aurez besoin de :

- Microsoft Visual Studio (2015 ou supérieur recommandé)
- CMake (version 3.10 ou supérieure)
- La bibliothèque HDF5 pour Windows

## Installation de HDF5

### Option 1 : Utiliser les binaires précompilés

1. Téléchargez la version précompilée de HDF5 depuis [le site officiel HDF Group](https://www.hdfgroup.org/downloads/hdf5)
2. Exécutez l'installateur et suivez les instructions
3. Notez le chemin d'installation (par défaut: `C:\Program Files\HDF_Group\HDF5\<version>`)

### Option 2 : Compiler depuis les sources

1. Téléchargez les sources de HDF5
2. Ouvrez une invite de commande pour développeurs Visual Studio
3. Exécutez les commandes suivantes :

```cmd
mkdir build
cd build
cmake -G "Visual Studio 16 2019" -A Win32 ...
cmake --build . --config Release
cmake --install . --prefix C:\HDF5
```

## Compilation de HDF5 Logger

1. Clonez le dépôt

```cmd
git clone https://github.com/FyFou/hdf5-logger.git
cd hdf5-logger
```

2. Créez et configurez le dossier de build

```cmd
mkdir build
cd build
```

3. Générez les fichiers de projet Visual Studio

Si HDF5 est installé à l'emplacement standard :

```cmd
cmake .. -G "Visual Studio 16 2019" -A Win32
```

Si HDF5 est installé à un emplacement personnalisé :

```cmd
cmake .. -G "Visual Studio 16 2019" -A Win32 -DHDF5_ROOT=C:\chemin\vers\HDF5
```

4. Compilez la bibliothèque

```cmd
cmake --build . --config Release
```

5. (Optionnel) Lancez les tests

```cmd
ctest -C Release
```

## Utilisation dans un projet Visual Studio

1. Copiez les fichiers d'en-tête et de bibliothèque :
   - `include/hdf5_logger.h` dans votre dossier d'inclusion
   - `build/Release/hdf5_logger.lib` dans votre dossier de bibliothèques

2. Configurez votre projet Visual Studio :
   - Ajoutez le chemin vers `hdf5_logger.h` dans les répertoires d'inclusion
   - Ajoutez le chemin vers `hdf5_logger.lib` dans les répertoires de bibliothèques
   - Ajoutez `hdf5_logger.lib`, `hdf5.lib` et `szip.lib` aux dépendances supplémentaires

3. Assurez-vous que les DLLs de HDF5 sont dans le PATH ou copiez-les dans le répertoire de votre exécutable

## Résolution des problèmes courants

### "hdf5.h" introuvable

Vérifiez que le chemin vers les en-têtes HDF5 est correctement configuré dans votre projet.

### Erreurs de liaison avec HDF5

Assurez-vous que vous utilisez la même architecture (32 ou 64 bits) pour HDF5 et votre application.

### Erreur de DLL introuvable à l'exécution

Vérifiez que les DLLs de HDF5 (hdf5.dll, szip.dll, zlib.dll) sont dans le PATH ou copiez-les dans le répertoire de votre exécutable.