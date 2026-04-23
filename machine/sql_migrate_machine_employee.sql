-- Migration unique: MACHINE + regle TAG/PRIORITE
-- Règle métier demandée: si PRIORITE est Basse/Faible/Low alors TAG = '-'

-------------------------------------------------------------------------------
-- 1) Ajout colonne de liaison employé (si absente)
-------------------------------------------------------------------------------
BEGIN
  EXECUTE IMMEDIATE 'ALTER TABLE MACHINE ADD (ID_USER NUMBER)';
EXCEPTION
  WHEN OTHERS THEN
    IF SQLCODE != -1430 THEN -- ORA-01430: column already exists
      RAISE;
    END IF;
END;
/

BEGIN
  EXECUTE IMMEDIATE 'ALTER TABLE MACHINE ADD CONSTRAINT FK_MACHINE_EMPLOYEE FOREIGN KEY (ID_USER) REFERENCES EMPLOYEES(USER_ID)';
EXCEPTION
  WHEN OTHERS THEN
    IF SQLCODE != -2261 THEN -- ORA-02261
      IF SQLCODE != -2275 THEN -- ORA-02275
        RAISE;
      END IF;
    END IF;
END;
/

-------------------------------------------------------------------------------
-- 2) UPDATE one-shot: corrige immédiatement les lignes existantes
--    (pas seulement les futures insert/update)
-------------------------------------------------------------------------------
DECLARE
  v_rows_updated NUMBER := 0;
BEGIN
  UPDATE MACHINE
  SET TAG = '-'
  WHERE LOWER(TRIM(NVL(PRIORITE, ''))) LIKE '%basse%'
     OR LOWER(TRIM(NVL(PRIORITE, ''))) LIKE '%faible%'
     OR LOWER(TRIM(NVL(PRIORITE, ''))) LIKE '%low%';

  v_rows_updated := SQL%ROWCOUNT;
  DBMS_OUTPUT.PUT_LINE('Lignes mises a jour (TAG force a ''-''): ' || v_rows_updated);
END;
/

-------------------------------------------------------------------------------
-- 2.b) Supprimer les anciens triggers potentiellement conflictuels
--      (ex: ancien trigger qui mettait TAG='à vérifier')
-------------------------------------------------------------------------------
BEGIN
  EXECUTE IMMEDIATE 'DROP TRIGGER TRG_MACHINE_TAG_PRIORITE';
EXCEPTION
  WHEN OTHERS THEN
    IF SQLCODE != -4080 THEN -- ORA-04080: trigger does not exist
      RAISE;
    END IF;
END;
/

-- Verification rapide (optionnelle)
-- SELECT ID_MACHINE, NOM_MACHINE, PRIORITE, TAG
-- FROM MACHINE
-- WHERE LOWER(TRIM(NVL(PRIORITE, ''))) LIKE '%basse%'
--    OR LOWER(TRIM(NVL(PRIORITE, ''))) LIKE '%faible%'
--    OR LOWER(TRIM(NVL(PRIORITE, ''))) LIKE '%low%'
-- ORDER BY ID_MACHINE;

