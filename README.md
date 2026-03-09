# mpi_rma_vect


Pour lancer :

make 

Puis au choix tout va dependre des parametres:

OMP_NUM_THREADS=4 mpirun -np 4 ./main 100 8 0 res.txt
OMP_NUM_THREADS=4 mpirun -np 1 ./main 1000 8 0 res.txt
OMP_NUM_THREADS=4 mpirun -np 4 ./main 1000 4 0 res.txt
OMP_NUM_THREADS=1 mpirun -np 4 ./main 1000 8 0 res.txt