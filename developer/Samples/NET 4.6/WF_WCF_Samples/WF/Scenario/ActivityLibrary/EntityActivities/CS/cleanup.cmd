@echo off

echo Removing Northwind Database...
Osql -S localhost\SQLExpress -E  -n -i "cleanup.sql"

Pause