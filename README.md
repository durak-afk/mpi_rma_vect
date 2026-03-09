# Version 2

Pour lancer :

make 

Et tous les test que j'ai faites 

OMP_NUM_THREADS=4 mpirun -np 4 ./main 100 8 0 res.txt
OMP_NUM_THREADS=4 mpirun -np 4 ./main 500 8 0 res.txt
OMP_NUM_THREADS=4 mpirun -np 4 ./main 1000 8 0 res.txt
OMP_NUM_THREADS=4 mpirun -np 4 ./main 5000 8 0 res.txt
OMP_NUM_THREADS=4 mpirun -np 4 ./main 10000 8 0 res.txt


OMP_NUM_THREADS=4 mpirun -np 2 ./main 5000 128 0 res.txt
OMP_NUM_THREADS=4 mpirun -np 4 ./main 5000 128 0 res.txt

OMP_NUM_THREADS=4 mpirun -np 4 ./main 5000 4 0 res.txt
OMP_NUM_THREADS=4 mpirun -np 4 ./main 5000 16 0 res.txt
OMP_NUM_THREADS=4 mpirun -np 4 ./main 5000 64 0 res.txt
OMP_NUM_THREADS=4 mpirun -np 4 ./main 5000 128 0 res.txt
OMP_NUM_THREADS=4 mpirun -np 4 ./main 5000 256 0 res.txt

OMP_NUM_THREADS=1 mpirun -np 4 ./main 1000 8 0 res.txt
OMP_NUM_THREADS=2 mpirun -np 4 ./main 1000 8 0 res.txt
OMP_NUM_THREADS=4 mpirun -np 4 ./main 1000 8 0 res.txt
OMP_NUM_THREADS=8 mpirun -np 4 ./main 1000 8 0 res.txt
