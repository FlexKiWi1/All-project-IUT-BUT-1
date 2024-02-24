/**
 *
 * @file RESOLUTION_2.c
 * @author Loris Caruhel - Enzo Vivion-Michaud
 * @brief Programme qui permet de résoudre des grilles de sudoku automatiquement.
 * @version 2.0
 * @date 28/01/2024
 * 
 * @copyright Copyright (c) 2023
 * 
 * 
 * 
 * /////////////////////////////////////////////////
 * ///////////  AMELIORATIONS APPORTÉES  ///////////
 * /////////////////////////////////////////////////
 * 
 * 1. Le backtracking parcours les candidats de chaque case au lieu de parcourir toutes les valeurs à chaque fois.
 * 
 * 2. Pour retirer un candidat, la fontion "rebouche le troue" en remplaçant juste par la dernière valeur du t_candidat de la case.
 * 
 * 3. Pour estCandidat, au lieu de parcourir tout le tableau jusqu'à TAILLE, la fontion fait une recherche séquentielle (pas forcément besoin de changer
 *    de technique car ici ce sont sur des petits tableaux non trié).
 * 
 * 4. Agencement des différents appel de fonction de résolution dans la main pour gagner du temps.
 * 
 * 5. Quelques boucle for transformée en en while. (notamment dans les fonctions absentSur...)
 * 
 * 6. Rajout de quelques conditions dans les fonctions de résolution pour que les fonctions aille que des les cases ou il y a besoin (valeur = 0, nbCandidat > 1 
 *    pour les singletons caché par exemple).
 * 
 * 7. Dans quelques fonctions notamment dans initCand, il y a une fusion entre le parcours des lignes et colonnes pour enlever une boucle for et optimiser du temps.
 * 
 * 8. Pour les singletons cachées ligne par exemple (pareil pour les 2 autres) j'enlevais tous les candidats dans la ligne, colonne et bloc mais le singleton est seul
 *    dans sa ligne, colonne ou bloc pas besoin de l'enlever en candidat dans sa propre ligne, colonne ou bloc puisque il est déjà le seul.
 * 
 */


#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <unistd.h>
#include <time.h>


// Définition de la taille de la grille Sudoku.
#define N 4
#define TAILLE (N * N)

// Structure représentant une case Sudoku avec sa valeur, ses candidats et son nombre de candidats.
typedef struct
{
    int valeur;
    int t_candidats[TAILLE + 1];
    int nbCandidats;
} tCase1;

// Définition du type tGrille qui est une grille Sudoku de cases tCase2.
typedef tCase1 tGrille[TAILLE][TAILLE];

// Coordonné temporaire pour stocké ou se trouve un singleton caché sur une ligne par exemple.
typedef struct 
{
    int CoorTempLig;
    int CoorTempCol;
}CoorTemp;
typedef CoorTemp t_CoorTemp[TAILLE+1];


// Constantes pour l'affichage de la grille.
const char LIGNE_SEPARATION[70] = "+-------------+------------+------------+------------+";
const char CARAC_POINT[2] = ".";
const char CARAC_PIPE[2] = "|";
const char ESPACE[2] = " ";

// Prototypes des fonctions.
void chargerGrille(tGrille g, char nomFichier[]);
void initCand(tGrille grilleCand);

void affichageGrille(tGrille grilleAffichage);

void retirerCandLig(tGrille grilleCand, int lig, int valeur);
void retirerCandCol(tGrille grilleCand, int col, int valeur);
void retirerCandCase(tGrille grilleCand, int lig, int col, int valeur);

void ajouterCandidat(tCase1 *laCase, int val);
void retirerCandidat(tCase1 *laCase, int val);
bool estCandidat(tCase1 laCase, int val);

int nbOccuValParBloc(tGrille grilleCand, int lig, int col, int val);
int nbOccuValParLigne(tGrille grilleCand, int lig, int val);
int nbOccuValParColonne(tGrille grilleCand, int col, int val);

// Prototype des fonctions de technique de résolution.
void singletonNu(tGrille grilleCand, bool *progression);

void singletonCacheBloc(tGrille grilleCand, bool *progression);
void singletonCacheLigne(tGrille grilleCand, bool *progression);
void singletonCacheColonne(tGrille grilleCand, bool *progression);

void paireCacheeBloc(tGrille grilleCand);
void paireCacheeColonne(tGrille grilleCand);
void paireCacheeLigne(tGrille grilleCand);

void paireNueBloc(tGrille grilleCand);
void paireNueLigne(tGrille grilleCand);
void paireNueColonne(tGrille grilleCand);

bool backtracking(tGrille grille, int numeroCase);
bool absentSurLigne(int valeur, tGrille grille, int ligne);
bool absentSurColonne(int valeur, tGrille grille, int colonne);
bool absentSurBloc(int valeur, tGrille grille, int ligne, int colonne);

