@echo off

echo Creating Northwind Database...
Osql -S localhost\SQLExpress -E  -n -i "instnwnd.sql"

Pause