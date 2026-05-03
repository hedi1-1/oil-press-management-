-- Set or create login user: jasser / jasser@gmail.com / jasser123
-- Password hash is SHA-256 of 'jasser123'

DECLARE
    v_password_hash VARCHAR2(64) := 'd2a825e6165088151f75b3b2d9cdb1bd00d02a3ca05580e8fbc52e2bcc2080b5';
BEGIN
    UPDATE EMPLOYEES
    SET USERNAME = 'jasser',
        EMAIL = 'jasser@gmail.com',
        PASSWORD_HASH = v_password_hash,
        STATE = 'ACTIVE',
        FAILED_ATTEMPTS = 0
    WHERE UPPER(USERNAME) = 'JASSER'
       OR UPPER(EMAIL) = 'JASSER@GMAIL.COM';

    IF SQL%ROWCOUNT = 0 THEN
        INSERT INTO EMPLOYEES (USERNAME, EMAIL, PASSWORD_HASH, ROLE, STATE, FAILED_ATTEMPTS)
        VALUES ('jasser', 'jasser@gmail.com', v_password_hash, 'ADMIN', 'ACTIVE', 0);
    END IF;

    COMMIT;
END;
/

-- Verification
SELECT USER_ID, USERNAME, EMAIL, ROLE, STATE, FAILED_ATTEMPTS
FROM EMPLOYEES
WHERE UPPER(USERNAME) = 'JASSER'
   OR UPPER(EMAIL) = 'JASSER@GMAIL.COM';