int main()
{    
    // Initialisation des variables.
    tGrille grilleCand;
    char nomFichier[20];    
    int numeroCase = 0;
    bool progression = true;

    system("clear");
    printf("Nom du fichier ? (MaxiGrille[A-D].sud)\n");
    scanf("%s", nomFichier);

    // Affichage de la grille chargée.
    system("clear");
    chargerGrille(grilleCand, nomFichier);
    printf("Grille chargée avec succès !\n");
    sleep(1);
    system("clear");

    // Affichage de la grille initiale.
    printf("Grille initiale :\n");
    affichageGrille(grilleCand);

    clock_t begin = clock();

    initCand(grilleCand);

    // *** Résolution ***
    paireCacheeBloc(grilleCand);
    paireCacheeLigne(grilleCand);
    paireCacheeColonne(grilleCand);

    paireNueBloc(grilleCand);
    paireNueLigne(grilleCand);
    paireNueColonne(grilleCand);

    while(progression)
    {
        progression = false;

        singletonCacheBloc(grilleCand, &progression);
        singletonCacheLigne(grilleCand, &progression);
        singletonCacheColonne(grilleCand, &progression);

        singletonNu(grilleCand, &progression);
    }

    backtracking(grilleCand, numeroCase);

    clock_t end = clock();

    // Affichage de la grille finale et des statistiques.
    printf("\nGrille finale :\n");
    affichageGrille(grilleCand);

    double tmpsCPU = (end - begin)*1.0 / CLOCKS_PER_SEC;
    printf("Résolution de la grille en %f secondes\n",tmpsCPU);
}

bool backtracking(tGrille grille, int numeroCase)
{
    int ligne, colonne;
    bool resultat = false;
    int i;

    if(numeroCase == TAILLE * TAILLE)
    {
        resultat = true;
    }
    else
    {
        ligne = numeroCase / TAILLE;
        colonne = numeroCase % TAILLE;

        if(grille[ligne][colonne].valeur != 0)
        {
            resultat = backtracking(grille, numeroCase + 1);
        }
        else
        {
            i = 0;
            while(i < grille[ligne][colonne].nbCandidats && !resultat)
            {
                int candidatCourant = grille[ligne][colonne].t_candidats[i];

                if(absentSurLigne(candidatCourant, grille, ligne) &&
                   absentSurColonne(candidatCourant, grille, colonne) &&
                   absentSurBloc(candidatCourant, grille, ligne, colonne))
                {
                    grille[ligne][colonne].valeur = candidatCourant;

                    if (backtracking(grille, numeroCase + 1)){
                        resultat = true;
                    } else
                    {
                        grille[ligne][colonne].valeur = 0;
                    }
                }
                i++;
            }
        }
    }

    return resultat;
}

bool absentSurLigne(int valeur, tGrille grille, int ligne)
{
    bool absent = true;
    int col = 0;

    while(col < TAILLE && absent)
    {
        if(grille[ligne][col].valeur  == valeur)
        {
            absent = false;
        }
        col++;
    }

    return absent;
}

bool absentSurColonne(int valeur, tGrille grille, int colonne)
{
    bool absent = true;
    int lig = 0;

    while(lig < TAILLE && absent)
    {
        if(grille[lig][colonne].valeur  == valeur)
        {
            absent = false;
        }
        lig++;
    }

    return absent;
}

bool absentSurBloc(int valeur, tGrille grille, int ligne, int colonne)
{
    bool absent = true;

    int LigCase = (ligne / N) * N;
    int ColCase = (colonne / N) * N;

    int iCaseLig = LigCase;
    int iCaseCol = ColCase;

    // Parcours des blocs et mise à false des valeurs déjà présentes.
    while(iCaseLig < LigCase + N && absent)
    {
        iCaseCol = ColCase;

        while(iCaseCol < ColCase + N && absent)
        {
            if (grille[iCaseLig][iCaseCol].valeur == valeur)
            {
                absent = false;
            }
            iCaseCol++;
        }
        iCaseLig++;
    }

    return absent;
}

/* ------- LES PAIRES CACHÉES ------- */

