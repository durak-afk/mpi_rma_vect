MPICXX = mpic++
FLAGS = -fopenmp
BLAS_INC = -I/opt/OpenBLAS/include
BLAS_LIB = -L/opt/OpenBLAS/lib -lopenblas -Wl,-rpath,/opt/OpenBLAS/lib

all: main_blas2

fonctions.o: fonctions.cpp
	$(MPICXX) -c $< $(FLAGS)

main.o: main.cpp
	$(MPICXX) -c $< $(FLAGS) $(BLAS_INC)

main_blas2: main.o fonctions.o
	$(MPICXX) -o $@ $^ $(FLAGS) $(BLAS_LIB)

clean:
	rm -f *.o

mrproper: clean
	rm -f main_blas2