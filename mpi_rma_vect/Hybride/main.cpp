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

    int n    = atoi(argv[1]);
    int m    = atoi(argv[2]);
    int root = atoi(argv[3]);
    string name = argv[4];

    #pragma omp parallel num_threads(4)
    {
        int id = omp_get_thread_num();
        #pragma omp critical
        cout << "je suis le thread " << id << " pour pid=" << pid << endl;
    }

    chrono::time_point<chrono::system_clock> debut, fin;

    double *matrice  = new double[n * n];
    double *vecteurs = nullptr;

    fstream f;
    if (pid == root) {
        f.open(name, ios::out);
        srand(time(NULL));

        for (int i = 0; i < n * n; i++) matrice[i] = rand() % 20;

        f << "Matrice" << endl;
        for (int i = 0; i < n; i++) {
            for (int j = 0; j < n; j++) f << matrice[i * n + j] << " ";
            f << endl;
        }
        f << endl;

        vecteurs = new double[m * n];
        for (int i = 0; i < m; i++) {
            int nb_zero = rand() % (n / 2);
            int *temp = new int[n];
            generation_vecteur(n, temp, nb_zero);
            for (int j = 0; j < n; j++) vecteurs[i * n + j] = temp[j];
            delete[] temp;
        }

        f << "Les vecteurs" << endl;
        for (int i = 0; i < m; i++) {
            for (int j = 0; j < n; j++) f << (int)vecteurs[i * n + j] << " ";
            f << endl;
        }
    }

    // ----------------------------------------------------

    if (pid == root)
        debut = chrono::system_clock::now();

    // Diffusion de la matrice
    MPI_Bcast(matrice, n * n, MPI_DOUBLE, root, MPI_COMM_WORLD);

    int vecteurs_par_proc = m / nprocs;
    double *vecteurs_locaux = new double[vecteurs_par_proc * n];

    MPI_Scatter(vecteurs, vecteurs_par_proc * n, MPI_DOUBLE,
                vecteurs_locaux, vecteurs_par_proc * n, MPI_DOUBLE,
                root, MPI_COMM_WORLD);


    double *resultats_locaux = new double[vecteurs_par_proc * n];

    cblas_dgemm(
        CblasColMajor,
        CblasNoTrans,       // V (vpp x n) : pas de transposition côté ColMajor
        CblasNoTrans,       // matrice (n x n) : pas de transposition
        vecteurs_par_proc,  // M : lignes de C = vpp
        n,                  // N : colonnes de C = n
        n,                  // K : colonnes de A = lignes de B = n
        1.0,
        vecteurs_locaux,    // A (vpp x n en RowMajor = n x vpp en ColMajor)
        vecteurs_par_proc,  // lda = vpp
        matrice,            // B (n x n)
        n,                  // ldb = n
        0.0,
        resultats_locaux,   // C (vpp x n en RowMajor = n x vpp en ColMajor)
        vecteurs_par_proc   // ldc = vpp
    );

    if (pid == root) {
        fin = chrono::system_clock::now();
        chrono::duration<double> elapsed_seconds = fin - debut;
        cout << "temps en secondes : " << elapsed_seconds.count() << endl;
    }

    // ----------------------------------------------------

    double *resultats = nullptr;
    if (pid == root) resultats = new double[m * n];

    MPI_Gather(resultats_locaux, vecteurs_par_proc * n, MPI_DOUBLE,
               resultats, vecteurs_par_proc * n, MPI_DOUBLE,
               root, MPI_COMM_WORLD);

    if (pid == root) {
        f << "Les vecteurs" << endl;
        for (int i = 0; i < m; i++) {
            for (int j = 0; j < n; j++) f << (int)resultats[i * n + j] << " ";
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