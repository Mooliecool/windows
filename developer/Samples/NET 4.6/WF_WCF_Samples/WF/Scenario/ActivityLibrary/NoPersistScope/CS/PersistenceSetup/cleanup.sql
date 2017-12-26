Use Master
Go
IF EXISTS (SELECT * 
	   FROM   master..sysdatabases 
	   WHERE  name = N'NoPersistScopeSample')
	DROP DATABASE NoPersistScopeSample
GO
