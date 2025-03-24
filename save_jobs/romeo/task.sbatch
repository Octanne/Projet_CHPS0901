#!/bin/bash
NTASKS=2
CPUS_PER_TASK=2
N_PARTICULES=800000
N_ITERATIONS=100
        
sbatch <<EOF
#!/bin/bash
#SBATCH --job-name=chps0901_${NTASKS}mpi_${CPUS_PER_TASK}thds
#SBATCH --output=results/romeo/chps0901_${NTASKS}mpi_${CPUS_PER_TASK}thds_%j.txt
#SBATCH --ntasks=$NTASKS
#SBATCH --cpus-per-task=$CPUS_PER_TASK
#SBATCH --time=04:00:00
#SBATCH --mem=20G
#SBATCH --account=r250059
#SBATCH --constraint=x64cpu

romeo_load_x64cpu_env
spack load openmpi@4.1.7 %aocc
ulimit -s unlimited

export OMP_NUM_THREADS=\$SLURM_CPUS_PER_TASK

mpirun -n \$SLURM_NTASKS bin/main -n $N_PARTICULES -e $N_ITERATIONS -t 1 -w 100000 -s false
EOF