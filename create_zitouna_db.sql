-- ============================================================================
-- Script de creation de la base de donnees ZITOUNA
-- Oracle XE - Management Oil Application
-- ============================================================================
-- Executer en etant connecte en tant que ZITOUNA
-- ============================================================================

-- ============================================================================
-- 1. Nettoyage complet (supprimer les objets existants)
-- ============================================================================
BEGIN EXECUTE IMMEDIATE 'DROP TRIGGER TRG_PRODUCTION_ID'; EXCEPTION WHEN OTHERS THEN NULL; END;
/
BEGIN EXECUTE IMMEDIATE 'DROP TRIGGER TRG_STOCK_ID'; EXCEPTION WHEN OTHERS THEN NULL; END;
/
BEGIN EXECUTE IMMEDIATE 'DROP TRIGGER TRG_CLIENT_ID'; EXCEPTION WHEN OTHERS THEN NULL; END;
/
BEGIN EXECUTE IMMEDIATE 'DROP TRIGGER TRG_MACHINE_ID'; EXCEPTION WHEN OTHERS THEN NULL; END;
/
BEGIN EXECUTE IMMEDIATE 'DROP TRIGGER EMPLOYEES_BI'; EXCEPTION WHEN OTHERS THEN NULL; END;
/
BEGIN EXECUTE IMMEDIATE 'DROP TABLE PRODUCTION CASCADE CONSTRAINTS'; EXCEPTION WHEN OTHERS THEN NULL; END;
/
BEGIN EXECUTE IMMEDIATE 'DROP TABLE STOCK CASCADE CONSTRAINTS'; EXCEPTION WHEN OTHERS THEN NULL; END;
/
BEGIN EXECUTE IMMEDIATE 'DROP TABLE CLIENT CASCADE CONSTRAINTS'; EXCEPTION WHEN OTHERS THEN NULL; END;
/
BEGIN EXECUTE IMMEDIATE 'DROP TABLE MACHINE CASCADE CONSTRAINTS'; EXCEPTION WHEN OTHERS THEN NULL; END;
/
BEGIN EXECUTE IMMEDIATE 'DROP TABLE EMPLOYEES CASCADE CONSTRAINTS'; EXCEPTION WHEN OTHERS THEN NULL; END;
/
BEGIN EXECUTE IMMEDIATE 'DROP SEQUENCE SEQ_PRODUCTION'; EXCEPTION WHEN OTHERS THEN NULL; END;
/
BEGIN EXECUTE IMMEDIATE 'DROP SEQUENCE SEQ_STOCK'; EXCEPTION WHEN OTHERS THEN NULL; END;
/
BEGIN EXECUTE IMMEDIATE 'DROP SEQUENCE SEQ_CLIENT'; EXCEPTION WHEN OTHERS THEN NULL; END;
/
BEGIN EXECUTE IMMEDIATE 'DROP SEQUENCE SEQ_MACHINE'; EXCEPTION WHEN OTHERS THEN NULL; END;
/
BEGIN EXECUTE IMMEDIATE 'DROP SEQUENCE USERS_SEQ'; EXCEPTION WHEN OTHERS THEN NULL; END;
/

-- ============================================================================
-- 2. Sequence CLIENT
-- ============================================================================
CREATE SEQUENCE SEQ_CLIENT START WITH 1 INCREMENT BY 1;

-- =========================================================================
-- USERS table (Oracle 11g compatible) + sequence + trigger
-- =========================================================================
CREATE SEQUENCE USERS_SEQ
START WITH 1
INCREMENT BY 1
NOCACHE;

