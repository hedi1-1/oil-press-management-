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
  IF LOWER(TRIM(NVL(:NEW.PRIORITE, ''))) LIKE '%basse%'
     OR LOWER(TRIM(NVL(:NEW.PRIORITE, ''))) LIKE '%faible%'
     OR LOWER(TRIM(NVL(:NEW.PRIORITE, ''))) LIKE '%low%' THEN
    :NEW.TAG := '-';
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
