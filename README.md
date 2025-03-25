# Projet CHPS0901

## Description
Ce projet est une application qui simule l'algorithme de Barnes-Hut pour la simulation de systèmes de particules.

## Performances

### Comparaison des temps d'exécution

| Version                | Particules | Configuration            | Temps (s)   |
|------------------------|------------|--------------------------|-------------|
| Sequential             | 100k       | -                        | 128.761     |
| Sequential             | 800k       | -                        | 1842.83     |
| Parallel recur         | 100k       | 8 MPI, 24 threads        | 13.1497     |
| Parallel recur         | 100k       | 5 MPI, 24 threads        | 12.042      |
| Parallel recur         | 100k       | 4 MPI, 24 threads        | 7.8727      |
| Parallel no_recycle    | 100k       | 8 MPI, 24 threads        | 21.0021     |
| Parallel no_recycle    | 100k       | 5 MPI, 24 threads        | 19.9955     |
| Parallel no_recycle    | 100k       | 4 MPI, 24 threads        | 16.3927     |
| Parallel vect (stack)  | 100k       | 8 MPI, 24 threads        | 40.9011     |
| Parallel vect (stack)  | 100k       | 5 MPI, 24 threads        | 32.0044     |
| Parallel vect (stack)  | 100k       | 4 MPI, 24 threads        | 10.5198     |
| Parallel recur         | 800k       | 8 MPI, 24 threads        | 832.531     |

### Observations

- Les versions parallèles montrent une accélération significative par rapport à la version séquentielle.
- La configuration avec 4 MPI et 24 threads semble offrir les meilleures performances pour les versions parallèles.
- Les résultats peuvent varier en fonction de l'affinité des threads et du binding des NUMA nodes. Lors de nos tests, les tâches étaient réparties sur plusieurs nœuds sans binding spécifique, ce qui peut expliquer pourquoi certaines configurations avec moins de ressources (OMP) ont produit de meilleurs résultats.

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

## Compilation
Pour compiler l'application, vous pouvez utiliser les commandes suivantes dans le répertoire du projet :

- Pour compiler avec les bibliothèques graphiques :
```sh
make all
```

- Pour compiler sans les bibliothèques graphiques :
```sh
make all NO_VISUAL=1
```

- Pour compiler une version spécifique avec les bibliothèques graphiques :
```sh
make <specific_version>
```

- Pour compiler une version spécifique sans les bibliothèques graphiques :
```sh
make <specific_version> NO_VISUAL=1
```

Les versions spécifiques disponibles sont :
- `parallel_vect`
- `parallel_recur`
- `parallel_norecycle`
- `sequential`

## Structure du projet
Le projet est organisé comme suit pour chaque version situé dans le dossier 'versions' :
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
