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
BEGIN EXECUTE IMMEDIATE 'DROP TABLE PRODUCTION CASCADE CONSTRAINTS'; EXCEPTION WHEN OTHERS THEN NULL; END;
/
BEGIN EXECUTE IMMEDIATE 'DROP TABLE STOCK CASCADE CONSTRAINTS'; EXCEPTION WHEN OTHERS THEN NULL; END;
/
BEGIN EXECUTE IMMEDIATE 'DROP TABLE CLIENT CASCADE CONSTRAINTS'; EXCEPTION WHEN OTHERS THEN NULL; END;
/
BEGIN EXECUTE IMMEDIATE 'DROP SEQUENCE SEQ_PRODUCTION'; EXCEPTION WHEN OTHERS THEN NULL; END;
/
BEGIN EXECUTE IMMEDIATE 'DROP SEQUENCE SEQ_STOCK'; EXCEPTION WHEN OTHERS THEN NULL; END;
/
BEGIN EXECUTE IMMEDIATE 'DROP SEQUENCE SEQ_CLIENT'; EXCEPTION WHEN OTHERS THEN NULL; END;
/

-- ============================================================================
-- 2. Sequence CLIENT
-- ============================================================================
CREATE SEQUENCE SEQ_CLIENT START WITH 1 INCREMENT BY 1;

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
-- 2. Sequence STOCK
-- ============================================================================
CREATE SEQUENCE SEQ_STOCK START WITH 1 INCREMENT BY 1;

-- ============================================================================
-- 3. Table STOCK
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
-- 4. Sequence PRODUCTION
-- ============================================================================
CREATE SEQUENCE SEQ_PRODUCTION START WITH 1 INCREMENT BY 1 NOCACHE NOCYCLE;

-- ============================================================================
-- 5. Table PRODUCTION
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
    REMARQUESQUALITE        VARCHAR2(500),
    DATEGENERATIONRAPPORT   DATE,
    CONSTRAINT FK_PROD_STOCK FOREIGN KEY (ID_STOCK) REFERENCES STOCK(ID_STOCK)
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
-- 6. Donnees de test STOCK
-- ============================================================================
INSERT INTO STOCK (TYPE_HUILE, QUANTITE_ACTUELLE, SEUIL_ALERTE, EMPLACEMENT_STOCKAGE, ETAT_STOCK)
VALUES ('Huile Extra Vierge', 500.00, 100, 'Entrepot A - Zone 1', 'Disponible');

INSERT INTO STOCK (TYPE_HUILE, QUANTITE_ACTUELLE, SEUIL_ALERTE, EMPLACEMENT_STOCKAGE, ETAT_STOCK)
VALUES ('Huile Vierge', 200.00, 50, 'Entrepot A - Zone 2', 'Disponible');

INSERT INTO STOCK (TYPE_HUILE, QUANTITE_ACTUELLE, SEUIL_ALERTE, EMPLACEMENT_STOCKAGE, ETAT_STOCK)
VALUES ('Huile Lampante', 30.00, 50, 'Entrepot B - Zone 1', 'Faible');

-- ============================================================================
-- 7. Donnees de test PRODUCTION
-- ============================================================================
INSERT INTO PRODUCTION (DATEPRODUCTION, QUANTITEOLIVESKG, TYPEPRESSAGE, DUREEESTIMEE, TEMPSECOULE, STATUT, HUILEPRODUITEL, RENDEMENT, CONFORMENORMES)
VALUES (SYSDATE, 1000, 'Pression a froid', 120, 110, 'Termine', 180.50, 18.05, 1);

INSERT INTO PRODUCTION (DATEPRODUCTION, QUANTITEOLIVESKG, TYPEPRESSAGE, DUREEESTIMEE, TEMPSECOULE, STATUT, HUILEPRODUITEL, RENDEMENT, CONFORMENORMES)
VALUES (SYSDATE, 500, 'Centrifugation', 90, 85, 'Termine', 85.00, 17.00, 1);

INSERT INTO PRODUCTION (DATEPRODUCTION, QUANTITEOLIVESKG, TYPEPRESSAGE, DUREEESTIMEE, TEMPSECOULE, STATUT, HUILEPRODUITEL, RENDEMENT, CONFORMENORMES)
VALUES (SYSDATE, 750, 'Pression a froid', 100, 0, 'En cours', 0, 0, 0);

COMMIT;

-- ============================================================================
-- 9. Donnees de test CLIENT
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
-- 10. Verification
-- ============================================================================
SELECT 'CLIENT' AS TABLE_NAME, COUNT(*) AS NB_ROWS FROM CLIENT
UNION ALL
SELECT 'STOCK', COUNT(*) FROM STOCK
UNION ALL
SELECT 'PRODUCTION', COUNT(*) FROM PRODUCTION;

-- ============================================================================
-- FIN DU SCRIPT
-- ============================================================================
