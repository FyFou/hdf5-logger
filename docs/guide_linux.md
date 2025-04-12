# Guide d'installation et d'utilisation sur Linux

## Prérequis

Pour compiler et utiliser HDF5 Logger sur Linux, vous aurez besoin de :

- GCC ou Clang
- CMake (version 3.10 ou supérieure)
- La bibliothèque HDF5 (dev)

## Installation de HDF5

### Debian/Ubuntu

```bash
sudo apt update
sudo apt install libhdf5-dev
```

### Fedora/RHEL/CentOS

```bash
sudo dnf install hdf5-devel
```

### Arch Linux

```bash
sudo pacman -S hdf5
```

### Compilation depuis les sources

```bash
wget https://support.hdfgroup.org/ftp/HDF5/releases/hdf5-1.12/hdf5-1.12.1/src/hdf5-1.12.1.tar.gz
tar -xzf hdf5-1.12.1.tar.gz
cd hdf5-1.12.1
./configure --prefix=/usr/local
make
sudo make install
```

## Compilation de HDF5 Logger

1. Clonez le dépôt

```bash
git clone https://github.com/FyFou/hdf5-logger.git
cd hdf5-logger
```

2. Créez et configurez le dossier de build

```bash
mkdir build && cd build
```

3. Générez les makefiles et compilez

Si HDF5 est installé à l'emplacement standard :

```bash
cmake ..
make
```

Si HDF5 est installé à un emplacement personnalisé :

```bash
cmake .. -DHDF5_ROOT=/chemin/vers/hdf5
make
```

4. (Optionnel) Lancez les tests

```bash
ctest
```

5. (Optionnel) Installez la bibliothèque

```bash
sudo make install
```

## Compilation d'un programme utilisant HDF5 Logger

### Avec CMake

Dans votre `CMakeLists.txt` :

```cmake
find_package(hdf5_logger REQUIRED)

add_executable(mon_programme main.c)
target_link_libraries(mon_programme hdf5_logger)
```

### Avec GCC directement

```bash
gcc -o mon_programme main.c -lhdf5_logger -lhdf5
```

## Support de l'architecture 32 bits

Pour compiler en 32 bits sur un système 64 bits :

### 1. Installer les bibliothèques 32 bits nécessaires

#### Debian/Ubuntu

```bash
sudo dpkg --add-architecture i386
sudo apt update
sudo apt install libhdf5-dev:i386 gcc-multilib
```

### 2. Compiler HDF5 Logger en 32 bits

```bash
mkdir build32 && cd build32
CFLAGS=-m32 cmake ..
make
```

## Résolution des problèmes courants

### Bibliothèque HDF5 introuvable

Assurez-vous que les fichiers de développement HDF5 sont installés. Vérifiez que `pkg-config --libs hdf5` renvoie le chemin correct.

### Erreurs de compilation en 32 bits

Vérifiez que toutes les dépendances 32 bits sont installées et que vous utilisez les bons flags de compilation.

### Erreurs de performance

Considérez la compilation avec optimisations :

```bash
CMAKE_BUILD_TYPE=Release cmake ..
make
```

### Erreurs lors de la configuration CMake

Si CMake ne trouve pas HDF5, spécifiez explicitement son emplacement :

```bash
cmake .. -DHDF5_ROOT=/chemin/vers/hdf5
```