CREATE TABLE EMPLOYEES (
    USER_ID NUMBER PRIMARY KEY,

    USERNAME VARCHAR2(100) UNIQUE NOT NULL,
    EMAIL VARCHAR2(150) UNIQUE NOT NULL,
    PASSWORD_HASH VARCHAR2(255) NOT NULL,

    ROLE VARCHAR2(30) NOT NULL,

    STATE VARCHAR2(20) DEFAULT 'ACTIVE',

    FAILED_ATTEMPTS NUMBER DEFAULT 0,

    LAST_LOGIN TIMESTAMP,

    CREATED_AT TIMESTAMP DEFAULT CURRENT_TIMESTAMP,

    LAST_ACTION VARCHAR2(100),
    LAST_ACTION_TIME TIMESTAMP
);

CREATE OR REPLACE TRIGGER EMPLOYEES_BI
BEFORE INSERT ON EMPLOYEES
FOR EACH ROW
BEGIN
  SELECT USERS_SEQ.NEXTVAL
  INTO :NEW.USER_ID
  FROM DUAL;
END;
/

-- ============================================================================
-- 3. Table CLIENT
-- ============================================================================
CREATE TABLE CLIENT (
    ID_CLIENT               NUMBER PRIMARY KEY,
    NOM                     VARCHAR2(100) NOT NULL,
    PRENOM                  VARCHAR2(100) NOT NULL,
    TELEPHONE               VARCHAR2(20),
    ADRESSE                 VARCHAR2(200),
    EMAIL                   VARCHAR2(150),
    TYPE_CLIENT             VARCHAR2(50) DEFAULT 'Particulier',
    TOTAL_OLIVES_LIVREES    NUMBER(10,2) DEFAULT 0,
    DATE_CREATION           DATE DEFAULT SYSDATE,
    STATUT                  VARCHAR2(50) DEFAULT 'Standard'
);

-- Trigger auto-increment CLIENT
CREATE OR REPLACE TRIGGER TRG_CLIENT_ID
BEFORE INSERT ON CLIENT
FOR EACH ROW
BEGIN
    IF :NEW.ID_CLIENT IS NULL THEN
        SELECT SEQ_CLIENT.NEXTVAL INTO :NEW.ID_CLIENT FROM DUAL;
    END IF;
END;
/

-- ============================================================================
-- 4. Sequence STOCK
-- ============================================================================
CREATE SEQUENCE SEQ_STOCK START WITH 1 INCREMENT BY 1;

-- ============================================================================
-- 5. Table STOCK
-- ============================================================================
CREATE TABLE STOCK (
    ID_STOCK                    NUMBER PRIMARY KEY,
    TYPE_HUILE                  VARCHAR2(100) NOT NULL,
    QUANTITE_ACTUELLE           NUMBER(10,2) DEFAULT 0,
    SEUIL_ALERTE                NUMBER(10) DEFAULT 50,
    DATE_DERNIERE_MISE_A_JOUR   DATE DEFAULT SYSDATE,
    EMPLACEMENT_STOCKAGE        VARCHAR2(100),
    ETAT_STOCK                  VARCHAR2(50) DEFAULT 'Disponible'
);

-- Trigger auto-increment STOCK
CREATE OR REPLACE TRIGGER TRG_STOCK_ID
BEFORE INSERT ON STOCK
FOR EACH ROW
BEGIN
    IF :NEW.ID_STOCK IS NULL THEN
        SELECT SEQ_STOCK.NEXTVAL INTO :NEW.ID_STOCK FROM DUAL;
    END IF;
END;
/

-- ============================================================================
-- 6. Sequence MACHINE
-- ============================================================================
CREATE SEQUENCE SEQ_MACHINE START WITH 1 INCREMENT BY 1;