-------------------------------------------------------------------------------
-- 3) Trigger permanent: applique la regle sur les nouvelles écritures
-------------------------------------------------------------------------------
CREATE OR REPLACE TRIGGER TRG_MACHINE_TAG_LOW_PRIORITY
BEFORE INSERT OR UPDATE ON MACHINE
FOR EACH ROW
BEGIN
  -- PRIORITE automatique
  IF LOWER(TRIM(NVL(:NEW.NIVEAU_CRITICITE, ''))) = 'critique'
     OR LOWER(TRIM(NVL(:NEW.ETAT_FONCTIONNEMENT, ''))) = 'panne' THEN
    :NEW.PRIORITE := 'Haute';
  ELSIF LOWER(TRIM(NVL(:NEW.NIVEAU_CRITICITE, ''))) IN ('eleve', 'élevé', 'moyen')
        OR LOWER(TRIM(NVL(:NEW.ETAT_FONCTIONNEMENT, ''))) = 'alerte' THEN
    :NEW.PRIORITE := 'Moyenne';
  ELSE
    :NEW.PRIORITE := 'Basse';
  END IF;

  -- TAG automatique
  IF LOWER(TRIM(NVL(:NEW.PRIORITE, ''))) LIKE '%basse%'
     OR LOWER(TRIM(NVL(:NEW.PRIORITE, ''))) LIKE '%faible%'
     OR LOWER(TRIM(NVL(:NEW.PRIORITE, ''))) LIKE '%low%' THEN
    :NEW.TAG := '-';
  ELSIF NVL(:NEW.TEMPERATURE_ACTUELLE, 0) > 80
        OR (:NEW.DATE_DERNIERE_MAINTENANCE IS NOT NULL
            AND :NEW.DATE_DERNIERE_MAINTENANCE <= SYSDATE + 30) THEN
    :NEW.TAG := 'à vérifier';
  ELSIF :NEW.TAG IS NULL OR TRIM(:NEW.TAG) = '' THEN
    :NEW.TAG := '-';
  END IF;
END;
/

-------------------------------------------------------------------------------
-- 4) Correction finale de sécurité après création trigger
-------------------------------------------------------------------------------
UPDATE MACHINE
SET TAG = '-'
WHERE LOWER(TRIM(NVL(PRIORITE, ''))) LIKE '%basse%'
  OR LOWER(TRIM(NVL(PRIORITE, ''))) LIKE '%faible%'
  OR LOWER(TRIM(NVL(PRIORITE, ''))) LIKE '%low%';

COMMIT;

-------------------------------------------------------------------------------
-- 5) Configuration des seuils thermiques par type machine (Hypothèse C)
-------------------------------------------------------------------------------
BEGIN
  EXECUTE IMMEDIATE '
    CREATE TABLE MACHINE_THRESHOLDS (
      TYPE_MACHINE VARCHAR2(100) PRIMARY KEY,
      FAN_ON_TEMP NUMBER(5,2) NOT NULL,
      ALERT_TEMP NUMBER(5,2) NOT NULL,
      PANNE_TEMP NUMBER(5,2) NOT NULL,
      UPDATED_AT TIMESTAMP DEFAULT SYSTIMESTAMP,
      CONSTRAINT CHK_MACHINE_THRESHOLDS_RANGE
      CHECK (FAN_ON_TEMP < ALERT_TEMP AND ALERT_TEMP < PANNE_TEMP)
    )';
EXCEPTION
  WHEN OTHERS THEN
    IF SQLCODE != -955 THEN -- ORA-00955: name already used
      RAISE;
    END IF;
END;
/

MERGE INTO MACHINE_THRESHOLDS tgt
USING (
  SELECT 'Broyeur' AS TYPE_MACHINE, 30 AS FAN_ON_TEMP, 40 AS ALERT_TEMP, 55 AS PANNE_TEMP FROM dual
  UNION ALL SELECT 'Malaxeur', 45, 60, 75 FROM dual
  UNION ALL SELECT 'Presse', 45, 70, 85 FROM dual
  UNION ALL SELECT 'Chauffeur', 50, 80, 95 FROM dual
) src
ON (UPPER(TRIM(tgt.TYPE_MACHINE)) = UPPER(TRIM(src.TYPE_MACHINE)))
WHEN MATCHED THEN
  UPDATE SET tgt.FAN_ON_TEMP = src.FAN_ON_TEMP,
             tgt.ALERT_TEMP = src.ALERT_TEMP,
             tgt.PANNE_TEMP = src.PANNE_TEMP,
             tgt.UPDATED_AT = SYSTIMESTAMP
WHEN NOT MATCHED THEN
  INSERT (TYPE_MACHINE, FAN_ON_TEMP, ALERT_TEMP, PANNE_TEMP, UPDATED_AT)
  VALUES (src.TYPE_MACHINE, src.FAN_ON_TEMP, src.ALERT_TEMP, src.PANNE_TEMP, SYSTIMESTAMP);

COMMIT;
