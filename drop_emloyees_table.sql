-- Drops EMLOYEES if it exists (Oracle)
BEGIN
    EXECUTE IMMEDIATE 'DROP TABLE EMLOYEES CASCADE CONSTRAINTS PURGE';
EXCEPTION
    WHEN OTHERS THEN
        -- ORA-00942: table or view does not exist
        IF SQLCODE != -942 THEN
            RAISE;
        END IF;
END;
/
