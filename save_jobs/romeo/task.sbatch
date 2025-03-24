#!/bin/bash
NTASKS=1
CPUS_PER_TASK=1
N_PARTICULES=800000
N_ITERATIONS=100
        
sbatch <<EOF
#!/bin/bash
#SBATCH --job-name=chps0901_sequential
#SBATCH --output=results/romeo/chps0901_100k_and_800k_sequential_%j.txt
#SBATCH --ntasks=$NTASKS
#SBATCH --cpus-per-task=$CPUS_PER_TASK
#SBATCH --time=18:00:00
#SBATCH --mem=20G
#SBATCH --account=r250059
#SBATCH --constraint=x64cpu

romeo_load_x64cpu_env
spack load openmpi@4.1.7 %aocc
ulimit -s unlimited

export OMP_NUM_THREADS=\$SLURM_CPUS_PER_TASK

echo "========================================"
versions/sequential/bin/main -n 100000 -e $N_ITERATIONS -t 1 -w 100000
echo "========================================"
versions/sequential/bin/main -n 800000 -e $N_ITERATIONS -t 1 -w 100000
echo "========================================"
EOF