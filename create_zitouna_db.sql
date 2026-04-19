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
BEGIN EXECUTE IMMEDIATE 'DROP TABLE CLIENT_HISTORIQUE CASCADE CONSTRAINTS'; EXCEPTION WHEN OTHERS THEN NULL; END;
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
BEGIN EXECUTE IMMEDIATE 'DROP SEQUENCE SEQ_CLIENT_HIST'; EXCEPTION WHEN OTHERS THEN NULL; END;
/
BEGIN EXECUTE IMMEDIATE 'DROP SEQUENCE SEQ_MACHINE'; EXCEPTION WHEN OTHERS THEN NULL; END;
/
BEGIN EXECUTE IMMEDIATE 'DROP SEQUENCE USERS_SEQ'; EXCEPTION WHEN OTHERS THEN NULL; END;
/
BEGIN EXECUTE IMMEDIATE 'DROP TRIGGER TRG_FINANCE_ID'; EXCEPTION WHEN OTHERS THEN NULL; END;
/
BEGIN EXECUTE IMMEDIATE 'DROP TRIGGER TRG_FINANCE_AUDIT'; EXCEPTION WHEN OTHERS THEN NULL; END;
/
BEGIN EXECUTE IMMEDIATE 'DROP TABLE FINANCE CASCADE CONSTRAINTS'; EXCEPTION WHEN OTHERS THEN NULL; END;
/
BEGIN EXECUTE IMMEDIATE 'DROP SEQUENCE SEQ_FINANCE'; EXCEPTION WHEN OTHERS THEN NULL; END;
/
BEGIN EXECUTE IMMEDIATE 'DROP FUNCTION CALC_IMPACT'; EXCEPTION WHEN OTHERS THEN NULL; END;
/
BEGIN EXECUTE IMMEDIATE 'DROP FUNCTION IS_ANOMALY_FUNC'; EXCEPTION WHEN OTHERS THEN NULL; END;
/
BEGIN EXECUTE IMMEDIATE 'DROP FUNCTION CALC_SEVERITY'; EXCEPTION WHEN OTHERS THEN NULL; END;
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
    STATUT                  VARCHAR2(50) DEFAULT 'Standard',
    LAST_ACTION             VARCHAR2(100),
    LAST_ACTION_TIME        TIMESTAMP,
    LAST_ACTION_DETAILS     VARCHAR2(1000)
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
    ETAT_STOCK                  VARCHAR2(50) DEFAULT 'Disponible',
    SOURCE_DONNEE               VARCHAR2(30) DEFAULT 'MANUEL' NOT NULL,
    ARDUINO_STATUS              VARCHAR2(30),
    ARDUINO_STOCK_ID            NUMBER,
    ARDUINO_PORT                VARCHAR2(30),
    ARDUINO_DERNIERE_LECTURE    DATE,
    CONSTRAINT CHK_STOCK_SOURCE CHECK (SOURCE_DONNEE IN ('MANUEL', 'ARDUINO')),
    CONSTRAINT CHK_STOCK_ARDUINO_STATUS CHECK (ARDUINO_STATUS IS NULL OR ARDUINO_STATUS IN ('REMPLISSAGE', 'STABLE', 'FAIBLE', 'VALIDATED', 'FAIBLE_CONFIRMED'))
);