// Fonction de résolution par la paire cachée de toutes les colonnes.
void paireCacheeColonne(tGrille grilleCand)
{
    // Variables de stockage du nombre d'occurrences pour chaque candidat.
    int nbOccuCand = 0;
    int t_nbOccuAllCand[TAILLE + 1] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

    int coord1[2] = {-1, -1};
    int coord2[2] = {-1, -1};

    for (int colParcours = 0; colParcours < TAILLE; colParcours++)
    {
        // Calcul du nombre d'occurrences pour chaque candidat dans la colonne.
        for (int i = 1; i <= TAILLE; i++)
        {
            t_nbOccuAllCand[i] = nbOccuValParColonne(grilleCand, colParcours, i);
        }

        // Parcours des paires de valeurs possibles.
        for (int val1 = 1; val1 <= TAILLE; val1++)
        {
            for (int val2 = 1; val2 <= TAILLE; val2++)
            {
                if (val2 != val1)
                {
                    nbOccuCand = 0;
                    coord1[0] = -1;
                    coord1[1] = -1;
                    coord2[0] = -1;
                    coord2[1] = -1;

                    // Parcours de la colonne pour trouver les coordonnées des cases avec la paire de valeurs.
                    for (int ligParcours = 0; ligParcours < TAILLE; ligParcours++)
                    {
                        if (grilleCand[ligParcours][colParcours].valeur == 0 &&
                            grilleCand[ligParcours][colParcours].nbCandidats > 2 &&
                            (estCandidat(grilleCand[ligParcours][colParcours], val1) &&
                            estCandidat(grilleCand[ligParcours][colParcours], val2)))
                        {
                            if (coord1[0] == -1)
                            {
                                // Première paire tourvée.
                                coord1[0] = ligParcours;
                                coord1[1] = colParcours;
                            }
                            else
                            {
                                // Deuxième paire trouvée.
                                coord2[0] = ligParcours;
                                coord2[1] = colParcours;

                                nbOccuCand += 2; // += permet de détecter s'il y en a plus de 2 cases avec la paire et donc de ne pas retirer les candidats par la suite.
                            }
                        }
                    }

                    // Si deux cases ont été trouvées avec la paire de valeurs, retirer tous les candidats des deux cases de la paire sauf les 2 valeurs.
                    if (nbOccuCand == 2 &&
                    grilleCand[coord1[0]][coord1[1]].nbCandidats > 2 &&
                    grilleCand[coord2[0]][coord2[1]].nbCandidats > 2 &&
                    (t_nbOccuAllCand[val1] == 2 && t_nbOccuAllCand[val2] == 2))
                    {
                        for (int i = 1; i <= TAILLE; i++)
                        {
                            if (i != val1 && i != val2)
                            {
                                retirerCandidat(&grilleCand[coord1[0]][coord1[1]], i);
                                retirerCandidat(&grilleCand[coord2[0]][coord2[1]], i);
                            }
                        }
                    }
                }
            }
        }
    }
}

// Fonction de résolution par la paire cachée de toutes les lignes.
void paireCacheeLigne(tGrille grilleCand)
{
    // Variables de stockage du nombre d'occurrences pour chaque candidat.
    int nbOccuCand = 0;
    int t_nbOccuAllCand[TAILLE + 1] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

    int coord1[2] = {-1, -1};
    int coord2[2] = {-1, -1};

    for (int ligParcours = 0; ligParcours < TAILLE; ligParcours++)
    {
        // Calcul du nombre d'occurrences pour chaque candidat dans la ligne.
        for (int i = 1; i <= TAILLE; i++)
        {
            t_nbOccuAllCand[i] = nbOccuValParLigne(grilleCand, ligParcours, i);
        }

        // Parcours des paires de valeurs possibles.
        for (int val1 = 1; val1 <= TAILLE; val1++)
        {
            for (int val2 = 1; val2 <= TAILLE; val2++)
            {
                if (val2 != val1)
                {
                    nbOccuCand = 0;
                    coord1[0] = -1;
                    coord1[1] = -1;
                    coord2[0] = -1;
                    coord2[1] = -1;

                    // Parcours de la ligne pour trouver les coordonnées des cases avec la paire de valeurs.
                    for (int colParcours = 0; colParcours < TAILLE; colParcours++)
                    {
                        if (grilleCand[ligParcours][colParcours].valeur == 0 &&
                            grilleCand[ligParcours][colParcours].nbCandidats > 2 &&
                            (estCandidat(grilleCand[ligParcours][colParcours], val1) &&
                            estCandidat(grilleCand[ligParcours][colParcours], val2)))
                        {
                            if (coord1[0] == -1)
                            {
                                // Première paire trouvée.
                                coord1[0] = ligParcours;
                                coord1[1] = colParcours;
                            }
                            else
                            {
                                // Deuxième paire trouvée.
                                coord2[0] = ligParcours;
                                coord2[1] = colParcours;

                                nbOccuCand += 2; // += permet de détecter s'il y en a plus de 2 cases avec la paire et donc de ne pas retirer les candidats par la suite.
                            }
                        }
                    }

                    // Si deux cases ont été trouvées avec la paire de valeurs, retirer tous les candidats des deux cases de la paire sauf les 2 valeurs.
                    if (nbOccuCand == 2 && 
                    grilleCand[coord1[0]][coord1[1]].nbCandidats > 2 &&
                    grilleCand[coord2[0]][coord2[1]].nbCandidats > 2 &&
                    (t_nbOccuAllCand[val1] == 2 && t_nbOccuAllCand[val2] == 2))
                    {
                        for (int i = 1; i <= TAILLE; i++)
                        {
                            if (i != val1 && i != val2)
                            {
                                retirerCandidat(&grilleCand[coord1[0]][coord1[1]], i);
                                retirerCandidat(&grilleCand[coord2[0]][coord2[1]], i);
                            }
                        }
                    }
                }
            }
        }
    }
}

