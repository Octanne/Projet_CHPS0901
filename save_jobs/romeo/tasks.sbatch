#!/bin/bash
NTASKS=8
CPUS_PER_TASK=24
N_PARTICULES=100000
N_ITERATIONS=100

        sbatch <<EOF
#!/bin/bash
#!/bin/bash
#SBATCH --job-name=chps0901_romeo_results
#SBATCH --output=results/romeo/chps0901_romeo_%j.txt
#SBATCH --ntasks=$NTASKS
#SBATCH --cpus-per-task=$CPUS_PER_TASK
#SBATCH --time=08:00:00
#SBATCH --mem=20G
#SBATCH --account=r250059
#SBATCH --constraint=x64cpu

romeo_load_x64cpu_env
spack load openmpi@4.1.7 %aocc
ulimit -s unlimited

for NTASKS in {8..1}; do
    for CPUS_PER_TASK in {24..1}; do
        echo "========================================"
        export OMP_NUM_THREADS=\$CPUS_PER_TASK
        mpirun -n \$NTASKS bin/main -n $N_PARTICULES -e $N_ITERATIONS -t 1 -w 100000 -s false
        echo "========================================"
    done
done
EOF