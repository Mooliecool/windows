@echo off

echo Creating LinqToSqlSample Database ...
Osql -S localhost\SQLExpress -E  -n -i "createdb.sql"

Pause