/**
 *
 * @file RESOLUTION_1.c
 * @author Loris Caruhel - Enzo Vivion-Michaud
 * @brief Programme qui permet de résoudre des grilles de sudoku automatiquement.
 * @version 1.0
 * @date 17/01/2024
 * 
 * @copyright Copyright (c) 2023
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

// Définition du type tGrilleCand qui est une grille Sudoku de cases tCase2.
typedef int tGrilleCand[TAILLE][TAILLE];

// Constantes pour l'affichage de la grille.
const char LIGNE_SEPARATION[70] = "+-------------+------------+------------+------------+";
const char CARAC_POINT[2] = ".";
const char CARAC_PIPE[2] = "|";
const char ESPACE[2] = " ";

// Prototypes des fonctions.
void chargerGrille(tGrilleCand g, char nomFichier[]);
void affichageGrille(tGrilleCand grilleAffichage);

bool absentSurLigne(int valeur, tGrilleCand grille, int ligne);
bool absentSurColonne(int valeur, tGrilleCand grille, int colonne);
bool absentSurBloc(int valeur, tGrilleCand grille, int ligne, int colonne);

bool backtracking(tGrilleCand grille, int numeroCase);

int main()
{    
    // Initialisation des variables.
    tGrilleCand grilleCand;
    char nomFichier[30];
    int numeroCase = 0;

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

    // Résolution.
    backtracking(grilleCand, numeroCase);

    clock_t end = clock();

    // Affichage de la grille finale et des statistiques.
    printf("\nGrille finale :\n");
    affichageGrille(grilleCand);

    double tmpsCPU = (end - begin)*1.0 / CLOCKS_PER_SEC;
    printf("Résolution de la grille en %.3f secondes\n",tmpsCPU);
}


// Charge la grille choisis par l'utilisateur.
void chargerGrille(tGrilleCand g, char nomFichier[])
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
void affichageGrille(tGrilleCand grilleAffichage)
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
            if(grilleAffichage[lig][col] == 0) // Change les zéros en points, sinon affiche le nombre normal.
            {
                printf("%2s", CARAC_POINT);
            }
            else
            {
                printf("%2d", grilleAffichage[lig][col]);
            }

            if (col % N == 3) // Affiche le trait vertical si le parcours des colonnes est un multiple de trois, sinon affiche simplement un espace.
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

bool backtracking(tGrilleCand grille, int numeroCase)
{
    int ligne, colonne;
    bool resultat = false;

    if(numeroCase == TAILLE * TAILLE)
    {
        resultat = true;
    }
    else
    {
        ligne = numeroCase / TAILLE;
        colonne = numeroCase % TAILLE;

        if(grille[ligne][colonne] != 0)
        {
            resultat = backtracking(grille, numeroCase + 1);
        }
        else
        {
            for(int valeur = 1; valeur <= TAILLE; valeur++)
            {
                if(absentSurLigne(valeur, grille, ligne) &&
                   absentSurColonne(valeur, grille, colonne) &&
                   absentSurBloc(valeur, grille, ligne, colonne))
                {
                    grille[ligne][colonne] = valeur;

                    if(backtracking(grille, numeroCase + 1))
                    {
                        resultat = true;
                    }
                    else
                    {
                        grille[ligne][colonne] = 0;
                    }
                }
            }
        }
    }

    return resultat;
}

bool absentSurLigne(int valeur, tGrilleCand grille, int ligne)
{
    bool absent = true;
    int col = 0;

    while(col < TAILLE && absent)
    {
        if(grille[ligne][col] == valeur)
        {
            absent = false;
        }
        col++;
    }

    return absent;
}

bool absentSurColonne(int valeur, tGrilleCand grille, int colonne)
{
    bool absent = true;
    int lig = 0;

    while(lig < TAILLE && absent)
    {
        if(grille[lig][colonne] == valeur)
        {
            absent = false;
        }
        lig++;
    }

    return absent;
}

bool absentSurBloc(int valeur, tGrilleCand grille, int ligne, int colonne)
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
            if (grille[iCaseLig][iCaseCol] == valeur)
            {
                absent = false;
            }
            iCaseCol++;
        }
        iCaseLig++;
    }

    return absent;
}