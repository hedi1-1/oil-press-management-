-- ============================================================================
-- PressIQ one-time seed for Login + Userstaff testing (Oracle SQL Developer)
-- Uses existing tables only: EMPLOYEES, AUDIT_LOG, PASSWORD_RESET_REQUESTS
-- Safe to re-run thanks to MERGE statements.
-- ============================================================================

ALTER SESSION SET CURRENT_SCHEMA = ZITOUNA;

-- ----------------------------------------------------------------------------
-- Password hashes (SHA-256, same as app Login::hashPassword)
-- admin123 = 240be518fabd2724ddb6f04eeb1da5967448d7e831c08c8fa822809f74c720a9
-- user123  = e606e38b0d8c19b24cf0ee3808183162ea7cd63ff7912dbb22b5e803286b4446
-- lock123  = 926a7e47e168b5f4adc10bfeffc01a070837d73ccdbaaed8509ea31745775ff5
-- arch123  = 2cde40bc249d7716a0dbdd36dc6b404edd8f8526a42139e4fa2c6da809ade59d
-- ----------------------------------------------------------------------------

-- ----------------------------------------------------------------------------
-- 1) Upsert test employee accounts
-- ----------------------------------------------------------------------------
MERGE INTO EMPLOYEES e
USING (
    SELECT 'admin_pressiq' AS USERNAME,
           'admin@pressiq.tn' AS EMAIL,
           '240be518fabd2724ddb6f04eeb1da5967448d7e831c08c8fa822809f74c720a9' AS PASSWORD_HASH,
           'Admin' AS ROLE,
           'ACTIVE' AS STATE,
           0 AS FAILED_ATTEMPTS,
           'SEEDED' AS LAST_ACTION
    FROM DUAL
) s
ON (UPPER(e.USERNAME) = UPPER(s.USERNAME))
WHEN MATCHED THEN UPDATE SET
    e.EMAIL = s.EMAIL,
    e.PASSWORD_HASH = s.PASSWORD_HASH,
    e.ROLE = s.ROLE,
    e.STATE = s.STATE,
    e.FAILED_ATTEMPTS = s.FAILED_ATTEMPTS,
    e.LAST_ACTION = s.LAST_ACTION,
    e.LAST_ACTION_TIME = SYSTIMESTAMP
WHEN NOT MATCHED THEN INSERT
    (USERNAME, EMAIL, PASSWORD_HASH, ROLE, STATE, FAILED_ATTEMPTS, LAST_ACTION, LAST_ACTION_TIME)
VALUES
    (s.USERNAME, s.EMAIL, s.PASSWORD_HASH, s.ROLE, s.STATE, s.FAILED_ATTEMPTS, s.LAST_ACTION, SYSTIMESTAMP);

MERGE INTO EMPLOYEES e
USING (
    SELECT 'user_pressiq' AS USERNAME,
           'user@pressiq.tn' AS EMAIL,
           'e606e38b0d8c19b24cf0ee3808183162ea7cd63ff7912dbb22b5e803286b4446' AS PASSWORD_HASH,
           'Operateur' AS ROLE,
           'ACTIVE' AS STATE,
           0 AS FAILED_ATTEMPTS,
           'SEEDED' AS LAST_ACTION
    FROM DUAL
) s
ON (UPPER(e.USERNAME) = UPPER(s.USERNAME))
WHEN MATCHED THEN UPDATE SET
    e.EMAIL = s.EMAIL,
    e.PASSWORD_HASH = s.PASSWORD_HASH,
    e.ROLE = s.ROLE,
    e.STATE = s.STATE,
    e.FAILED_ATTEMPTS = s.FAILED_ATTEMPTS,
    e.LAST_ACTION = s.LAST_ACTION,
    e.LAST_ACTION_TIME = SYSTIMESTAMP
WHEN NOT MATCHED THEN INSERT
    (USERNAME, EMAIL, PASSWORD_HASH, ROLE, STATE, FAILED_ATTEMPTS, LAST_ACTION, LAST_ACTION_TIME)
