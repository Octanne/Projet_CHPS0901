#!/bin/bash
NTASKS=8
CPUS_PER_TASK=24
N_PARTICULES=100000
N_ITERATIONS=100
BIN_PATH=${1:-bin/main}
#Keep only the last folder of the path
VERS=$(basename $BIN_PATH)

sbatch <<EOF
#!/bin/bash
#!/bin/bash
#SBATCH --job-name=chps0901_romeo_results_800k
#SBATCH --output=results/romeo/chps0901_romeo_800k_${VERS}_%j.txt
#SBATCH --ntasks=$NTASKS
#SBATCH --cpus-per-task=$CPUS_PER_TASK
#SBATCH --time=012:00:00
#SBATCH --mem=20G
#SBATCH --account=r250059
#SBATCH --constraint=x64cpu

romeo_load_x64cpu_env
spack load openmpi@4.1.7 %aocc
ulimit -s unlimited

for NTASKS in {1..8}; do
    for CPUS_PER_TASK in {1..24}; do
        echo "========================================"
        export OMP_NUM_THREADS=\$CPUS_PER_TASK
        mpirun -n \$NTASKS $BIN_PATH -n $N_PARTICULES -e $N_ITERATIONS -t 1 -w 100000
        echo "========================================"
    done
done
EOF