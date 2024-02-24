/**
 *
 * @file ELIMINATION_Version1.c
 * @author Loris Caruhel - Enzo Vivion-Michaud
 * @brief Programme qui permet de résoudre des grilles de sudoku automatiquement.
 * @version 1.0
 * @date 06/01/2024
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <unistd.h>

// Définition de la taille de la grille Sudoku.
#define N 3
#define TAILLE (N * N)

// Structure représentant une case Sudoku avec sa valeur, ses candidats et son nombre de candidats.
typedef struct
{
    int valeur;
    int t_candidats[TAILLE + 1];
    int nbCandidats;
    bool dejaExaminer; // Permet de savoir si cette case à été examiné par la paire nue et donc de mettre progression à true ou non.
} tCase1;

// Définition du type tGrilleCand qui est une grille Sudoku de cases tCase2.
typedef tCase1 tGrille[TAILLE][TAILLE];

// Constantes pour l'affichage de la grille.
const char LIGNE_SEPARATION[28] = "  +-------+-------+-------+";
const char CARAC_POINT[2] = ".";
const char CARAC_PIPE[2] = "|";
const char ESPACE[2] = " ";

// Prototypes des fonctions.
int chargerGrille(tGrille g, char nomFichier[]);
void afficheStat(int nbCaseRemplis, int nbCaseARemplir, int nbCandidatInitial, int nbCandidatFinal, char nomFichier[]);
void initCand(tGrille grilleCand);

int nbCandidatTotal(tGrille grilleCand);
void affichageGrille(tGrille grilleAffichage);

void retirerCandLig(tGrille grilleCand, int lig, int valeur);
void retirerCandCol(tGrille grilleCand, int col, int valeur);
void retirerCandCase(tGrille grilleCand, int lig, int col, int valeur);

void ajouterCandidat(tCase1 *laCase, int val);
void retirerCandidat(tCase1 *laCase, int val);
bool estCandidat(tCase1 laCase, int val);
void lavageCandidat(tGrille grilleCand);

int fn_nbCandidat(tCase1 *laCase);
int fnNbCasesVides(tGrille grilleSudoku);

int nbOccuValParBloc(tGrille grilleCand, int lig, int col, int val);
int nbOccuValParLigne(tGrille grilleCand, int lig, int val);
int nbOccuValParColonne(tGrille grilleCand, int col, int val);

// Prototype des fonctions de technique de résolution.
void singletonNu(tGrille grilleCand, int *nbCasesVides, bool *progression, int *nbCaseRemplis);

void singletonCacheBloc(tGrille grilleCand, int *nbCasesVides, bool *progression, int *nbCaseRemplis);
void singletonCacheLigne(tGrille grilleCand, int *nbCasesVides, bool *progression, int *nbCaseRemplis);
void singletonCacheColonne(tGrille grilleCand, int *nbCasesVides, bool *progression, int *nbCaseRemplis);

void paireCacheeBloc(tGrille grilleCand, bool *progression);
void paireCacheeColonne(tGrille grilleCand, bool *progression);
void paireCacheeLigne(tGrille grilleCand, bool *progression);

void paireNueBloc(tGrille grilleCand, bool *progression);
void paireNueLigne(tGrille grilleCand, bool *progression);
void paireNueColonne(tGrille grilleCand, bool *progression);

int main()
{
    // Initialisation des variables.
    tGrille grilleCand;
    bool progression = true;
    int nbCasesVides = 0, nbCandidatInitial = 0, nbCandidatFinal = 0, nbCaseRemplis = 0, nbCasesVidesIni = 0;
    char nomFichier[30]; //Variable qui va stocker le nom du fichier à charger dans la grille.

    system("clear");
    printf("Nom du fichier ? (Grille[A-G].sud)\n");
    scanf("%s", nomFichier);

    // Affichage de la grille chargée.
    system("clear");
    nbCasesVides =  chargerGrille(grilleCand, nomFichier); // Cette variable sert pour la condition while.
    nbCasesVidesIni =  nbCasesVides; // Celle ci pour sauvegarder le nombre de case vide au départ pour le affiche stat à la fin.
    printf("Grille chargé avec succès !\n");
    sleep(1);
    system("clear");

    // Affichage de la grille initiale.
    printf("Grille vide :\n");
    affichageGrille(grilleCand);

    // Initialisation des candidats.
    initCand(grilleCand);

    // Calcul du nombre de candidats initial.
    nbCandidatInitial = nbCandidatTotal(grilleCand);

    printf("Nombre de cases vides au début : %d\n", nbCasesVidesIni);

    // Boucle principale pour résoudre la grille.
    while(nbCasesVides != 0 && progression)
    {
        progression = false;

        // Appel des différentes techniques de résolution.
        paireCacheeBloc(grilleCand, &progression);
        paireCacheeLigne(grilleCand, &progression);
        paireCacheeColonne(grilleCand, &progression);

        paireNueBloc(grilleCand, &progression);
        paireNueLigne(grilleCand, &progression);
        paireNueColonne(grilleCand, &progression);

        singletonNu(grilleCand, &nbCasesVides, &progression, &nbCaseRemplis);

        singletonCacheBloc(grilleCand, &nbCasesVides, &progression, &nbCaseRemplis);
        singletonCacheLigne(grilleCand, &nbCasesVides, &progression, &nbCaseRemplis);
        singletonCacheColonne(grilleCand, &nbCasesVides, &progression, &nbCaseRemplis);

        lavageCandidat(grilleCand); // Sert surtout pour le débogage.
    }

    // Calcul du nombre de candidats final.
    nbCandidatFinal = nbCandidatTotal(grilleCand);

    // Affichage de la grille finale et des statistiques.
    printf("\nGrille non vide (complété si le programme a réussi) :\n");
    affichageGrille(grilleCand);
    afficheStat(nbCaseRemplis, nbCasesVidesIni, nbCandidatInitial, nbCandidatFinal, nomFichier);

    return EXIT_SUCCESS;
}


/* ------- LES PAIRES CACHÉES ------- */

