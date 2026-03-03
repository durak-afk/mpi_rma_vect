#include <iostream>
#include <fstream>
#include <chrono>
#include <mpi.h>
#include <omp.h>
#include "fonctions.h"

using namespace std;

int main(int argc, char **argv) {

    // Pour initialiser l'environnement MPI avec la possibilité d'utiliser des threads (OpenMP)
    int provided;
    MPI_Init_thread(&argc, &argv, MPI_THREAD_MULTIPLE, &provided);

    // Pour connaître son pid et le nombre de processus de l'exécution parallèle (sans les threads)
    int pid, nprocs;
    MPI_Comm_rank(MPI_COMM_WORLD, &pid);
    MPI_Comm_size(MPI_COMM_WORLD, &nprocs);

    int n    = atoi(argv[1]); // taille de la matrice carrée
    int m    = atoi(argv[2]); // nombre de vecteurs en entrée
    int root = atoi(argv[3]); // processeur root : référence pour les données
    string name = argv[4];    // le nom du fichier pour sauvegarder les données

    // Petit test pour vérifier qu'on peut avoir plusieurs threads par processus.
    #pragma omp parallel num_threads(4)
    {
        int id = omp_get_thread_num();
        #pragma omp critical
        cout << "je suis le thread " << id << " pour pid=" << pid << endl;
    }

    // Pour mesurer le temps (géré par le processus root)
    chrono::time_point<chrono::system_clock> debut, fin;

    int *matrice  = new int[n * n];
    int *vecteurs = nullptr;

    fstream f;
    if (pid == root) {
        f.open(name, std::fstream::out);
        srand(time(NULL));
        for (int i = 0; i < n * n; i++)
            matrice[i] = rand() % 20;

        f << "Matrice" << endl;
        for (int i = 0; i < n; i++) {
            for (int j = 0; j < n; j++)
                f << matrice[i * n + j] << " ";
            f << endl;
        }
        f << endl;

        vecteurs = new int[m * n];
        for (int i = 0; i < m; i++) {
            int nb_zero = rand() % (n / 2);
            generation_vecteur(n, vecteurs + i * n, nb_zero);
        }

        f << "Les vecteurs" << endl;
        for (int i = 0; i < m; i++) {
            for (int j = 0; j < n; j++)
                f << vecteurs[i * n + j] << " ";
            f << endl;
        }
    }

    // ----------------------------------------------------
    // --- RMA 
    
    if (pid == root)
        debut = chrono::system_clock::now();

    MPI_Win win_matrice;
    MPI_Win_create(matrice, n * n * sizeof(int), sizeof(int),
                   MPI_INFO_NULL, MPI_COMM_WORLD, &win_matrice);

    MPI_Win_fence(0, win_matrice);
    if (pid != root) {
        MPI_Get(matrice, n * n, MPI_INT,
                root, 0, n * n, MPI_INT, win_matrice);
    }
    MPI_Win_fence(0, win_matrice);
    MPI_Win_free(&win_matrice);



    int vecteurs_par_proc = m / nprocs;
    int *vecteurs_locaux  = new int[vecteurs_par_proc * n];

    MPI_Win win_vecteurs;
    MPI_Win_create(vecteurs, (pid == root ? m * n : 0) * sizeof(int), sizeof(int),
                   MPI_INFO_NULL, MPI_COMM_WORLD, &win_vecteurs);

    MPI_Win_fence(0, win_vecteurs);
    MPI_Get(vecteurs_locaux, vecteurs_par_proc * n, MPI_INT,
            root, pid * vecteurs_par_proc * n,
            vecteurs_par_proc * n, MPI_INT, win_vecteurs);
    MPI_Win_fence(0, win_vecteurs);
    MPI_Win_free(&win_vecteurs);

    // Calcul local des produits matrice-vecteur (OpenMP dans matrice_vecteur)
    int *resultats_locaux = new int[vecteurs_par_proc * n];
    for (int i = 0; i < vecteurs_par_proc; i++) {
        matrice_vecteur(n, matrice, vecteurs_locaux + i * n, resultats_locaux + i * n);
    }

    int *resultats = nullptr;
    if (pid == root)
        resultats = new int[m * n];

    MPI_Win win_resultats;
    MPI_Win_create(resultats, (pid == root ? m * n : 0) * sizeof(int), sizeof(int),
                   MPI_INFO_NULL, MPI_COMM_WORLD, &win_resultats);

    MPI_Win_fence(0, win_resultats);
    MPI_Put(resultats_locaux, vecteurs_par_proc * n, MPI_INT,
            root, pid * vecteurs_par_proc * n,
            vecteurs_par_proc * n, MPI_INT, win_resultats);
    MPI_Win_fence(0, win_resultats);
    MPI_Win_free(&win_resultats);

    // Dans le temps écoulé on ne s'occupe que de la partie communications et calculs
    // (on oublie la génération des données et l'écriture des résultats sur le fichier de sortie)
    if (pid == root) {
        fin = chrono::system_clock::now();
        chrono::duration<double> elapsed_seconds = fin - debut;
        cout << "temps en secondes : " << elapsed_seconds.count() << endl;
    }

    // ----------------------------------------------------

    if (pid == root) {
        f << "Les résultats" << endl;
        for (int i = 0; i < m; i++) {
            for (int j = 0; j < n; j++)
                f << resultats[i * n + j] << " ";
            f << endl;
        }
        f.close();
    }

    MPI_Finalize();
    return 0;
}