VALUES
    (s.USERNAME, s.EMAIL, s.PASSWORD_HASH, s.ROLE, s.STATE, s.FAILED_ATTEMPTS, s.LAST_ACTION, SYSTIMESTAMP);

MERGE INTO EMPLOYEES e
USING (
    SELECT 'locked_pressiq' AS USERNAME,
           'locked@pressiq.tn' AS EMAIL,
           '926a7e47e168b5f4adc10bfeffc01a070837d73ccdbaaed8509ea31745775ff5' AS PASSWORD_HASH,
           'Manager' AS ROLE,
           'LOCKED' AS STATE,
           3 AS FAILED_ATTEMPTS,
           'ACCOUNT_LOCKED' AS LAST_ACTION
    FROM DUAL
) s
ON (UPPER(e.USERNAME) = UPPER(s.USERNAME))
WHEN MATCHED THEN UPDATE SET
    e.EMAIL = s.EMAIL,
    e.PASSWORD_HASH = s.PASSWORD_HASH,
    e.ROLE = s.ROLE,
    e.STATE = s.STATE,
    e.FAILED_ATTEMPTS = s.FAILED_ATTEMPTS,
    e.LAST_ACTION = s.LAST_ACTION,
    e.LAST_ACTION_TIME = SYSTIMESTAMP
WHEN NOT MATCHED THEN INSERT
    (USERNAME, EMAIL, PASSWORD_HASH, ROLE, STATE, FAILED_ATTEMPTS, LAST_ACTION, LAST_ACTION_TIME)
VALUES
    (s.USERNAME, s.EMAIL, s.PASSWORD_HASH, s.ROLE, s.STATE, s.FAILED_ATTEMPTS, s.LAST_ACTION, SYSTIMESTAMP);

MERGE INTO EMPLOYEES e
USING (
    SELECT 'archived_pressiq' AS USERNAME,
           'archived@pressiq.tn' AS EMAIL,
           '2cde40bc249d7716a0dbdd36dc6b404edd8f8526a42139e4fa2c6da809ade59d' AS PASSWORD_HASH,
           'Operateur' AS ROLE,
           'ARCHIVED' AS STATE,
           0 AS FAILED_ATTEMPTS,
           'ARCHIVED_FOR_TEST' AS LAST_ACTION
    FROM DUAL
) s
ON (UPPER(e.USERNAME) = UPPER(s.USERNAME))
WHEN MATCHED THEN UPDATE SET
    e.EMAIL = s.EMAIL,
    e.PASSWORD_HASH = s.PASSWORD_HASH,
    e.ROLE = s.ROLE,
    e.STATE = s.STATE,
    e.FAILED_ATTEMPTS = s.FAILED_ATTEMPTS,
    e.LAST_ACTION = s.LAST_ACTION,
    e.LAST_ACTION_TIME = SYSTIMESTAMP
WHEN NOT MATCHED THEN INSERT
    (USERNAME, EMAIL, PASSWORD_HASH, ROLE, STATE, FAILED_ATTEMPTS, LAST_ACTION, LAST_ACTION_TIME)
VALUES
    (s.USERNAME, s.EMAIL, s.PASSWORD_HASH, s.ROLE, s.STATE, s.FAILED_ATTEMPTS, s.LAST_ACTION, SYSTIMESTAMP);

-- ----------------------------------------------------------------------------
-- 2) Seed audit rows (deduplicated by DETAILS marker)
-- ----------------------------------------------------------------------------
INSERT INTO AUDIT_LOG (USERNAME, EVENT_TYPE, DETAILS, PERFORMED_BY, HOSTNAME)
SELECT 'admin_pressiq', 'LOGIN_SUCCESS', 'SEED: login success sample', 'admin_pressiq', 'SEED_HOST'
FROM DUAL
WHERE NOT EXISTS (
    SELECT 1 FROM AUDIT_LOG WHERE DETAILS = 'SEED: login success sample'
);

