-- Copyright (c) Microsoft Corporation.  All rights reserved.
Use Master
Go
IF EXISTS (SELECT * 
	   FROM   master..sysdatabases 
	   WHERE  name = N'Tracking')
	DROP DATABASE Tracking
GO
CREATE DATABASE Tracking
GO

