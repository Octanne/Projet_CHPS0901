# Projet CHPS0901

## Description
Ce projet est une application qui simule l'algorithme de Barnes-Hut pour la simulation de systèmes de particules.

## Prérequis
Avant de pouvoir utiliser et compiler l'application, assurez-vous d'avoir installé les bibliothèques nécessaires.

### Debian
```sh
sudo apt update
sudo apt install cmake g++ make libglfw3-dev libglew-dev libglm-dev
```

### ArchLinux
```sh
sudo pacman -Syu
sudo pacman -S cmake gcc make glfw-3 glew glm
```

### RHEL
```sh
sudo yum update
sudo yum install cmake gcc-c++ make glfw-devel glew-devel glm-devel
```

## Structure du projet
Le projet est organisé comme suit :
```
Projet_CHPS0901/
├── bin/         # Contient les exécutables compilés
├── headers/     # Contient les fichiers d'en-tête (.h)
├── obj/         # Contient les fichiers objets (.o)
├── particles/   # Contient les fichiers de configuration des particules
├── results/     # Contient les résultats des simulations
├── sources/     # Contient les fichiers source (.cpp)
├── Makefile     # Fichier Makefile pour la compilation
└── README.md    # Fichier README avec les instructions
```

## Compilation
Pour compiler l'application, exécutez la commande suivante dans le répertoire du projet :
```sh
make
```

## Utilisation
Après la compilation, vous pouvez exécuter l'application avec la commande suivante :
```sh
./bin/Projet_CHPS0901 -w <window_size> -n <num_particles> [options]
```
Options disponibles :
- `-G <max_mass>` : Masse maximale des particules.
- `-L <min_mass>` : Masse minimale des particules.
- `-g` : Activer l'interface graphique.
- `-f <file>` : Charger les particules depuis un fichier.
- `-e <nb_steps>` : Nombre de pas de simulation.
- `-r <refresh_rate>` : Taux de rafraîchissement de l'affichage.
- `-d` : Activer le mode débogage.
- `-t <time_step>` : Pas de temps de la simulation.

N'hésitez pas à utiliser l'option `-h` pour voir l'aide complète.

### Explication des dossiers
- `results/` : Ce dossier contient les résultats des simulations, tels que les positions finales des particules et les statistiques de performance.
- `particles/` : Ce dossier contient les fichiers de configuration des particules, qui définissent les propriétés initiales des particules pour les simulations.

## Crédits
Cette application utilise les bibliothèques suivantes :
- `stb_image` - Une bibliothèque pour le chargement d'images. (font)
- `GLFW` - Une bibliothèque pour la gestion des fenêtres et des entrées.
- `GLEW` - Une bibliothèque pour le chargement des extensions OpenGL.

Merci aux développeurs de ces bibliothèques pour leur travail précieux.

## Licence
Ce projet est distribué sous la licence MIT.

## Auteurs
- Corentin Levalet - Développeur GUI, Simulation et OpenMPI
- Théo Ponsin - Développeur Simulation et OpenMPI

Pour toute question ou suggestion, n'hésitez pas à nous contacter.