-- ============================================================================
-- 7. Table MACHINE
-- ============================================================================
CREATE TABLE MACHINE (
    ID_MACHINE                  NUMBER PRIMARY KEY,
    NOM_MACHINE                 VARCHAR2(100) NOT NULL,
    TYPE_MACHINE                VARCHAR2(50) DEFAULT 'Presse',
    ETAT_MARCHE                 VARCHAR2(10) DEFAULT 'OFF',
    TEMPERATURE_ACTUELLE        NUMBER(5,2) DEFAULT 0,
    NIVEAU_CHARGE               NUMBER(5,2) DEFAULT 0,
    ETAT_FONCTIONNEMENT         VARCHAR2(50) DEFAULT 'Normal',
    TYPE_ALERTE                 VARCHAR2(50) DEFAULT 'Aucune',
    NIVEAU_CRITICITE            VARCHAR2(50) DEFAULT 'Faible',
    DATE_DERNIERE_MAINTENANCE   DATE DEFAULT SYSDATE,
    DATE_INSTALLATION           DATE DEFAULT SYSDATE,
    SCORE_SANTE                 NUMBER(3) DEFAULT 100,
    DATE_MISE_A_JOUR            TIMESTAMP DEFAULT SYSTIMESTAMP
);

-- Trigger auto-increment MACHINE
CREATE OR REPLACE TRIGGER TRG_MACHINE_ID
BEFORE INSERT ON MACHINE
FOR EACH ROW
BEGIN
    IF :NEW.ID_MACHINE IS NULL THEN
        SELECT SEQ_MACHINE.NEXTVAL INTO :NEW.ID_MACHINE FROM DUAL;
    END IF;
END;
/

-- ============================================================================
-- 8. Sequence PRODUCTION
-- ============================================================================
CREATE SEQUENCE SEQ_PRODUCTION START WITH 1 INCREMENT BY 1 NOCACHE NOCYCLE;

-- ============================================================================
-- 9. Table PRODUCTION
-- ============================================================================
CREATE TABLE PRODUCTION (
    IDPRODUCTION            NUMBER PRIMARY KEY,
    ID_CLIENT               NUMBER,
    ID_USER                 NUMBER,
    ID_MACHINE              NUMBER,
    ID_STOCK                NUMBER,
    DATEPRODUCTION          DATE DEFAULT SYSDATE,
    QUANTITEOLIVESKG        NUMBER(10) DEFAULT 0,
    TYPEPRESSAGE            VARCHAR2(100),
    NOTESPLANIFICATION      VARCHAR2(500),
    HEUREDEMARRAGE          TIMESTAMP,
    DUREEESTIMEE            NUMBER(10) DEFAULT 0,
    TEMPSECOULE             NUMBER(10) DEFAULT 0,
    STATUT                  VARCHAR2(50) DEFAULT 'En attente',
    HUILEPRODUITEL          NUMBER(10,2) DEFAULT 0,
    RENDEMENT               NUMBER(5,2) DEFAULT 0,
    QUALITE                 VARCHAR2(50) DEFAULT 'Standard',
    CONFORMENORMES          NUMBER(1) DEFAULT 0,
    ACIDITE                 NUMBER(4,2),
    SCOREQUALITEIA          NUMBER(5,2),
    CONFIANCEIA             NUMBER(5,2),
    REMARQUESQUALITE        VARCHAR2(500),
    DATEGENERATIONRAPPORT   DATE,
    CONSTRAINT FK_PROD_STOCK FOREIGN KEY (ID_STOCK) REFERENCES STOCK(ID_STOCK),
    CONSTRAINT FK_PROD_MACHINE FOREIGN KEY (ID_MACHINE) REFERENCES MACHINE(ID_MACHINE)
);

-- Trigger auto-increment PRODUCTION
CREATE OR REPLACE TRIGGER TRG_PRODUCTION_ID
BEFORE INSERT ON PRODUCTION
FOR EACH ROW
BEGIN
    IF :NEW.IDPRODUCTION IS NULL THEN
        SELECT SEQ_PRODUCTION.NEXTVAL INTO :NEW.IDPRODUCTION FROM DUAL;
    END IF;
END;
/

-- ============================================================================
-- 10. Donnees de test MACHINE
-- ============================================================================
INSERT INTO MACHINE (NOM_MACHINE, TYPE_MACHINE, ETAT_MARCHE, TEMPERATURE_ACTUELLE, NIVEAU_CHARGE, ETAT_FONCTIONNEMENT, TYPE_ALERTE, NIVEAU_CRITICITE, DATE_DERNIERE_MAINTENANCE, DATE_INSTALLATION, SCORE_SANTE)
VALUES ('Presse Hydraulique A1', 'Presse', 'ON', 45.50, 72.00, 'Normal', 'Aucune', 'Faible', SYSDATE - 30, SYSDATE - 365, 92);