// Fonction de résolution par paire cachée dans tous les blocs.
void paireCacheeBloc(tGrille grilleCand)
{
    // Variables de stockage du nombre d'occurrences pour chaque candidat.
    int nbOccuCand = 0;
    int t_nbOccuAllCand[TAILLE + 1] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

    // Parcours des blocs dans la grille.
    for (int ligBloc = 0; ligBloc < TAILLE; ligBloc += N)
    {
        for (int colBloc = 0; colBloc < TAILLE; colBloc += N)
        {
            // Calcul du nombre d'occurrences pour chaque candidat dans le bloc.
            for (int i = 1; i <= TAILLE; i++)
            {
                t_nbOccuAllCand[i] = nbOccuValParBloc(grilleCand, ligBloc, colBloc, i);
            }

            // Parcours des paires de valeurs possibles.
            for (int val1 = 1; val1 <= TAILLE; val1++)
            {
                for (int val2 = 1; val2 <= TAILLE; val2++)
                {
                    if (val2 != val1)
                    {
                        nbOccuCand = 0;
                        int coord1[2] = {-1, -1};
                        int coord2[2] = {-1, -1};

                        // Parcours du bloc pour trouver les coordonnées des cases avec la paire de valeurs.
                        for (int iBlocLig = ligBloc; iBlocLig < ligBloc + N; iBlocLig++)
                        {
                            for (int iBlocCol = colBloc; iBlocCol < colBloc + N; iBlocCol++)
                            {
                                if (grilleCand[iBlocLig][iBlocCol].valeur == 0 &&
                                    grilleCand[iBlocLig][iBlocCol].nbCandidats > 2 &&
                                    (estCandidat(grilleCand[iBlocLig][iBlocCol], val1) &&
                                     estCandidat(grilleCand[iBlocLig][iBlocCol], val2)))
                                {
                                    if (coord1[0] == -1)
                                    {
                                        // Première paire trouvée.
                                        coord1[0] = iBlocLig;
                                        coord1[1] = iBlocCol;
                                    }
                                    else
                                    {
                                        // deuxième paire trouvée.
                                        coord2[0] = iBlocLig;
                                        coord2[1] = iBlocCol;

                                        nbOccuCand += 2; // += permet de détecter s'il y en a plus de 2 cases avec la paire et donc de ne pas retirer les candidats par la suite.
                                    }
                                }
                            }
                        }

                        // Si deux cases ont été trouvées avec la paire de valeurs, retirer tous les candidats des deux cases de la paire sauf les 2 valeurs.
                        if (nbOccuCand == 2 && 
                        grilleCand[coord1[0]][coord1[1]].nbCandidats > 2 &&
                        grilleCand[coord2[0]][coord2[1]].nbCandidats > 2 &&
                        (t_nbOccuAllCand[val1] == 2 && t_nbOccuAllCand[val2] == 2))
                        {
                            for (int i = 1; i <= TAILLE; i++)
                            {
                                if (i != val1 && i != val2)
                                {
                                    retirerCandidat(&grilleCand[coord1[0]][coord1[1]], i);
                                    retirerCandidat(&grilleCand[coord2[0]][coord2[1]], i);
                                }
                            }
                        }
                    }
                }
            }
        }
    }
}


/* ------- LES PAIRES NUES ------- */

// Fonction de résolution par paire nue sur les colonnes.
void paireNueColonne(tGrille grilleCand)
{
    // Variables de stockage des valeurs et coordonnées des paires.
    int val1 = -1, val2 = -1;
    int nbOccuCand;
    int colTemp = -1;
    int coord1[2] = {-1, -1};
    int coord2[2] = {-1, -1};

    // Parcours des colonnes de la grille.
    for (int col = 0; col < TAILLE; col++)
    {
        nbOccuCand = 0;
        val1 = -1;
        val2 = -1;

        coord1[0] = -1;
        coord1[1] = -1;
        coord2[0] = -1;
        coord2[1] = -1;

        // Parcours des lignes de la colonne.
        for (int lig = 0; lig < TAILLE; lig++)
        {
            // Recherche de paires de candidats dans la colonne.
            if (grilleCand[lig][col].valeur == 0 && grilleCand[lig][col].nbCandidats == 2)
            {
                if (coord1[0] == -1)
                {
                    // Première paire trouvée.
                    coord1[0] = lig;
                    coord1[1] = col;

                    for (int i = 1; i <= TAILLE; i++)
                    {
                        if (estCandidat(grilleCand[lig][col], i) && val1 == -1)
                        {
                            val1 = i;
                        }
                        else if (estCandidat(grilleCand[lig][col], i))
                        {
                            val2 = i;
                        }
                    }
                }
                else if ((grilleCand[lig][col].valeur == 0 && grilleCand[lig][col].nbCandidats == 2) &&
                         (estCandidat(grilleCand[lig][col], val1) && estCandidat(grilleCand[lig][col], val2)))
                {
                    // Deuxième paire trouvée.
                    coord2[0] = lig;
                    coord2[1] = col;
                    colTemp = col;

                    for (int i = 1; i <= TAILLE; i++)
                    {
                        if (estCandidat(grilleCand[lig][col], i) && val1 == -1)
                        {
                            val1 = i;
                        }
                        else if (estCandidat(grilleCand[lig][col], i))
                        {
                            val2 = i;
                        }
                    }
                    nbOccuCand += 2;
                }
            }
        }

        // Si deux cases ont été trouvées avec la paire de valeurs, retirer ces candidats du reste de la colonne.
        if (nbOccuCand == 2)
        {
            for (int lig = 0; lig < TAILLE; lig++)
            {
                if (!((lig == coord1[0] && colTemp == coord1[1]) || (lig == coord2[0] && colTemp == coord2[1])) &&
                    grilleCand[lig][colTemp].valeur == 0)
                {
                    retirerCandidat(&grilleCand[lig][colTemp], val1);
                    retirerCandidat(&grilleCand[lig][colTemp], val2);
                }
            }
        }
    }
}