INSERT INTO AUDIT_LOG (USERNAME, EVENT_TYPE, DETAILS, PERFORMED_BY, HOSTNAME)
SELECT 'user_pressiq', 'LOGIN_FAIL', 'SEED: invalid password sample', NULL, 'SEED_HOST'
FROM DUAL
WHERE NOT EXISTS (
    SELECT 1 FROM AUDIT_LOG WHERE DETAILS = 'SEED: invalid password sample'
);

INSERT INTO AUDIT_LOG (USERNAME, EVENT_TYPE, DETAILS, PERFORMED_BY, HOSTNAME)
SELECT 'locked_pressiq', 'ACCOUNT_LOCKED', 'SEED: account locked sample', 'system', 'SEED_HOST'
FROM DUAL
WHERE NOT EXISTS (
    SELECT 1 FROM AUDIT_LOG WHERE DETAILS = 'SEED: account locked sample'
);

INSERT INTO AUDIT_LOG (USERNAME, EVENT_TYPE, DETAILS, PERFORMED_BY, HOSTNAME)
SELECT 'locked_pressiq', 'ACCOUNT_UNLOCKED', 'SEED: account unlocked sample', 'admin_pressiq', 'SEED_HOST'
FROM DUAL
WHERE NOT EXISTS (
    SELECT 1 FROM AUDIT_LOG WHERE DETAILS = 'SEED: account unlocked sample'
);

-- ----------------------------------------------------------------------------
-- 3) Seed one pending password reset request for user test account
-- ----------------------------------------------------------------------------
INSERT INTO PASSWORD_RESET_REQUESTS (USER_ID, USERNAME, EMAIL, MESSAGE, STATUS)
SELECT e.USER_ID,
       e.USERNAME,
       e.EMAIL,
       'SEED: forgot password request',
       'PENDING'
FROM EMPLOYEES e
WHERE UPPER(e.USERNAME) = 'USER_PRESSIQ'
  AND NOT EXISTS (
      SELECT 1
      FROM PASSWORD_RESET_REQUESTS r
      WHERE r.USER_ID = e.USER_ID
        AND r.STATUS = 'PENDING'
  );

COMMIT;

-- ----------------------------------------------------------------------------
-- 4) Verification queries
-- ----------------------------------------------------------------------------
SELECT USER_ID,
       USERNAME,
       EMAIL,
       ROLE,
       STATE,
       FAILED_ATTEMPTS,
       LAST_LOGIN,
       LAST_ACTION,
       LAST_ACTION_TIME
FROM EMPLOYEES
WHERE USERNAME IN ('admin_pressiq', 'user_pressiq', 'locked_pressiq', 'archived_pressiq')
ORDER BY USERNAME;

SELECT LOG_TIME,
       USERNAME,
       EVENT_TYPE,
       DETAILS,
       PERFORMED_BY,
       HOSTNAME
FROM AUDIT_LOG
WHERE USERNAME IN ('admin_pressiq', 'user_pressiq', 'locked_pressiq', 'archived_pressiq')
   OR DETAILS LIKE 'SEED:%'
ORDER BY LOG_TIME DESC;

SELECT REQUEST_ID,
       USER_ID,
       USERNAME,
       STATUS,
       REQUEST_TIME,
       RESOLVED_BY,
       RESOLVED_TIME,
       NEW_PASSWORD_SET
FROM PASSWORD_RESET_REQUESTS
WHERE USERNAME IN ('admin_pressiq', 'user_pressiq', 'locked_pressiq', 'archived_pressiq')
ORDER BY REQUEST_TIME DESC;

-- ----------------------------------------------------------------------------
-- Login credentials to test in the app
-- admin_pressiq / admin123      -> should access admin features (userstaff)
-- user_pressiq / user123        -> normal user login
-- locked_pressiq / lock123      -> locked account behavior
-- archived_pressiq / arch123    -> archived account behavior
-- ----------------------------------------------------------------------------
