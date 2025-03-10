#!/bin/bash
#SBATCH --job-name=n_corps_chps0901    # Job name
#SBATCH --output=results/output_ncorps_%j.txt  # Output file (%j will be replaced with the job ID)
#SBATCH --ntasks=5                     # Number of MPI processes (n processes)
#SBATCH --cpus-per-task=16             # Number of CPU cores per MPI process (for OpenMP threads)
#SBATCH --time=01:00:00                # Maximum runtime (1 hour)
#SBATCH --mem=20G                      # Memory allocation
#SBATCH --account=M24090               # Projet name
# --partition=standard                 # Partition to use (change based on your cluster setup)

# Load OpenMPI module
spack load openmpi@4.1.6               # Load OpenMPI module if available
spack load gcc@13.1.0
spack load maqao@2.20.13

# Set the number of threads for OpenMP (if not set in code or environment)
export LD_LIBRARY_PATH="$(pwd)/libs/install/lib64:$LD_LIBRARY_PATH"
export OMP_NUM_THREADS=$SLURM_CPUS_PER_TASK  # Use the number of CPU cores per task

# Run the program using mpirun with OpenMP parallelism inside each MPI process
# mpirun -n $SLURM_NTASKS bin/main -n 100000 -e 100 -t 1
maqao oneview -R1 --ignore-output-code --number-processes=$SLURM_NTASKS --mpi-command="mpirun -n $SLURM_NTASKS" --number-processes-per-node=$SLURM_CPUS_PER_TASK -- bin/main -n 100000 -e 100 -t 1
