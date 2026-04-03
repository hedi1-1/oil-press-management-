-- Migration: associer une machine a un employe responsable
-- A executer une seule fois sur la base existante.

BEGIN
  EXECUTE IMMEDIATE 'ALTER TABLE MACHINE ADD (ID_USER NUMBER)';
EXCEPTION
  WHEN OTHERS THEN
    IF SQLCODE != -1430 THEN -- ORA-01430: column being added already exists
      RAISE;
    END IF;
END;
/

BEGIN
  EXECUTE IMMEDIATE 'ALTER TABLE MACHINE ADD CONSTRAINT FK_MACHINE_EMPLOYEE FOREIGN KEY (ID_USER) REFERENCES EMPLOYEES(USER_ID)';
EXCEPTION
  WHEN OTHERS THEN
    IF SQLCODE != -2261 THEN -- ORA-02261: such unique or primary key already exists in the table
      -- If constraint already exists with another name, ignore common duplicate cases
      IF SQLCODE != -2275 THEN -- ORA-02275: such a referential constraint already exists
        RAISE;
      END IF;
    END IF;
END;
/

COMMIT;