// Fonction de résolution par paire nue sur les lignes.
void paireNueLigne(tGrille grilleCand)
{
    // Variables de stockage des valeurs et coordonnées des paires.
    int val1 = -1, val2 = -1;
    int nbOccuCand;
    int ligTemp = -1;
    int coord1[2] = {-1, -1};
    int coord2[2] = {-1, -1};

    // Parcours des lignes de la grille.
    for (int lig = 0; lig < TAILLE; lig++)
    {
        nbOccuCand = 0;
        val1 = -1;
        val2 = -1;

        coord1[0] = -1;
        coord1[1] = -1;
        coord2[0] = -1;
        coord2[1] = -1;

        // Parcours des colonnes de la ligne.
        for (int col = 0; col < TAILLE; col++)
        {
            // Recherche de paires de candidats dans la ligne.
            if (grilleCand[lig][col].valeur == 0 && grilleCand[lig][col].nbCandidats == 2)
            {
                if (coord1[0] == -1)
                {
                    // Première paire trouvée.
                    coord1[0] = lig;
                    coord1[1] = col;

                    for (int i = 1; i <= TAILLE; i++)
                    {
                        if (estCandidat(grilleCand[lig][col], i) && val1 == -1)
                        {
                            val1 = i;
                        }
                        else if (estCandidat(grilleCand[lig][col], i))
                        {
                            val2 = i;
                        }
                    }
                }
                else if ((grilleCand[lig][col].valeur == 0 && grilleCand[lig][col].nbCandidats == 2) &&
                         (estCandidat(grilleCand[lig][col], val1) && estCandidat(grilleCand[lig][col], val2)))
                {
                    // Deuxième paire trouvée.
                    coord2[0] = lig;
                    coord2[1] = col;
                    ligTemp = lig;

                    for (int i = 1; i <= TAILLE; i++)
                    {
                        if (estCandidat(grilleCand[lig][col], i) && val1 == -1)
                        {
                            val1 = i;
                        }
                        else if (estCandidat(grilleCand[lig][col], i)) 

                        {
                            val2 = i;
                        }
                    }
                    nbOccuCand += 2;
                }
            }
        }

        // Si deux cases ont été trouvées avec la paire de valeurs, retirer ces candidats du reste de la ligne.
        if (nbOccuCand == 2)
        {
            for (int col = 0; col < TAILLE; col++)
            {
                if (!((ligTemp == coord1[0] && col == coord1[1]) || (ligTemp == coord2[0] && col == coord2[1])) &&
                    grilleCand[ligTemp][col].valeur == 0)
                {
                    retirerCandidat(&grilleCand[ligTemp][col], val1);
                    retirerCandidat(&grilleCand[ligTemp][col], val2);
                }
            }
        }
    }
}

// Fonction de résolution par paire nue dans un bloc.
void paireNueBloc(tGrille grilleCand)
{
    // Variables de stockage des valeurs de la paire.
    int val1 = -1, val2 = -1;

    // Parcours de la grille bloc par bloc.
    for (int lig = 0; lig < TAILLE; lig += N)
    {
        for (int col = 0; col < TAILLE; col += N)
        {
            int nbOccuCand = 0;
            val1 = -1;
            val2 = -1;

            int coord1[2] = {-1, -1};
            int coord2[2] = {-1, -1};

            // Parcours des cases de ce bloc.
            for (int iBlocLig = lig; iBlocLig < lig + N; iBlocLig++)
            {
                for (int iBlocCol = col; iBlocCol < col + N; iBlocCol++)
                {
                    if (grilleCand[iBlocLig][iBlocCol].valeur == 0 && grilleCand[iBlocLig][iBlocCol].nbCandidats == 2)
                    {
                        if (coord1[0] == -1)
                        {
                            // Première paire trouvée.
                            coord1[0] = iBlocLig;
                            coord1[1] = iBlocCol;

                            for (int i = 1; i <= TAILLE; i++)
                            {
                                if (estCandidat(grilleCand[iBlocLig][iBlocCol], i) && val1 == -1)
                                {
                                    val1 = i;
                                }
                                else if (estCandidat(grilleCand[iBlocLig][iBlocCol], i))
                                {
                                    val2 = i;
                                }
                            }
                        }
                        else if (estCandidat(grilleCand[iBlocLig][iBlocCol], val1) && estCandidat(grilleCand[iBlocLig][iBlocCol], val2))
                        {
                            // Deuxième paire trouvée.
                            coord2[0] = iBlocLig;
                            coord2[1] = iBlocCol;

                            for (int i = 1; i <= TAILLE; i++)
                            {
                                if (estCandidat(grilleCand[iBlocLig][iBlocCol], i) && val1 == -1)
                                {
                                    val1 = i;
                                }
                                else if (estCandidat(grilleCand[iBlocLig][iBlocCol], i))
                                {
                                    val2 = i;
                                }
                            }

                            nbOccuCand = 2;
                        }
                    }
                }
            }

            // Si deux cases ont été trouvées avec la paire de valeurs, retirer ces candidats du reste du bloc.
            if (nbOccuCand == 2)
            {
                for (int iBlocLig = lig; iBlocLig < lig + N; iBlocLig++)
                {
                    for (int iBlocCol = col; iBlocCol < col + N; iBlocCol++)
                    {
                        if (!((iBlocLig == coord1[0] && iBlocCol == coord1[1]) || (iBlocLig == coord2[0] && iBlocCol == coord2[1])) &&
                            grilleCand[iBlocLig][iBlocCol].valeur == 0)
                        {
                            retirerCandidat(&grilleCand[iBlocLig][iBlocCol], val1);
                            retirerCandidat(&grilleCand[iBlocLig][iBlocCol], val2);
                        }
                    }
                }
            }
        }
    }
}


