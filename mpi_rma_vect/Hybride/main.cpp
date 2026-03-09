#include <iostream>
#include <fstream>
#include <chrono>
#include <mpi.h>
#include <omp.h>
#include <cblas.h>
#include "fonctions.h"

using namespace std;

int main(int argc, char **argv) {

    int provided;
    MPI_Init_thread(&argc, &argv, MPI_THREAD_MULTIPLE, &provided);

    int pid, nprocs;
    MPI_Comm_rank(MPI_COMM_WORLD, &pid);
    MPI_Comm_size(MPI_COMM_WORLD, &nprocs);

    int n = atoi(argv[1]);
    int m = atoi(argv[2]);
    int root = atoi(argv[3]);
    string name = argv[4];

    // Petit test OpenMP
#pragma omp parallel num_threads(4)
    {
        int id = omp_get_thread_num();
#pragma omp critical
        cout << "je suis le thread " << id << " pour pid=" << pid << endl;
    }

    chrono::time_point<chrono::system_clock> debut, fin;

    double *matrice = new double[n*n];
    double *vecteurs;

    fstream f;
    if (pid == root) {
        f.open(name, ios::out);
        srand(time(NULL));

        for (int i = 0; i < n*n; i++) matrice[i] = rand() % 20;

        f << "Matrice" << endl;
        for (int i=0; i<n; i++) {
            for (int j=0; j<n; j++) f << matrice[i*n+j] << " ";
            f << endl;
        }
        f << endl;

        vecteurs = new double[m*n];
        for (int i=0; i<m; i++) {
            int nb_zero = rand() % (n/2);
            int *temp = new int[n];
            generation_vecteur(n, temp, nb_zero);
            for (int j=0; j<n; j++) vecteurs[i*n+j] = temp[j];
            delete[] temp;
        }

        f << "Les vecteurs" << endl;
        for (int i=0; i<m; i++) {
            for (int j=0; j<n; j++) f << (int)vecteurs[i*n+j] << " ";
            f << endl;
        }
    }

    // ----------------------------------------------------
    if (pid == root) debut = chrono::system_clock::now();

    MPI_Bcast(matrice, n*n, MPI_DOUBLE, root, MPI_COMM_WORLD);

    int vecteurs_par_proc = m / nprocs;
    double *vecteurs_locaux = new double[vecteurs_par_proc*n];

    MPI_Scatter(vecteurs, vecteurs_par_proc*n, MPI_DOUBLE,
                vecteurs_locaux, vecteurs_par_proc*n, MPI_DOUBLE,
                root, MPI_COMM_WORLD);

    double *resultats_locaux = new double[vecteurs_par_proc*n];

    // BLAS Niveau 2 
    for (int i=0; i<vecteurs_par_proc; i++) {
        cblas_dgemv(
            CblasRowMajor,
            CblasNoTrans,
            n, n,
            1.0,
            matrice,
            n,
            vecteurs_locaux + i*n,
            1,
            0.0,
            resultats_locaux + i*n,
            1
        );
    }

    if (pid == root) {
        fin = chrono::system_clock::now();
        chrono::duration<double> elapsed = fin - debut;
        cout << "temps en secondes (BLAS niveau 2) : " << elapsed.count() << endl;
    }

    double *resultats = nullptr;
    if (pid == root) resultats = new double[m*n];

    MPI_Gather(resultats_locaux, vecteurs_par_proc*n, MPI_DOUBLE,
               resultats, vecteurs_par_proc*n, MPI_DOUBLE,
               root, MPI_COMM_WORLD);

    if (pid == root) {
        f << "Resultats BLAS Niveau 2" << endl;
        for (int i=0; i<m; i++) {
            for (int j=0; j<n; j++) f << (int)resultats[i*n+j] << " ";
            f << endl;
        }
        f.close();
    }

    delete[] matrice;
    delete[] vecteurs_locaux;
    delete[] resultats_locaux;
    if (pid == root) {
        delete[] vecteurs;
        delete[] resultats;
    }

    MPI_Finalize();
    return 0;
}