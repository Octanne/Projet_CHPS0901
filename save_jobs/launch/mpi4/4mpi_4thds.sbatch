#!/bin/bash
NTASKS=4
CPUS_PER_TASK=4

sbatch <<EOF
#!/bin/bash
#SBATCH --job-name=chps0901_${NTASKS}mpi_${CPUS_PER_TASK}thds
#SBATCH --output=results/chps0901_${NTASKS}mpi_${CPUS_PER_TASK}thds_%j.txt
#SBATCH --ntasks=$NTASKS
#SBATCH --cpus-per-task=$CPUS_PER_TASK
#SBATCH --time=04:00:00
#SBATCH --mem=64G
#SBATCH --account=M24090

spack load openmpi@4.1.6
spack load gcc@13.1.0
ulimit -s unlimited

export LD_LIBRARY_PATH="$(pwd)/libs/install/lib64:\$LD_LIBRARY_PATH"
export OMP_NUM_THREADS=$SLURM_CPUS_PER_TASK

mpirun -n \$SLURM_NTASKS bin/main -n 800000 -e 100 -t 1 -w 100000
EOF