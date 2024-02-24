/*Semaine 1 (A pofinier)*/
Semestre :
    no_sem INT,
    KEY{no_sem};

UE :
    no_ue INT,
    libelle_ue CHAR,
    no_sem INT, FK semestre.no_sem
    no_competence INT,
    no_niv INT              /*Ajout du niveau car ("Chaque U.E. se réfère à une compétence finale et à un niveau de cette compétence.")*/
    KEY{no_ue, no_sem};

Niveau : 
    no_niv INT
    KEY{no_niv};

Activité : 
    libelle_act CHAR,
    no_niv INT
    KEY{libelle_act};

Compétence :
    no_competence INT,
    libelle_competence CHAR,
    nom_parcours CHAR, FK parcours.nom_parcours
    KEY{no_competence};

Parcours : 
    nom_parcours CHAR,
    KEY{nom_parcours};


/*
- Un parcours a 6 compétences, et 3 niveaux.

- Un niveau a 6 compétences ou moins.

- Dans chaque parcours les étudiants sont séparés dans plusieurs 
  groupe de TD qui sont eux mêmes séparés en groupe de TP.

- Une compétence a une activitée par niveau maximum, et une activité à 2 UE à chaques fois.

- Un groupe TD est définis comme sa :
    (no_niv, nom_grp_TD)
  Le nom du parcours est rajouté car il faut différencier les 
  groupe de TD de chaques années (2A et 1A par exemple)

- Les groupes de TP sont eux définis par leurs groupe de TD : 
    (no_grp_TD, no_grp_TP)     Exemple : A1, A2
  Pour les différencier de chaques années le lien avec le groupe
  de TD le fait déjà normalement.
*/


/*Semaine 2 (A finir)*/
Ressource :
    code_ress CHAR, -- utiliser les types pour 'R', '2 chiffres'
    no_ress INT,
    no_sem INT, FK semestre.no_sem
    nom_parcours CHAR, FK parcours.nom_parcours
    KEY{no_sem, nom_parcours, no_ress};
/*
Ajouter le lien qu'ils ont avec les UE durant la 
semaine 2. (semaine du 04 décembre).
*/
SAE : 
    code_sae CHAR,
    no_sae INT,
    no_sem INT, FK semestre.no_sem
    nom_parcours CHAR,
    libelle_sae CHAR,
    KEY{no_sae, no_sem};


/*
A la fin de la quatrième semaines rendre le diagramme de classe
en PDF sur moodle avec la notation :
        DC_GroupeTP_nomDeFamilleEtudiant1_nomDeFamilleEtudiant2.pdf
*/