#include <iostream>
#include <fstream>
#include <chrono>
#include <mpi.h>
#include <omp.h>
#include "fonctions.h"

using namespace std;

// Tags pour les communications
#define TAG_TRAVAIL   1  // leader envoie un vecteur à traiter
#define TAG_RESULTAT  2  // follower renvoie le résultat
#define TAG_FIN       3  // leader signale qu'il n'y a plus de travail

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
    int *resultats = nullptr;

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

        resultats = new int[m * n];
    }

    // ----------------------------------------------------

    // Diffusion de la matrice à tous les processus (nécessaire pour les calculs)
    MPI_Bcast(matrice, n * n, MPI_INT, root, MPI_COMM_WORLD);

    if (pid == root)
        debut = chrono::system_clock::now();

    // LEADER 

    if (pid == root) {

        int prochain_vecteur = 0; // index du prochain vecteur à distribuer
        int calculs_restants  = m; // nombre de résultats encore attendus

        for (int dest = 0; dest < nprocs; dest++) {
            if (dest == root) continue; // root ne se traite pas lui-même
            if (prochain_vecteur < m) {
                MPI_Send(&prochain_vecteur, 1, MPI_INT,
                         dest, TAG_TRAVAIL, MPI_COMM_WORLD);
                MPI_Send(vecteurs + prochain_vecteur * n, n, MPI_INT,
                         dest, TAG_TRAVAIL, MPI_COMM_WORLD);
                prochain_vecteur++;
            } else {

                MPI_Send(&prochain_vecteur, 1, MPI_INT,
                         dest, TAG_FIN, MPI_COMM_WORLD);
            }
        }


        while (calculs_restants > 0) {
            MPI_Status status;
            int index_resultat;


            MPI_Recv(&index_resultat, 1, MPI_INT,
                     MPI_ANY_SOURCE, TAG_RESULTAT, MPI_COMM_WORLD, &status);

            MPI_Recv(resultats + index_resultat * n, n, MPI_INT,
                     status.MPI_SOURCE, TAG_RESULTAT, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            calculs_restants--;

            if (prochain_vecteur < m) {
                MPI_Send(&prochain_vecteur, 1, MPI_INT,
                         status.MPI_SOURCE, TAG_TRAVAIL, MPI_COMM_WORLD);
                MPI_Send(vecteurs + prochain_vecteur * n, n, MPI_INT,
                         status.MPI_SOURCE, TAG_TRAVAIL, MPI_COMM_WORLD);
                prochain_vecteur++;
            } else {
                MPI_Send(&prochain_vecteur, 1, MPI_INT,
                         status.MPI_SOURCE, TAG_FIN, MPI_COMM_WORLD);
            }
        }

        fin = chrono::system_clock::now();
        chrono::duration<double> elapsed_seconds = fin - debut;
        cout << "temps en secondes : " << elapsed_seconds.count() << endl;


    // FOLLOWERS 
    } else {

        int *vecteur_local   = new int[n];
        int *resultat_local  = new int[n];

        // Les followers connaissent m 
        while (true) {
            MPI_Status status;
            int index_vecteur;

            MPI_Recv(&index_vecteur, 1, MPI_INT,
                     root, MPI_ANY_TAG, MPI_COMM_WORLD, &status);

            // Si le leader signale la fin, on arrête
            if (status.MPI_TAG == TAG_FIN)
                break;

            MPI_Recv(vecteur_local, n, MPI_INT,
                     root, TAG_TRAVAIL, MPI_COMM_WORLD, MPI_STATUS_IGNORE);


            matrice_vecteur(n, matrice, vecteur_local, resultat_local);

            MPI_Send(&index_vecteur, 1, MPI_INT,
                     root, TAG_RESULTAT, MPI_COMM_WORLD);
            MPI_Send(resultat_local, n, MPI_INT,
                     root, TAG_RESULTAT, MPI_COMM_WORLD);
        }

        delete[] vecteur_local;
        delete[] resultat_local;
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