INSERT INTO MACHINE (NOM_MACHINE, TYPE_MACHINE, ETAT_MARCHE, TEMPERATURE_ACTUELLE, NIVEAU_CHARGE, ETAT_FONCTIONNEMENT, TYPE_ALERTE, NIVEAU_CRITICITE, DATE_DERNIERE_MAINTENANCE, DATE_INSTALLATION, SCORE_SANTE)
VALUES ('Malaxeur B2', 'Malaxeur', 'ON', 38.00, 55.00, 'Normal', 'Aucune', 'Faible', SYSDATE - 15, SYSDATE - 200, 88);

INSERT INTO MACHINE (NOM_MACHINE, TYPE_MACHINE, ETAT_MARCHE, TEMPERATURE_ACTUELLE, NIVEAU_CHARGE, ETAT_FONCTIONNEMENT, TYPE_ALERTE, NIVEAU_CRITICITE, DATE_DERNIERE_MAINTENANCE, DATE_INSTALLATION, SCORE_SANTE)
VALUES ('Broyeur C3', 'Broyeur', 'OFF', 22.00, 0.00, 'Panne', 'Panne', 'Critique', SYSDATE - 90, SYSDATE - 500, 35);

INSERT INTO MACHINE (NOM_MACHINE, TYPE_MACHINE, ETAT_MARCHE, TEMPERATURE_ACTUELLE, NIVEAU_CHARGE, ETAT_FONCTIONNEMENT, TYPE_ALERTE, NIVEAU_CRITICITE, DATE_DERNIERE_MAINTENANCE, DATE_INSTALLATION, SCORE_SANTE)
VALUES ('Chauffeur D4', 'Chauffeur', 'VEILLE', 60.00, 30.00, 'Alerte', 'Surcharge', 'Eleve', SYSDATE - 7, SYSDATE - 150, 65);

INSERT INTO MACHINE (NOM_MACHINE, TYPE_MACHINE, ETAT_MARCHE, TEMPERATURE_ACTUELLE, NIVEAU_CHARGE, ETAT_FONCTIONNEMENT, TYPE_ALERTE, NIVEAU_CRITICITE, DATE_DERNIERE_MAINTENANCE, DATE_INSTALLATION, SCORE_SANTE)
VALUES ('Presse Hydraulique A5', 'Presse', 'ON', 50.00, 85.00, 'Normal', 'Aucune', 'Faible', SYSDATE - 10, SYSDATE - 730, 78);

-- ============================================================================
-- 11. Donnees de test STOCK
-- ============================================================================
INSERT INTO STOCK (TYPE_HUILE, QUANTITE_ACTUELLE, SEUIL_ALERTE, EMPLACEMENT_STOCKAGE, ETAT_STOCK)
VALUES ('Huile Extra Vierge', 500.00, 100, 'Entrepot A - Zone 1', 'Disponible');

INSERT INTO STOCK (TYPE_HUILE, QUANTITE_ACTUELLE, SEUIL_ALERTE, EMPLACEMENT_STOCKAGE, ETAT_STOCK)
VALUES ('Huile Vierge', 200.00, 50, 'Entrepot A - Zone 2', 'Disponible');

INSERT INTO STOCK (TYPE_HUILE, QUANTITE_ACTUELLE, SEUIL_ALERTE, EMPLACEMENT_STOCKAGE, ETAT_STOCK)
VALUES ('Huile Lampante', 30.00, 50, 'Entrepot B - Zone 1', 'Faible');

