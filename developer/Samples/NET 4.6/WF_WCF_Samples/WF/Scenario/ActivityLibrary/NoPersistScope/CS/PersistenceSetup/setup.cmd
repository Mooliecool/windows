@echo off

echo Creating NoPersistScope Database...
Osql -S localhost\SQLExpress -E  -n -i "createNoPersistScopeDb.sql"

echo Creating SQL Workflow instance schema...
Osql -S localhost\SQLExpress -E  -n -i "SqlWorkflowInstanceStoreSchema.sql"

echo Creating SQL Workflow instance logic...
Osql -S localhost\SQLExpress -E  -n -i "SqlWorkflowInstanceStoreLogic.sql"

Pause