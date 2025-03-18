#!/bin/bash

# Usage: ./submit_all.sh <directory> [file_pattern]

if [ $# -lt 1 ]; then
    echo "Usage: $0 <directory> [file_pattern]"
    exit 1
fi

directory="$1"
pattern="${2:-*}"  # Par défaut tous les fichiers

# Trouver et soumettre les fichiers récursivement
find "$directory" -type f -name "$pattern" | while read -r file; do
    if [ -f "$file" ]; then
        echo "Submitting $file with sbatch"
        sbatch "$file"
        sleep 1  # Petit délai pour éviter de surcharger le scheduler
    else
        echo "Skipping non-file: $file"
    fi
done

echo "All jobs submitted!"