-- ============================================================================
-- 12. Donnees de test PRODUCTION
-- ============================================================================
INSERT INTO PRODUCTION (DATEPRODUCTION, QUANTITEOLIVESKG, TYPEPRESSAGE, DUREEESTIMEE, TEMPSECOULE, STATUT, HUILEPRODUITEL, RENDEMENT, CONFORMENORMES)
VALUES (SYSDATE, 1000, 'Pression a froid', 120, 110, 'Termine', 180.50, 18.05, 1);

INSERT INTO PRODUCTION (DATEPRODUCTION, QUANTITEOLIVESKG, TYPEPRESSAGE, DUREEESTIMEE, TEMPSECOULE, STATUT, HUILEPRODUITEL, RENDEMENT, CONFORMENORMES)
VALUES (SYSDATE, 500, 'Centrifugation', 90, 85, 'Termine', 85.00, 17.00, 1);

INSERT INTO PRODUCTION (DATEPRODUCTION, QUANTITEOLIVESKG, TYPEPRESSAGE, DUREEESTIMEE, TEMPSECOULE, STATUT, HUILEPRODUITEL, RENDEMENT, CONFORMENORMES)
VALUES (SYSDATE, 750, 'Pression a froid', 100, 0, 'En cours', 0, 0, 0);

COMMIT;

-- ============================================================================
-- 13. Donnees de test CLIENT
-- ============================================================================
INSERT INTO CLIENT (NOM, PRENOM, TELEPHONE, ADRESSE, EMAIL, TYPE_CLIENT, TOTAL_OLIVES_LIVREES, DATE_CREATION, STATUT)
VALUES ('Ben Ali', 'Mohamed', '71234567', 'Tunis, Rue de la Liberte', 'mohamed.benali@email.com', 'Particulier', 3500, SYSDATE, 'Standard');

INSERT INTO CLIENT (NOM, PRENOM, TELEPHONE, ADRESSE, EMAIL, TYPE_CLIENT, TOTAL_OLIVES_LIVREES, DATE_CREATION, STATUT)
VALUES ('Trabelsi', 'Fatma', '98765432', 'Sfax, Avenue Bourguiba', 'fatma.trabelsi@email.com', 'Societe', 12000, SYSDATE, 'Important');

INSERT INTO CLIENT (NOM, PRENOM, TELEPHONE, ADRESSE, EMAIL, TYPE_CLIENT, TOTAL_OLIVES_LIVREES, DATE_CREATION, STATUT)
VALUES ('Chaabane', 'Ahmed', '55123456', 'Sousse, Rue Ibn Khaldoun', 'ahmed.chaabane@email.com', 'Particulier', 800, SYSDATE, 'Standard');

INSERT INTO CLIENT (NOM, PRENOM, TELEPHONE, ADRESSE, EMAIL, TYPE_CLIENT, TOTAL_OLIVES_LIVREES, DATE_CREATION, STATUT)
VALUES ('Mejri', 'Amira', '22334455', 'Bizerte, Centre ville', 'amira.mejri@email.com', 'Societe', 7500, SYSDATE, 'Important');

INSERT INTO CLIENT (NOM, PRENOM, TELEPHONE, ADRESSE, EMAIL, TYPE_CLIENT, TOTAL_OLIVES_LIVREES, DATE_CREATION, STATUT)
VALUES ('Khemiri', 'Youssef', '99887766', 'Kairouan, Zone industrielle', 'youssef.khemiri@email.com', 'Particulier', 2200, SYSDATE, 'Standard');

COMMIT;

-- ============================================================================
-- 14. Verification
-- ============================================================================
SELECT 'CLIENT' AS TABLE_NAME, COUNT(*) AS NB_ROWS FROM CLIENT
UNION ALL
SELECT 'STOCK', COUNT(*) FROM STOCK
UNION ALL
SELECT 'MACHINE', COUNT(*) FROM MACHINE
UNION ALL
SELECT 'PRODUCTION', COUNT(*) FROM PRODUCTION;

-- ============================================================================
-- FIN DU SCRIPT
-- ============================================================================