// Fonction de résolution par la paire cachée de toutes les colonnes.
void paireCacheeColonne(tGrille grilleCand, bool *progression)
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
                        *progression = true; 
                    }
                }
            }
        }
    }
}

// Fonction de résolution par la paire cachée de toutes les lignes.
void paireCacheeLigne(tGrille grilleCand, bool *progression)
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
                        *progression = true; 
                    }
                }
            }
        }
    }
}

// Fonction de résolution par paire cachée dans tous les blocs.
void paireCacheeBloc(tGrille grilleCand, bool *progression)
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
                            *progression = true;
                        }
                    }
                }
            }
        }
    }
}


/* ------- LES PAIRES NUES ------- */

// Fonction de résolution par paire nue sur les colonnes.
void paireNueColonne(tGrille grilleCand, bool *progression)
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
            
            // Vérification pour la progression et marquage des cases déjà traitées.
            if (grilleCand[coord1[0]][coord1[1]].dejaExaminer == false &&
                grilleCand[coord2[0]][coord2[1]].dejaExaminer == false)
            {
                *progression = true;
            }

            grilleCand[coord1[0]][coord1[1]].dejaExaminer = true;
            grilleCand[coord2[0]][coord2[1]].dejaExaminer = true;
        }
    }
}

// Fonction de résolution par paire nue sur les lignes.
void paireNueLigne(tGrille grilleCand, bool *progression)
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

            // Vérification pour la progression et marquage des cases déjà traitées.
            if (grilleCand[coord1[0]][coord1[1]].dejaExaminer == false &&
                grilleCand[coord2[0]][coord2[1]].dejaExaminer == false)
            {
                *progression = true;
            }

            grilleCand[coord1[0]][coord1[1]].dejaExaminer = true;
            grilleCand[coord2[0]][coord2[1]].dejaExaminer = true;
        }
    }
}

