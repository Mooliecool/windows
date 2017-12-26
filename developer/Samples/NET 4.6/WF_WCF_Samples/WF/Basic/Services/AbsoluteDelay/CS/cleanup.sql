Use Master
Go
IF EXISTS (SELECT * 
	   FROM   master..sysdatabases 
	   WHERE  name = N'AbsoluteDelaySampleDB')
	DROP DATABASE AbsoluteDelaySampleDB
GO

