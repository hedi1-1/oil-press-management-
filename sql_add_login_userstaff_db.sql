-- ============================================================================
-- Add only Login/Userstaff database objects on top of the base old database
-- Run as user ZITOUNA after create_zitouna_db.sql
-- ============================================================================

ALTER SESSION SET CURRENT_SCHEMA = ZITOUNA;

-- ---------------------------------------------------------------------------
-- 1) Ensure EMPLOYEES has required auth/account columns
-- ---------------------------------------------------------------------------
DECLARE
    v_count NUMBER;
BEGIN
    SELECT COUNT(*) INTO v_count FROM USER_TAB_COLUMNS
    WHERE TABLE_NAME = 'EMPLOYEES' AND COLUMN_NAME = 'STATE';
    IF v_count = 0 THEN
        EXECUTE IMMEDIATE 'ALTER TABLE EMPLOYEES ADD (STATE VARCHAR2(20) DEFAULT ''ACTIVE'')';
    END IF;

    SELECT COUNT(*) INTO v_count FROM USER_TAB_COLUMNS
    WHERE TABLE_NAME = 'EMPLOYEES' AND COLUMN_NAME = 'FAILED_ATTEMPTS';
    IF v_count = 0 THEN
        EXECUTE IMMEDIATE 'ALTER TABLE EMPLOYEES ADD (FAILED_ATTEMPTS NUMBER DEFAULT 0)';
    END IF;

    SELECT COUNT(*) INTO v_count FROM USER_TAB_COLUMNS
    WHERE TABLE_NAME = 'EMPLOYEES' AND COLUMN_NAME = 'LAST_LOGIN';
    IF v_count = 0 THEN
        EXECUTE IMMEDIATE 'ALTER TABLE EMPLOYEES ADD (LAST_LOGIN TIMESTAMP)';
    END IF;

    SELECT COUNT(*) INTO v_count FROM USER_TAB_COLUMNS
    WHERE TABLE_NAME = 'EMPLOYEES' AND COLUMN_NAME = 'CREATED_AT';
    IF v_count = 0 THEN
        EXECUTE IMMEDIATE 'ALTER TABLE EMPLOYEES ADD (CREATED_AT TIMESTAMP DEFAULT CURRENT_TIMESTAMP)';
    END IF;

    SELECT COUNT(*) INTO v_count FROM USER_TAB_COLUMNS
    WHERE TABLE_NAME = 'EMPLOYEES' AND COLUMN_NAME = 'LAST_ACTION';
    IF v_count = 0 THEN
        EXECUTE IMMEDIATE 'ALTER TABLE EMPLOYEES ADD (LAST_ACTION VARCHAR2(100))';
    END IF;

    SELECT COUNT(*) INTO v_count FROM USER_TAB_COLUMNS
    WHERE TABLE_NAME = 'EMPLOYEES' AND COLUMN_NAME = 'LAST_ACTION_TIME';
    IF v_count = 0 THEN
        EXECUTE IMMEDIATE 'ALTER TABLE EMPLOYEES ADD (LAST_ACTION_TIME TIMESTAMP)';
    END IF;
END;
/

-- ---------------------------------------------------------------------------
-- 2) Create AUDIT_LOG used by Login/Userstaff (if missing)
-- ---------------------------------------------------------------------------
DECLARE
    v_count NUMBER;
BEGIN
    SELECT COUNT(*) INTO v_count FROM USER_SEQUENCES WHERE SEQUENCE_NAME = 'SEQ_AUDIT';
    IF v_count = 0 THEN
        EXECUTE IMMEDIATE 'CREATE SEQUENCE SEQ_AUDIT START WITH 1 INCREMENT BY 1 NOCACHE';
    END IF;
END;
/

DECLARE
    v_count NUMBER;
BEGIN
    SELECT COUNT(*) INTO v_count FROM USER_TABLES WHERE TABLE_NAME = 'AUDIT_LOG';
    IF v_count = 0 THEN
        EXECUTE IMMEDIATE q'[
            CREATE TABLE AUDIT_LOG (
                LOG_ID          NUMBER PRIMARY KEY,
                LOG_TIME        TIMESTAMP DEFAULT SYSTIMESTAMP,
                USERNAME        VARCHAR2(100),
                EVENT_TYPE      VARCHAR2(50),
                DETAILS         VARCHAR2(500),
                PERFORMED_BY    VARCHAR2(100),
                HOSTNAME        VARCHAR2(200)
            )
        ]';
    END IF;
END;
/

CREATE OR REPLACE TRIGGER AUDIT_LOG_BI
BEFORE INSERT ON AUDIT_LOG
FOR EACH ROW
BEGIN
    IF :NEW.LOG_ID IS NULL THEN
        SELECT SEQ_AUDIT.NEXTVAL INTO :NEW.LOG_ID FROM DUAL;
    END IF;
END;
/

-- ---------------------------------------------------------------------------
-- 3) Create PASSWORD_RESET_REQUESTS used by Login/Userstaff (if missing)
-- ---------------------------------------------------------------------------
DECLARE
    v_count NUMBER;
BEGIN
    SELECT COUNT(*) INTO v_count FROM USER_SEQUENCES WHERE SEQUENCE_NAME = 'SEQ_PWD_RESET';
    IF v_count = 0 THEN
        EXECUTE IMMEDIATE 'CREATE SEQUENCE SEQ_PWD_RESET START WITH 1 INCREMENT BY 1 NOCACHE';
    END IF;
END;
/

DECLARE
    v_count NUMBER;
BEGIN
    SELECT COUNT(*) INTO v_count FROM USER_TABLES WHERE TABLE_NAME = 'PASSWORD_RESET_REQUESTS';
    IF v_count = 0 THEN
        EXECUTE IMMEDIATE q'[
            CREATE TABLE PASSWORD_RESET_REQUESTS (
                REQUEST_ID       NUMBER PRIMARY KEY,
                USER_ID          NUMBER,
                USERNAME         VARCHAR2(100) NOT NULL,
                EMAIL            VARCHAR2(150),
                MESSAGE          VARCHAR2(500),
                REQUEST_TIME     TIMESTAMP DEFAULT SYSTIMESTAMP,
                STATUS           VARCHAR2(20) DEFAULT 'PENDING',
                RESOLVED_BY      VARCHAR2(100),
                RESOLVED_TIME    TIMESTAMP,3
                NEW_PASSWORD_SET NUMBER(1) DEFAULT 0,
                CONSTRAINT FK_PWDRESET_EMP FOREIGN KEY (USER_ID)
                    REFERENCES EMPLOYEES(USER_ID) ON DELETE CASCADE
            )
        ]';
    END IF;
END;
/

CREATE OR REPLACE TRIGGER PWD_RESET_BI
BEFORE INSERT ON PASSWORD_RESET_REQUESTS
FOR EACH ROW
BEGIN
    IF :NEW.REQUEST_ID IS NULL THEN
        SELECT SEQ_PWD_RESET.NEXTVAL INTO :NEW.REQUEST_ID FROM DUAL;
    END IF;
END;
/

COMMIT;

PROMPT Login/Userstaff database patch applied.
