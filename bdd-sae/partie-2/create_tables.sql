DROP SCHEMA IF EXISTS programme_but CASCADE;
CREATE SCHEMA programme_but;
SET SCHEMA 'programme_but';

CREATE TABLE _competences
(
    lib_competence VARCHAR(100) NOT NULL,
    PRIMARY KEY (lib_competence)
);

CREATE TABLE _semestre
(
    numero_sem VARCHAR(1) NOT NULL,
    PRIMARY KEY (numero_sem)
);

CREATE TABLE _activites
(
    lib_activite   VARCHAR(100) NOT NULL,
    lib_competence VARCHAR(100) NOT NULL,
    PRIMARY KEY (lib_activite)
);

ALTER TABLE _activites
    ADD CONSTRAINT _activites_fk_competence
        FOREIGN KEY (lib_competence)
            REFERENCES _competences (lib_competence);

CREATE TABLE _ue
(
    code_UE      VARCHAR(4)   NOT NULL,
    numero_sem   VARCHAR(1)   NOT NULL,
    lib_activite VARCHAR(100) NOT NULL,
    PRIMARY KEY (code_UE)
);

ALTER TABLE _ue
    ADD CONSTRAINT _ue_fk_semestre
        FOREIGN KEY (numero_sem)
            REFERENCES _semestre (numero_sem),
    ADD CONSTRAINT _ue_fk_activite
        FOREIGN KEY (lib_activite)
            REFERENCES _activites (lib_activite);

CREATE TABLE _parcours
(
    code_P          VARCHAR(1)   NOT NULL,
    libelle_pacours VARCHAR(100) NOT NULL,
    nbre_gpe_TD_P   NUMERIC(10)  NOT NULL,
    nbre_gpe_TP_P   NUMERIC(10)  NOT NULL,
    PRIMARY KEY (code_P)
);

CREATE TABLE _niveau
(
    numero_N NUMERIC(1) NOT NULL,
    PRIMARY KEY (numero_N)
);

CREATE TABLE _correspond
(
    lib_activite VARCHAR(100) NOT NULL,
    numero_N     NUMERIC(1)   NOT NULL,
    code_P       VARCHAR(1)   NOT NULL
);

ALTER TABLE _correspond
    ADD CONSTRAINT _correspond_fk_activites
        FOREIGN KEY (lib_activite)
            REFERENCES _activites (lib_activite),
    ADD CONSTRAINT _correspond_fk_niveau
        FOREIGN KEY (numero_N)
            REFERENCES _niveau (numero_N),
    ADD CONSTRAINT _correspond_fk_parcous
        FOREIGN KEY (code_P)
            REFERENCES _parcours (code_P);

CREATE TABLE _ressources
(
    code_R     VARCHAR(6)   NOT NULL,
    lib_R      VARCHAR(100) NOT NULL,
    nb_h_CM_PN NUMERIC(4)   NOT NULL,
    nb_h_TD_PN NUMERIC(4)   NOT NULL,
    nb_h_TP_PN NUMERIC(4)   NOT NULL,
    numero_sem VARCHAR(1)   NOT NULL,
    code_P     VARCHAR(1)   NOT NULL,
    PRIMARY KEY (code_R),
    FOREIGN KEY (code_p) REFERENCES _parcours (code_p),
    FOREIGN KEY (numero_sem) REFERENCES _semestre (numero_sem)
);

ALTER TABLE _ressources
    ADD CONSTRAINT _ressources_fk_parcours
        FOREIGN KEY (code_P)
            REFERENCES _parcours (code_P),
    ADD CONSTRAINT _ressources_fk_semestre
        FOREIGN KEY (numero_sem)
            REFERENCES _semestre (numero_sem);

CREATE TABLE _sae
(
    code_SAE                 VARCHAR(6)   NOT NULL,
    lib_SAE                  VARCHAR(100) NOT NULL,
    nb_h_TD_enc              NUMERIC(4)   NOT NULL,
    nb_h_TP_projet_autonomie NUMERIC(4)   NOT NULL,
    PRIMARY KEY (code_SAE)
);

CREATE TABLE _comprend_r
(
    code_R    VARCHAR(255) NOT NULL,
    code_SAE  VARCHAR(255) NOT NULL,
    nb_h_TD_C NUMERIC(10)  NOT NULL,
    nb_h_TP_C NUMERIC(10)  NOT NULL,
    PRIMARY KEY (code_R, code_SAE),
    FOREIGN KEY (code_R) REFERENCES _ressources (code_R),
    FOREIGN KEY (code_SAE) REFERENCES _sae (code_SAE)
);

ALTER TABLE _comprend_r
    ADD CONSTRAINT _comprend_r_fk_resources
        FOREIGN KEY (code_R)
            REFERENCES _ressources (code_R),
    ADD CONSTRAINT _comprend_r_fk_sae
        FOREIGN KEY (code_SAE)
            REFERENCES _sae (code_SAE);