CREATE INDEX IDX_STOCK_SOURCE ON STOCK(SOURCE_DONNEE);
CREATE INDEX IDX_STOCK_ARDUINO_STATUS ON STOCK(ARDUINO_STATUS);
CREATE UNIQUE INDEX UQ_STOCK_ARDUINO_ROW ON STOCK (
    CASE WHEN TYPE_HUILE = 'MESURE_ARDUINO' AND SOURCE_DONNEE = 'ARDUINO' THEN 1 ELSE NULL END
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
    ID_USER                     NUMBER,
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
    DATE_MISE_A_JOUR            TIMESTAMP DEFAULT SYSTIMESTAMP,
    CONSTRAINT FK_MACHINE_EMPLOYEE FOREIGN KEY (ID_USER) REFERENCES EMPLOYEES(USER_ID)
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

INSERT INTO STOCK (TYPE_HUILE, QUANTITE_ACTUELLE, SEUIL_ALERTE, EMPLACEMENT_STOCKAGE, ETAT_STOCK, SOURCE_DONNEE, ARDUINO_STATUS, ARDUINO_STOCK_ID, ARDUINO_PORT, ARDUINO_DERNIERE_LECTURE)
VALUES ('MESURE_ARDUINO', 0.00, 5, 'CUVE ARDUINO', 'En commande', 'ARDUINO', 'REMPLISSAGE', NULL, 'N/A', SYSDATE);

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
-- 14. Sequence FINANCE
-- ============================================================================
CREATE SEQUENCE SEQ_FINANCE START WITH 1 INCREMENT BY 1 NOCACHE;

-- ============================================================================
-- 15. Table FINANCE (Transactions & Historique Audit Unifies en UNE SEULE table)
-- ============================================================================
CREATE TABLE FINANCE (
    ID_TRANSACTION          NUMBER PRIMARY KEY,
    
    -- Colonnes pour une TRANSACTION NORMALE
    ID_EMPLOYEE             NUMBER,
    CTYPE                   VARCHAR2(20) CHECK (UPPER(CTYPE) IN ('REVENU', 'DÉPENSE', 'DEPENSE')),
    CATEGORIE               VARCHAR2(100),
    MONTANT                 NUMBER(15,2),
    DATE_TRANS              TIMESTAMP DEFAULT SYSTIMESTAMP,
    DESCRIPTION             VARCHAR2(500),
    ID_MACHINE              NUMBER,
    CREATED_AT              TIMESTAMP DEFAULT SYSTIMESTAMP,
    UPDATED_AT              TIMESTAMP DEFAULT SYSTIMESTAMP,
    
    -- Colonnes pour l'AUDIT (AUTO-REFERENCE dans la meme table)
    TYPE_LIGNE              VARCHAR2(20) DEFAULT 'TRANSACTION' CHECK (TYPE_LIGNE IN ('TRANSACTION', 'AUDIT')),
    PARENT_TRANSACTION_ID   NUMBER,
    AUDIT_ACTION            VARCHAR2(20) CHECK (UPPER(AUDIT_ACTION) IN ('INSERT', 'UPDATE', 'DELETE')),
    AUDIT_CHAMP             VARCHAR2(100),
    AUDIT_OLD_VALUE         CLOB,
    AUDIT_NEW_VALUE         CLOB,
    AUDIT_OPERATION_TYPE    VARCHAR2(100),
    AUDIT_SEVERITE          VARCHAR2(20) DEFAULT 'LOW',
    AUDIT_EST_ANOMALIE      NUMBER(1) DEFAULT 0,
    AUDIT_IMPACT_SCORE      NUMBER(5,2) DEFAULT 0,
    AUDIT_VERSION           NUMBER DEFAULT 1,
    
    CONSTRAINT FK_FINANCE_EMPLOYEE FOREIGN KEY (ID_EMPLOYEE) REFERENCES EMPLOYEES(USER_ID),
    CONSTRAINT FK_FINANCE_MACHINE FOREIGN KEY (ID_MACHINE) REFERENCES MACHINE(ID_MACHINE)
);

-- Index pour performance
CREATE INDEX IDX_FINANCE_TYPE ON FINANCE(TYPE_LIGNE);
CREATE INDEX IDX_FINANCE_PARENT ON FINANCE(PARENT_TRANSACTION_ID);
CREATE INDEX IDX_FINANCE_DATE ON FINANCE(CREATED_AT);

-- Trigger auto-increment FINANCE
CREATE OR REPLACE TRIGGER TRG_FINANCE_ID
BEFORE INSERT ON FINANCE
FOR EACH ROW
BEGIN
    IF :NEW.ID_TRANSACTION IS NULL THEN
        SELECT SEQ_FINANCE.NEXTVAL INTO :NEW.ID_TRANSACTION FROM DUAL;
    END IF;
END;
/

-- ============================================================================
-- 16. Fonctions de detection et statistiques (Impact, Anomalie, Sévérité)
-- ============================================================================
CREATE OR REPLACE FUNCTION CALC_IMPACT(p_old_val NUMBER, p_new_val NUMBER) 
RETURN NUMBER IS
    v_impact NUMBER;
BEGIN
    IF p_old_val = 0 OR p_old_val IS NULL THEN
        v_impact := 100;
    ELSE
        v_impact := ABS((p_new_val - p_old_val) / p_old_val) * 100;
    END IF;
    RETURN v_impact;
END CALC_IMPACT;
/

CREATE OR REPLACE FUNCTION IS_ANOMALY_FUNC(p_action VARCHAR2, p_impact NUMBER, p_montant NUMBER) 
RETURN NUMBER IS
    v_is_anomaly NUMBER := 0;
BEGIN
    IF UPPER(p_action) = 'DELETE' THEN v_is_anomaly := 1;
    ELSIF UPPER(p_action) = 'UPDATE' AND p_impact > 50 THEN v_is_anomaly := 1;
    ELSIF p_montant > 15000 THEN v_is_anomaly := 1;
    END IF;
    RETURN v_is_anomaly;
END IS_ANOMALY_FUNC;
/

CREATE OR REPLACE FUNCTION CALC_SEVERITY(p_action VARCHAR2, p_montant NUMBER, p_impact NUMBER) 
RETURN VARCHAR2 IS
    v_severity VARCHAR2(20) := 'LOW';
BEGIN
    IF UPPER(p_action) = 'DELETE' THEN v_severity := 'CRITICAL';
    ELSIF p_montant > 15000 THEN v_severity := 'CRITICAL';
    ELSIF p_montant > 10000 OR p_impact > 50 THEN v_severity := 'HIGH';
    ELSIF p_montant > 5000 OR p_impact > 25 THEN v_severity := 'MEDIUM';
    ELSE v_severity := 'LOW';
    END IF;
    RETURN v_severity;
END CALC_SEVERITY;
/

-- ============================================================================
-- 17. Trigger COMPOUND sur FINANCE (Genere l'historique automatiquement)
-- ============================================================================
CREATE OR REPLACE TRIGGER TRG_FINANCE_AUDIT
FOR INSERT OR UPDATE OR DELETE ON FINANCE
COMPOUND TRIGGER

    TYPE t_audit_rec IS RECORD (
        action VARCHAR2(20),
        parent_id NUMBER,
        champ VARCHAR2(100),
        old_val VARCHAR2(4000),
        new_val VARCHAR2(4000),
        op_type VARCHAR2(100),
        emp_id NUMBER,
        sev VARCHAR2(20),
        anom NUMBER,
        imp NUMBER
    );
    TYPE t_audit_tab IS TABLE OF t_audit_rec INDEX BY PLS_INTEGER;
    v_audits t_audit_tab;
    v_idx PLS_INTEGER := 0;

    PROCEDURE add_audit(
        p_action VARCHAR2, p_parent NUMBER, p_champ VARCHAR2, p_old VARCHAR2, p_new VARCHAR2,
        p_op VARCHAR2, p_emp NUMBER, p_sev VARCHAR2, p_anom NUMBER, p_imp NUMBER
    ) IS
    BEGIN
        v_idx := v_idx + 1;
        v_audits(v_idx).action := p_action;
        v_audits(v_idx).parent_id := p_parent;
        v_audits(v_idx).champ := p_champ;
        v_audits(v_idx).old_val := p_old;
        v_audits(v_idx).new_val := p_new;
        v_audits(v_idx).op_type := p_op;
        v_audits(v_idx).emp_id := p_emp;
        v_audits(v_idx).sev := p_sev;
        v_audits(v_idx).anom := p_anom;
        v_audits(v_idx).imp := p_imp;
    END add_audit;

    AFTER EACH ROW IS
        v_impact NUMBER;
        v_sev VARCHAR2(20);
        v_anom NUMBER;
    BEGIN
        IF INSERTING AND :NEW.TYPE_LIGNE = 'TRANSACTION' THEN
            add_audit('INSERT', :NEW.ID_TRANSACTION, 'MONTANT', NULL, TO_CHAR(:NEW.MONTANT),
                      'TRANSACTION_CREATED', :NEW.ID_EMPLOYEE, CALC_SEVERITY('INSERT', :NEW.MONTANT, 0),
                      IS_ANOMALY_FUNC('INSERT', 0, :NEW.MONTANT), 0);
            
            add_audit('INSERT', :NEW.ID_TRANSACTION, 'CATEGORIE', NULL, :NEW.CATEGORIE,
                      'CATEGORY_SET', :NEW.ID_EMPLOYEE, CALC_SEVERITY('INSERT', :NEW.MONTANT, 0),
                      IS_ANOMALY_FUNC('INSERT', 0, :NEW.MONTANT), 0);
                      
        ELSIF UPDATING AND :NEW.TYPE_LIGNE = 'TRANSACTION' THEN
            IF :NEW.MONTANT != :OLD.MONTANT THEN
                v_impact := CALC_IMPACT(:OLD.MONTANT, :NEW.MONTANT);
                v_sev := CALC_SEVERITY('UPDATE', :NEW.MONTANT, v_impact);
                v_anom := IS_ANOMALY_FUNC('UPDATE', v_impact, :NEW.MONTANT);
                add_audit('UPDATE', :NEW.ID_TRANSACTION, 'MONTANT', TO_CHAR(:OLD.MONTANT), TO_CHAR(:NEW.MONTANT),
                          'MONTANT_CHANGE', :NEW.ID_EMPLOYEE, v_sev, v_anom, v_impact);
            END IF;
            
            IF :NEW.CATEGORIE != :OLD.CATEGORIE THEN
                add_audit('UPDATE', :NEW.ID_TRANSACTION, 'CATEGORIE', :OLD.CATEGORIE, :NEW.CATEGORIE,
                          'CATEGORIE_CHANGE', :NEW.ID_EMPLOYEE, 'MEDIUM', 0, 0);
            END IF;
            
            IF NVL(:NEW.DESCRIPTION, 'NULL') != NVL(:OLD.DESCRIPTION, 'NULL') THEN
                add_audit('UPDATE', :NEW.ID_TRANSACTION, 'DESCRIPTION', :OLD.DESCRIPTION, :NEW.DESCRIPTION,
                          'DESCRIPTION_CHANGE', :NEW.ID_EMPLOYEE, 'LOW', 0, 0);
            END IF;
            
        ELSIF DELETING AND :OLD.TYPE_LIGNE = 'TRANSACTION' THEN
            add_audit('DELETE', :OLD.ID_TRANSACTION, 'TRANSACTION', TO_CHAR(:OLD.MONTANT), NULL,
                      'TRANSACTION_DELETED', :OLD.ID_EMPLOYEE, 'CRITICAL', 1, 100);
        END IF;
    END AFTER EACH ROW;

    AFTER STATEMENT IS
        v_version NUMBER;
    BEGIN
        FOR i IN 1 .. v_idx LOOP
            SELECT NVL(MAX(AUDIT_VERSION), 0) + 1 INTO v_version 
            FROM FINANCE 
            WHERE PARENT_TRANSACTION_ID = v_audits(i).parent_id AND TYPE_LIGNE = 'AUDIT';
            
            INSERT INTO FINANCE (
                TYPE_LIGNE, PARENT_TRANSACTION_ID, AUDIT_ACTION, AUDIT_CHAMP, AUDIT_OLD_VALUE, AUDIT_NEW_VALUE,
                AUDIT_OPERATION_TYPE, ID_EMPLOYEE, AUDIT_SEVERITE, AUDIT_EST_ANOMALIE, AUDIT_IMPACT_SCORE, AUDIT_VERSION
            ) VALUES (
                'AUDIT', v_audits(i).parent_id, v_audits(i).action, v_audits(i).champ, v_audits(i).old_val, v_audits(i).new_val,
                v_audits(i).op_type, v_audits(i).emp_id, v_audits(i).sev, v_audits(i).anom, v_audits(i).imp, v_version
            );
        END LOOP;
    END AFTER STATEMENT;

END TRG_FINANCE_AUDIT;
/

-- ============================================================================
-- 18. Verification
-- ============================================================================
SELECT 'CLIENT' AS TABLE_NAME, COUNT(*) AS NB_ROWS FROM CLIENT
UNION ALL
SELECT 'STOCK', COUNT(*) FROM STOCK
UNION ALL
SELECT 'MACHINE', COUNT(*) FROM MACHINE
UNION ALL
SELECT 'PRODUCTION', COUNT(*) FROM PRODUCTION
UNION ALL
SELECT 'FINANCE', COUNT(*) FROM FINANCE;

-- ============================================================================
-- FIN DU SCRIPT
-- ============================================================================
