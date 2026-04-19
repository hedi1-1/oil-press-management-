-- ============================================================================
-- MASTER DATABASE SCRIPT (single entry point)
-- Run this file once to build and patch the full database.
-- ============================================================================

WHENEVER SQLERROR EXIT SQL.SQLCODE
SET ECHO ON
SET DEFINE OFF

PROMPT [1/9] Base schema...
@create_zitouna_db.sql

PROMPT [2/9] Login/Userstaff database additions...
@sql_add_login_userstaff_db.sql

PROMPT [3/9] Machine migration...
@machine/sql_migrate_machine_employee.sql

PROMPT [4/9] Production quality AI migration...
@production/sql_migrate_quality_ai.sql

PROMPT [5/9] Stock Arduino migration...
@stock/sql_migrate_stock_arduino.sql

PROMPT [6/9] Stock Arduino fix...
@stock/sql_fix_stock_arduino.sql

PROMPT [7/9] Login/Userstaff seed data...
@one_time_login_userstaff_seed.sql

PROMPT [8/9] Optional userstaff verification queries...
@userstaff/Projet2A1.sql

PROMPT [9/9] Optional stock health check...
@stock/sql_check_stock_arduino_health.sql

COMMIT;
PROMPT All database scripts executed successfully.
EXIT;