// Fonction de résolution par paire nue dans un bloc.
void paireNueBloc(tGrille grilleCand, bool *progression)
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
                
                // Vérification pour la progression et marquage des cases déjà traitées
                if (grilleCand[coord1[0]][coord1[1]].dejaExaminer == false &&
                    grilleCand[coord2[0]][coord2[1]].dejaExaminer == false)
                {
                    *progression = true;
                }

                grilleCand[coord1[0]][coord1[1]].dejaExaminer = true;
                grilleCand[coord2[0]][coord2[1]].dejaExaminer = true;
            }
        }
    }
}


/* ------- LES SINGLETONS CACHÉS ------- */

// Fonction de résolution par singleton caché sur les colonnes.
void singletonCacheColonne(tGrille grilleCand, int *nbCasesVides, bool *progression, int *nbCaseRemplis)
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
            if (nbOccu == 1)
            {
                grilleCand[CoorTempLig][CoorTempCol].valeur = i;
                (*nbCasesVides)--;
                (*nbCaseRemplis)++;

                retirerCandLig(grilleCand, CoorTempLig, i);
                retirerCandCol(grilleCand, CoorTempCol, i);
                retirerCandCase(grilleCand, CoorTempLig, CoorTempCol, i);

                *progression = true;
            }
        }
    }
}

// Fonction de résolution par singleton caché sur les lignes.
void singletonCacheLigne(tGrille grilleCand, int *nbCasesVides, bool *progression, int *nbCaseRemplis)
{
    int nbOccu, CoorTempLig, CoorTempCol;

    // Parcours toutes les lignes de la grille.
    for (int lig = 0; lig < TAILLE; lig++)
    {
        // Parcours chaque ligne 9 fois et compte le nombre d'occurence de chaque valeur de 1 à 9.
        for(int i = 1; i <= TAILLE; i++)
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
            if (nbOccu == 1)
            {
                grilleCand[CoorTempLig][CoorTempCol].valeur = i;
                (*nbCasesVides)--;
                (*nbCaseRemplis)++;

                retirerCandLig(grilleCand, CoorTempLig, i);
                retirerCandCol(grilleCand, CoorTempCol, i);

                *progression = true;
            }
        }
    }
}