/* ------- LES SINGLETONS CACHÉS ------- */

// Fonction de résolution par singleton caché sur les colonnes.
void singletonCacheColonne(tGrille grilleCand, bool *progression)
{
    int nbOccu = 0, CoorTempLig, CoorTempCol;

    for (int col = 0; col < TAILLE; col++)
    {
        // Parcours chaque bloc 9 fois et compte le nombre d'occurrences de chaque valeur de 1 à 9.
        for (int i = 1; i <= TAILLE; i++)
        {
            nbOccu = 0;
            CoorTempLig = -1;
            CoorTempCol = -1;

            for (int lig = 0; lig < TAILLE; lig++)
            {
                if (grilleCand[lig][col].valeur == 0 && estCandidat(grilleCand[lig][col], i))
                {
                    nbOccu++;
                    CoorTempLig = lig;
                    CoorTempCol = col;
                }
            }

            // Si le nombre d'occurrence de la valeur i est égal à 1, affecter la valeur.
            if (nbOccu == 1 && grilleCand[CoorTempLig][CoorTempCol].nbCandidats > 1)
            {
                grilleCand[CoorTempLig][CoorTempCol].valeur = i;

                retirerCandLig(grilleCand, CoorTempLig, i);
                retirerCandCase(grilleCand, CoorTempLig, CoorTempCol, i);

                *progression = true;
            }
        }
    }
}

// Fonction de résolution par singleton caché sur les lignes.
void singletonCacheLigne(tGrille grilleCand, bool *progression)
{
    int nbOccu = 0, CoorTempLig, CoorTempCol;

    for (int lig = 0; lig < TAILLE; lig++)
    {
        // Parcours chaque bloc 9 fois et compte le nombre d'occurrences de chaque valeur de 1 à 9.
        for (int i = 1; i <= TAILLE; i++)
        {
            nbOccu = 0;
            CoorTempLig = -1;
            CoorTempCol = -1;

            for (int col = 0; col < TAILLE; col++)
            {
                if (grilleCand[lig][col].valeur == 0 && estCandidat(grilleCand[lig][col], i))
                {
                    nbOccu++;
                    CoorTempLig = lig;
                    CoorTempCol = col;
                }
            }

            // Si le nombre d'occurrence de la valeur i est égal à 1, affecter la valeur.
            if (nbOccu == 1 && grilleCand[CoorTempLig][CoorTempCol].nbCandidats > 1)
            {
                grilleCand[CoorTempLig][CoorTempCol].valeur = i;

                retirerCandCol(grilleCand, CoorTempCol, i);
                retirerCandCase(grilleCand, CoorTempLig, CoorTempCol, i);

                *progression = true;
            }
        }
    }
}

// Fonction de résolution par singleton caché sur les blocs.
void singletonCacheBloc(tGrille grilleCand, bool *progression)
{
    int nbOccu = 0, CoorTempLig, CoorTempCol;

    // Parcours de tous les blocs de la grille.
    for (int lig = 0; lig < TAILLE; lig += N)
    {
        for (int col = 0; col < TAILLE; col += N)
        {
            // Parcours chaque bloc 9 fois et compte le nombre d'occurrences de chaque valeur de 1 à 9.
            for (int i = 1; i <= TAILLE; i++)
            {
                nbOccu = 0;
                CoorTempLig = -1;
                CoorTempCol = -1;

                for (int iBlocLig = lig; iBlocLig < lig + N; iBlocLig++)
                {
                    for (int iBlocCol = col; iBlocCol < col + N; iBlocCol++)
                    {
                        if (grilleCand[iBlocLig][iBlocCol].valeur == 0 && estCandidat(grilleCand[iBlocLig][iBlocCol], i))
                        {
                            nbOccu++;
                            CoorTempLig = iBlocLig;
                            CoorTempCol = iBlocCol;
                        }
                    }
                }

                // Si le nombre d'occurrence de la valeur i est égal à 1, affecter la valeur
                if (nbOccu == 1 && grilleCand[CoorTempLig][CoorTempCol].nbCandidats > 1)
                {
                    grilleCand[CoorTempLig][CoorTempCol].valeur = i;

                    retirerCandLig(grilleCand, CoorTempLig, i);
                    retirerCandCol(grilleCand, CoorTempCol, i);

                    *progression = true;
                }
            }
        }
    }
}


