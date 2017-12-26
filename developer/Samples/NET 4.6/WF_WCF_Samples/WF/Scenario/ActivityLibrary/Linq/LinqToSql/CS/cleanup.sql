Use Master
Go
IF EXISTS (SELECT * 
	   FROM   master..sysdatabases 
	   WHERE  name = N'LinqToSqlSample')
	DROP DATABASE LinqToSqlSample
GO
