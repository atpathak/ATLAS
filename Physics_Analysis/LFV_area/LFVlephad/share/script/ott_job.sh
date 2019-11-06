#!/bin/bash
#SBATCH --mem=4000
#SBATCH --time=300
#SBATCH --partition=short

. .slurm_boilerplate

./scripts/ott.py $1 -i ${SLURM_ARRAY_TASK_ID}
