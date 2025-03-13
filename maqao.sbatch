#!/bin/bash
#SBATCH --job-name=n_corps_chps0901    # Job name
#SBATCH --output=results/output_ncorps_%j.txt  # Output file (%j will be replaced with the job ID)
#SBATCH --ntasks=2                     # Number of MPI processes (n processes)
#SBATCH --cpus-per-task=8             # Number of CPU cores per MPI process (for OpenMP threads)
#SBATCH --time=01:00:00                # Maximum runtime (1 hour)
#SBATCH --mem=25G                      # Memory allocation
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
maqao oneview -R1 --ignore-signals=15 --set-exit-signals=15 --number-processes=$SLURM_NTASKS --mpi-command="mpirun -n $SLURM_NTASKS" --number-processes-per-node=$SLURM_CPUS_PER_NODE -- bin/main -n 100000 -e 100 -t 1

#../maqao.x86_64.2.20.13/bin/maqao oneview -R1 --number-processes=2 --mpi-command="mpirun -n 2 --bind-to core --map-by socket:PE=2 --report-bindings" --number-processes-per-node=4 -- bin/main -n 100000 -e 100 -t 1 -s false