/* ------- LE SINGLETON NU ------- */

// Fonction de résolution par singleton nu.
// Fonction de résolution par singleton nu.
void singletonNu(tGrille grilleCand, bool *progression)
{
    int nbCandidatCase, indice = 0;

    // Parcours de la grille case par case.
    for (int lig = 0; lig < TAILLE; lig++)
    {
        for (int col = 0; col < TAILLE; col++)
        {
            if (grilleCand[lig][col].valeur == 0) // Aller seulement dans les cases où la valeur est nulle.
            {
                nbCandidatCase = 0;
                for (int i = 1; i <= TAILLE; i++)
                {
                    if (estCandidat(grilleCand[lig][col], i))
                    {
                        indice = i; // Stocker l'indice à laquelle le candidat est présent pour ensuite pouvoir l'enlever de la ligne, colonne, case et l'affecter à la case.
                        nbCandidatCase++; // Compte le nombre de candidats dans le t_candidat d'une case.
                    }
                }

                // Si le nombre de candidats de la case est égal à 1, affecter la valeur et diminuer le nombre de valeurs vides.
                if (nbCandidatCase == 1 && grilleCand[lig][col].nbCandidats == 1)
                {
                    grilleCand[lig][col].valeur = indice;

                    retirerCandLig(grilleCand, lig, indice);
                    retirerCandCol(grilleCand, col, indice);
                    retirerCandCase(grilleCand, lig, col, indice);

                    *progression = true;
                }
            }
        }
    }
}


/* ------- LES DIFFÉRENTES FONCTIONS RETIRER CANDIDATS ------- */

// Fonction pour retirer une valeur de candidat dans toute une ligne.
void retirerCandLig(tGrille grilleCand, int lig, int valeur)
{
    for (int j = 0; j < TAILLE; j++) 
    {
        if (grilleCand[lig][j].valeur == 0 && estCandidat(grilleCand[lig][j], valeur))
        {
            retirerCandidat(&grilleCand[lig][j], valeur);
        }
    }
}

// Fonction pour retirer une valeur de candidat dans toute une colonne.
void retirerCandCol(tGrille grilleCand, int col, int valeur)
{
    for (int k = 0; k < TAILLE; k++) 
    {
        if (grilleCand[k][col].valeur == 0 && estCandidat(grilleCand[k][col], valeur))
        {
            retirerCandidat(&grilleCand[k][col], valeur);
        }
    }
}


// Fonction pour retirer une valeur de candidat dans un bloc entier 3x3.
void retirerCandCase(tGrille grilleCand, int lig, int col, int valeur)
{
    int iLigCase = (lig / N) * N;//Convertis les coordonnées de la case ou on est au début de la case (exemple : ligne 5, (5/3)*3 = 3 la boucle doit commencer à la ligne 3).
    int iColCase = (col / N) * N; 

    // Parcours la case de la valeur où la boucle en est (lig et col) et l'enlève comme candidat dans toute sa case.
    for (int iBlocLig = iLigCase; iBlocLig < iLigCase + N; iBlocLig++)
    {
        for (int iBlocCol = iColCase; iBlocCol < iColCase + N; iBlocCol++)
        {
            if(grilleCand[iBlocLig][iBlocCol].valeur == 0 && estCandidat(grilleCand[iBlocLig][iBlocCol], valeur))
            {
                retirerCandidat(&grilleCand[iBlocLig][iBlocCol], valeur);
            }
        }
    }
}

// Retire un candidat d'une case du Sudoku s'il est présent.
void retirerCandidat(tCase1 *laCase, int val)
{
    // Retirer le candidat s'il a été trouvé.
    if (estCandidat(*laCase, val))
    {
        int i = 0;

        while (i < laCase->nbCandidats)
        {
            if (laCase->t_candidats[i] == val)
            {
                laCase->t_candidats[i] = laCase->t_candidats[laCase->nbCandidats - 1];

                laCase->t_candidats[laCase->nbCandidats - 1] = 0;
                laCase->nbCandidats--;
            }
            else
            {
                i++;
            }
        }
    }
}


/* ------- INITIALISATION DES CANDIDATS ------- */

// Fonction d'initialisation des candidats.
void initCand(tGrille grille)
{
    for (int lig = 0; lig < TAILLE; lig++)
    {
        for (int col = 0; col < TAILLE; col++)
        {
            // Initialiser tous les candidats possibles.
            for (int i = 0; i < TAILLE; i++)
            {
                grille[lig][col].t_candidats[i] = i + 1;
            }
            grille[lig][col].nbCandidats = TAILLE;

            if (grille[lig][col].valeur == 0)
            {
                // Éliminer les candidats déjà présents dans la ligne et colonne.
                for (int i = 0; i < TAILLE; i++)
                {
                    // Éliminer les candidats déjà présents dans la ligne.
                    if (grille[lig][i].valeur != 0)
                    {
                        retirerCandidat(&grille[lig][col], grille[lig][i].valeur);
                    }

                    // Éliminer les candidats déjà présents dans la colonne.
                    if (grille[i][col].valeur != 0)
                    {
                        retirerCandidat(&grille[lig][col], grille[i][col].valeur);
                    }
                }

                // Éliminer les candidats déjà présents dans la sous-grille N x N.
                int sousGrilleLig = lig / N * N;
                int sousGrilleCol = col / N * N;

                for (int i = sousGrilleLig; i < sousGrilleLig + N; i++)
                {
                    for (int j = sousGrilleCol; j < sousGrilleCol + N; j++)
                    {
                        if (grille[i][j].valeur != 0)
                        {
                            retirerCandidat(&grille[lig][col], grille[i][j].valeur);
                        }
                    }
                }
            }
        }
    }
}

