#!/bin/bash

sbatch --array=0-154 ./scripts/ott_job.sh $1
