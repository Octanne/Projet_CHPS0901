import sys
import re



# Vérification des arguments
if len(sys.argv) != 2:
    print("Usage: python tab.py <fichier>")
    sys.exit(1)

# Chargement du contenu du fichier
file_path = sys.argv[1]
try:
    with open(file_path, 'r') as file:
        data = file.read()
except FileNotFoundError:
    print(f"Erreur : le fichier '{file_path}' n'existe pas.")
    sys.exit(1)

# Expression régulière pour extraire les groupes
pattern = r"Number of MPI processes: (?P<mpi>[0-9]+)\nNumber of OpenMP threads: (?P<omp>[0-9]+)\nStarting simulation with [0-9]+ particles\nSimulation time: (?P<time>[0-9.]+) seconds\n"
# Fonction pour extraire les groupes
def extract_data(text):
    return re.findall(pattern, text)

# Fonction pour afficher le temps pour un nombre donné de processus MPI
def display_time_for_mpi(mpi_value, text):
    # Extraction des données avec l'expression régulière
    results = extract_data(text)
    
    # Parcours des résultats et affichage du temps pour les valeurs MPI correspondantes
    for result in results:
        mpi, omp, time = result
        if int(mpi) == mpi_value:
            print(f"0:0:{time}")
            
# Fonction pour afficher le temps pour un nombre donné de processus MPI
def display_time(text):
    # Extraction des données avec l'expression régulière
    results = extract_data(text)
    tabVal = [['','','','','','','','','','','','','','','','','','','','','','','',''],
    ['','','','','','','','','','','','','','','','','','','','','','','',''],
    ['','','','','','','','','','','','','','','','','','','','','','','',''],
    ['','','','','','','','','','','','','','','','','','','','','','','',''],
    ['','','','','','','','','','','','','','','','','','','','','','','',''],
    ['','','','','','','','','','','','','','','','','','','','','','','',''],
    ['','','','','','','','','','','','','','','','','','','','','','','',''],
    ['','','','','','','','','','','','','','','','','','','','','','','','']]
    # Parcours des résultats et affichage du temps pour les valeurs MPI correspondantes
    for result in results:
        mpi, omp, time = result
        tabVal[int(mpi)-1][int(omp)-1] = time
    for omp in range(0,24):
        line = ""
        for mpi in range(0,8):
            if tabVal[mpi][omp] == '':
                line = line + "NoData" + " "
            else:
                line = line + "0:0:" + str(tabVal[mpi][omp]) + " "
        print(line)

# Exemple d'utilisation : afficher le temps pour les simulations où le nombre de processus MPI est 4
display_time(data)