/* ------- COMPTER LE NOMBRE D'OCCURENCES D'UN CANDIDAT ------- */

// Compte le nombre d'occurrences d'un candidat donnée dans une colonne.
int nbOccuValParColonne(tGrille grilleCand, int col, int val)
{
    int nbOccuCand = 0;

    // Parcourir la colonne et compte le nombre d'occurrences de la valeur.
    for (int lig = 0; lig < TAILLE; lig++)
    {
        if(grilleCand[lig][col].valeur == 0 && estCandidat(grilleCand[lig][col], val))
        {
            nbOccuCand++;
        }
    }

    return nbOccuCand;
}

// Compte le nombre d'occurrences d'un candidat donnée dans une ligne.
int nbOccuValParLigne(tGrille grilleCand, int lig, int val)
{
    int nbOccuCand = 0;

    // Parcourir la ligne et compte le nombre d'occurrences de la valeur.
    for (int col = 0; col < TAILLE; col++)
    {
        if(grilleCand[lig][col].valeur == 0 && estCandidat(grilleCand[lig][col], val))
        {
            nbOccuCand++;
        }
    }

    return nbOccuCand;
}

// Compte le nombre d'occurrences d'un candidat donnée dans un bloc.
int nbOccuValParBloc(tGrille grilleCand, int lig, int col, int val)
{
    int nbOccuCand = 0;

    // Parcourir le bloc et compte le nombre d'occurrences de la valeur.
    for (int iBlocLig = lig; iBlocLig < lig + N; iBlocLig++)
    {
        for (int iBlocCol = col; iBlocCol < col + N; iBlocCol++)
        {
            if(grilleCand[iBlocLig][iBlocCol].valeur == 0 && estCandidat(grilleCand[iBlocLig][iBlocCol], val))
            {
                nbOccuCand++;
            }
        }
    }

    return nbOccuCand;
}

/* ------- FONCTIONS DIVERSES ------- */

// Ajoute un candidat à une case du Sudoku si celui-ci n'est pas déjà présent.
void ajouterCandidat(tCase1 *laCase, int val)
{
    // Ajouter le candidat s'il n'a pas été trouvé.
    if (!estCandidat(*laCase, val) && laCase->nbCandidats < TAILLE)
    {
        laCase->t_candidats[laCase->nbCandidats] = val;
        laCase->nbCandidats++;
    }
}

// Vérifie si un candidat est présent dans une case du Sudoku.
bool estCandidat(tCase1 laCase, int val)
{
    bool candidatTrouve = false;
    int indiceCandidat = 0;

    while (indiceCandidat < TAILLE && !candidatTrouve)
    {
        if (laCase.t_candidats[indiceCandidat] == val)
        {
            candidatTrouve = true;
        }
        else
        {
            indiceCandidat++;
        }
    }

    return candidatTrouve; 
}

// Charge la grille choisis par l'utilisateur.
void chargerGrille(tGrille g, char nomFichier[])
{    
    FILE * f;

    f = fopen(nomFichier, "rb");
    if (f == NULL)
    {
        printf("\n Fichier %s n'existe pas.\n", nomFichier);
    } 
    else 
    {
        for(int lig = 0; lig < TAILLE; lig++)
        {
            for(int col = 0; col < TAILLE; col++)
            {
                fread(&g[lig][col], sizeof(int), 1, f);
            }
        }
    }
    fclose(f);
}

// Affichage de la grille (pour le débogage).
void affichageGrille(tGrille grilleAffichage)
{
    printf("%s\n", LIGNE_SEPARATION);

    for (int lig = 0; lig < TAILLE; lig++) 
    {
        if (lig % N  == 0 && lig != 0) // Affiche la ligne de séparation si le parcours des lignes est un multiple de trois.
        {
            printf("%s\n", LIGNE_SEPARATION);
        }
        
        printf("%s ", CARAC_PIPE); // Affiche le premier trait vertical à chaque ligne.

        for (int col = 0; col < TAILLE; col++) 
        {
            if(grilleAffichage[lig][col].valeur == 0) // Change les zéros en points, sinon affiche le nombre normal.
            {
                printf("%2s", CARAC_POINT);
            }
            else
            {
                printf("%2d", grilleAffichage[lig][col].valeur);
            }

            if (col % N == N-1) // Affiche le trait vertical si le parcours des colonnes est un multiple de trois, sinon affiche simplement un espace.
            {
                printf(" %s", CARAC_PIPE);
            } 
            else 
            {
                printf("%s", ESPACE);
            }
        }
        printf("\n");
    }

    printf("%s\n", LIGNE_SEPARATION);
}