// Fonction de résolution par singleton caché sur les blocs.
void singletonCacheBloc(tGrille grilleCand, int *nbCasesVides, bool *progression, int *nbCaseRemplis)
{
    int nbOccu, CoorTempLig, CoorTempCol;

    //Parcours tous les blocs de la grille.
    for (int lig = 0; lig < TAILLE; lig += N)
    {
        for (int col = 0; col < TAILLE; col += N)
        {
            //Parcours chaque bloc 9 fois et compte le nombre d'occurence de chaque valeur de 1 à 9.
            for(int i = 1; i <= TAILLE; i++)
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
                if (nbOccu == 1)
                {
                    grilleCand[CoorTempLig][CoorTempCol].valeur = i;
                    (*nbCasesVides)--;
                    (*nbCaseRemplis)++;

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
void singletonNu(tGrille grilleCand, int *nbCasesVides, bool *progression, int *nbCaseRemplis)
{
    for(int lig = 0; lig < TAILLE; lig++)
    {
        for(int col = 0; col < TAILLE; col++)
        {
            if(grilleCand[lig][col].valeur == 0 && grilleCand[lig][col].nbCandidats == 1)
            {
                grilleCand[lig][col].valeur = grilleCand[lig][col].t_candidats[0];

                int valeur = grilleCand[lig][col].t_candidats[0];
                
                retirerCandidat(&grilleCand[lig][col], valeur);

                retirerCandLig(grilleCand, lig, valeur);
                retirerCandCol(grilleCand, col, valeur);
                retirerCandCase(grilleCand, lig, col, valeur);

                (*nbCaseRemplis)++;
                (*nbCasesVides)--;
                *progression = true;
            }
        }
    }
}


/* ------- LES DIFFÉRENTES FONCTIONS RETIRER CANDIDATS ------- */

// Fonction pour retirer une valeur de candidat dans toute une ligne.
void retirerCandLig(tGrille grilleCand, int lig, int valeur)
{
    //Parcours la ligne de la valeur où la boucle en est (lig et col) et l'enlève comme candidat dans toute sa ligne.
    for (int col = 0; col < TAILLE; col++) 
    {
        if(grilleCand[lig][col].valeur == 0)
        {
            retirerCandidat(&grilleCand[lig][col], valeur);
        }
    }       
}

// Fonction pour retirer une valeur de candidat dans toute une colonne.
void retirerCandCol(tGrille grilleCand, int col, int valeur)
{
    //Parcours la colonne de la valeur où la boucle en est (lig et col) et l'enlève comme candidat dans toute sa colonne.
    for (int lig = 0; lig < TAILLE; lig++) 
    {
        if(grilleCand[lig][col].valeur == 0)
        {
            retirerCandidat(&grilleCand[lig][col], valeur);
        }
    }   
}

// Fonction pour retirer une valeur de candidat dans un bloc entier 3x3.
void retirerCandCase(tGrille grilleCand, int lig, int col, int valeur)
{
    int iLigCase = (lig / N) * N;//Convertis les coordonnées de la case ou on est au début de la case (exemple : ligne 5, (5/3)*3 = 3 la boucle doit commencer à la ligne 3).
    int iColCase = (col / N) * N; 

    // Parcours la case de la valeur où la boucle en est (lig et col) et l'enlève comme candidat dans toute sa case.
    for (int iBlocLig = iLigCase; iBlocLig <= iLigCase + 2; iBlocLig++)
    {
        for (int iBlocCol = iColCase; iBlocCol <= iColCase + 2; iBlocCol++)
        {
            if(grilleCand[iBlocLig][iBlocCol].valeur == 0)
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
                // Décaler les candidats suivants pour remplir le trou.
                int j = i;
                while (j < laCase->nbCandidats - 1)
                {
                    laCase->t_candidats[j] = laCase->t_candidats[j + 1];
                    j++;
                }

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
            grille[lig][col].dejaExaminer = false; // Permet dans les paires nues de mettre progression à true ou non pour éviter les boucles infinis.

            // Initialiser tous les candidats possibles.
            for (int i = 0; i < TAILLE; i++)
            {
                grille[lig][col].t_candidats[i] = i + 1;
            }
            grille[lig][col].nbCandidats = TAILLE;

            
            if (grille[lig][col].valeur == 0)
            {
                // Éliminer les candidats déjà présents dans la ligne.
                for (int j = 0; j < TAILLE; j++)
                {
                    if (grille[lig][j].valeur != 0)
                    {
                        retirerCandidat(&grille[lig][col], grille[lig][j].valeur);
                    }
                }
            
                // Éliminer les candidats déjà présents dans la colonne.
                for (int k = 0; k < TAILLE; k++)
                {
                    if (grille[k][col].valeur != 0)
                    {
                        retirerCandidat(&grille[lig][col], grille[k][col].valeur);
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

// Fonction qui "lave" tous les candidats dans les cases qui ont des valeurs pour que le débogage soit plus propre.
void lavageCandidat(tGrille grilleCand)
{
    for(int lig = 0; lig < TAILLE; lig++)
    {
        for(int col = 0; col < TAILLE; col++)
        {
            if(grilleCand[lig][col].valeur != 0)
            {
                for(int i = 0; i <= TAILLE; i++)
                {
                    for(int j = 0; j <= TAILLE; j++)
                    {
                        retirerCandidat(&grilleCand[lig][col], j);
                    }
                    grilleCand[lig][col].nbCandidats = 0;
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

// Compte le nombre total de candidats dans la grille.
int nbCandidatTotal(tGrille grilleCand)
{
    int nbCandidatTotal = 0;

    for(int lig = 0; lig < TAILLE; lig++)
    {
        for(int col = 0; col < TAILLE; col++)
        {
            if(grilleCand[lig][col].valeur == 0)
            {
                nbCandidatTotal = nbCandidatTotal + grilleCand[lig][col].nbCandidats;
            }
        }
    }

    return nbCandidatTotal;
}

// Retourne le nombre de candidats dans une case Sudoku.
int fn_nbCandidat(tCase1 *laCase)
{
    int nbCandidats = 0;

    nbCandidats = laCase->nbCandidats;

    return nbCandidats;
}


/* ------- AFFICHE STATS ------- */

// Affiche les statistiques du remplissage de grille.
void afficheStat(int nbCaseRemplis, int nbCaseARemplir, int nbCandidatInitial, int nbCandidatFinal, char nomFichier[])
{
    float taux_de_remplissage;
    float pourcentage_d_elimination;
    int nbCandidatElimines;

    nbCandidatElimines = nbCandidatInitial - nbCandidatFinal;

    if (nbCaseARemplir > 0)
    {
        taux_de_remplissage = ((float)nbCaseRemplis / nbCaseARemplir) * 100.0;
    }
    else
    {
        taux_de_remplissage = -1;
    }

    if (nbCandidatInitial > 0)
    {
        pourcentage_d_elimination = ((float)nbCandidatElimines / nbCandidatInitial) * 100.0;
    }
    else
    {
        pourcentage_d_elimination = -1;
    }

    printf("******  RESULTATS POUR %s  ******\n", nomFichier);
    printf("Nombre de cases remplies = %d sur %d    Taux de remplissage = %.3f %%\n", nbCaseRemplis, nbCaseARemplir, taux_de_remplissage);
    printf("Nombre de candidats éliminés = %d       Pourcentage d'élimination = %.3f  %%\n", nbCandidatElimines, pourcentage_d_elimination);
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
int chargerGrille(tGrille g, char nomFichier[])
{
    FILE * f;

    f = fopen(nomFichier, "rb");
    if (f==NULL)
    {
        printf("\n Fichier %s n'existe pas.\n", nomFichier);
    } 
    else 
    {
        for(int lig = 0; lig < TAILLE; lig++)
        {
            for(int col = 0; col < TAILLE; col++)
            {
                fread(&g[lig][col].valeur, sizeof(int), 1, f);
            }
        }
    }
    fclose(f);

    return fnNbCasesVides(g);
}

// Affichage de la grille (pour le débogage).
void affichageGrille(tGrille grilleAffichage)
{
    printf("    1 2 3   4 5 6   7 8 9\n");
    printf("%s\n", LIGNE_SEPARATION);

    for (int lig = 0; lig < TAILLE; lig++) 
    {
        if (lig % N == 0 && lig != 0) //Si le parcour des lignes est un multiple de trois afficher la ligne séparatrice. 
        {
            printf("%s\n", LIGNE_SEPARATION);
        }

        printf("%d %s ", lig + 1, CARAC_PIPE); // Affiche le premier pipe à chaques lignes.

        for (int col = 0; col < TAILLE; col++) 
        {
            if(grilleAffichage[lig][col].valeur == 0) //Change les zéros en point sinon affiche le nombre "normal".
            {
                printf("%s", CARAC_POINT);
            }
            else
            {
                printf("%d", grilleAffichage[lig][col].valeur);
            }

            if (col % N == 2 && col != (TAILLE - 1)) //Si le parcours des colonnes est un multiple de trois afficher le pipe sinon faire simplement un espace.
            {
                printf(" %s ", CARAC_PIPE);
            } 
            else 
            {
                printf("%s", ESPACE);
            }
        }

        printf("%s\n", CARAC_PIPE); //Affiche le dernier pipe à la fin de chaque ligne.
    }

    printf("%s\n", LIGNE_SEPARATION);
}

// Compte le nombre total de cases vides.
int fnNbCasesVides(tGrille grilleSudoku)
{
    int nbCasesVides = 0;

    for(int lig = 0; lig < TAILLE; lig++)
    {
        for(int col = 0; col < TAILLE; col++)
        {
            if (grilleSudoku[lig][col].valeur == 0)
            {
                nbCasesVides++;
            }
        }
    }

    return nbCasesVides;
}
