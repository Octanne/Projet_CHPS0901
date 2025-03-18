#!/bin/bash
#SBATCH --job-name=chps0901_1mpi_32thds         # Job name
#SBATCH --output=results/chps0901_1mpi_32thds_%j.txt  # Output file (%j will be replaced with the job ID)
#SBATCH --ntasks=1                             # Number of MPI processes (n processes)
#SBATCH --cpus-per-task=32                     # Number of CPU cores per MPI process (for OpenMP threads)
#SBATCH --time=02:00:00                        # Maximum runtime (1 hour)
#SBATCH --mem=64G                              # Memory allocation
#SBATCH --account=M24090                       # Projet name
# --partition=standard                         # Partition to use (change based on your cluster setup)

# Load OpenMPI module
spack load openmpi@4.1.6                       # Load OpenMPI module if available
spack load gcc@13.1.0
# Augmente la stack à "illimitée" (avant l'exécution)
ulimit -s unlimited

export LD_LIBRARY_PATH="$(pwd)/libs/install/lib64:$LD_LIBRARY_PATH"
# Set the number of threads for OpenMP (if not set in code or environment)
export OMP_NUM_THREADS=$SLURM_CPUS_PER_TASK  # Use the number of CPU cores per task

# Run the program using mpirun with OpenMP parallelism inside each MPI process
mpirun -n $SLURM_NTASKS bin/main -n 800000 -e 100 -t 1 -w 100000

