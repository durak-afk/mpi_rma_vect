# Version 3 niveau 2
Commande 

Make 

Liste des lignes de commandes testé : 

OMP_NUM_THREADS=1 OPENBLAS_NUM_THREADS=4 mpirun -np 4 ./main_blas2 100 8 0 res.txt
OMP_NUM_THREADS=1 OPENBLAS_NUM_THREADS=4 mpirun -np 4 ./main_blas2 500 8 0 res.txt
OMP_NUM_THREADS=1 OPENBLAS_NUM_THREADS=4 mpirun -np 4 ./main_blas2 1000 8 0 res.txt
OMP_NUM_THREADS=1 OPENBLAS_NUM_THREADS=4 mpirun -np 4 ./main_blas2 5000 8 0 res.txt
OMP_NUM_THREADS=1 OPENBLAS_NUM_THREADS=4 mpirun -np 4 ./main_blas2 10000 8 0 res.txt

OMP_NUM_THREADS=1 OPENBLAS_NUM_THREADS=4 mpirun -np 1 ./main_blas2 5000 128 0 res.txt
OMP_NUM_THREADS=1 OPENBLAS_NUM_THREADS=4 mpirun -np 2 ./main_blas2 5000 128 0 res.txt
OMP_NUM_THREADS=1 OPENBLAS_NUM_THREADS=4 mpirun -np 4 ./main_blas2 5000 128 0 res.txt

OMP_NUM_THREADS=1 OPENBLAS_NUM_THREADS=4 mpirun -np 4 ./main_blas2 5000 4 0 res.txt
OMP_NUM_THREADS=1 OPENBLAS_NUM_THREADS=4 mpirun -np 4 ./main_blas2 5000 16 0 res.txt
OMP_NUM_THREADS=1 OPENBLAS_NUM_THREADS=4 mpirun -np 4 ./main_blas2 5000 64 0 res.txt
OMP_NUM_THREADS=1 OPENBLAS_NUM_THREADS=4 mpirun -np 4 ./main_blas2 5000 128 0 res.txt
OMP_NUM_THREADS=1 OPENBLAS_NUM_THREADS=4 mpirun -np 4 ./main_blas2 5000 256 0 res.txt

OMP_NUM_THREADS=1 OPENBLAS_NUM_THREADS=1 mpirun -np 4 ./main_blas2 1000 8 0 res.txt
OMP_NUM_THREADS=1 OPENBLAS_NUM_THREADS=2 mpirun -np 4 ./main_blas2 1000 8 0 res.txt
OMP_NUM_THREADS=1 OPENBLAS_NUM_THREADS=4 mpirun -np 4 ./main_blas2 1000 8 0 res.txt
OMP_NUM_THREADS=1 OPENBLAS_NUM_THREADS=8 mpirun -np 4 ./main_blas2 1000 8 0 res.txt
