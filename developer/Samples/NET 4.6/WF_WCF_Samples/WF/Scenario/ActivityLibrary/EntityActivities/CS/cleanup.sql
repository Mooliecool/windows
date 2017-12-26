Use Master
Go
IF EXISTS (SELECT * 
	   FROM   master..sysdatabases 
	   WHERE  name = N'Northwind')
	DROP DATABASE Northwind
GO
