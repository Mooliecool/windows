-- Copyright (c) Microsoft Corporation.  All rights reserved.
Use Master
Go
IF EXISTS (SELECT * 
	   FROM   master..sysdatabases 
	   WHERE  name = N'NetFx35Samples_DurableServiceStore')
	DROP DATABASE NetFx35Samples_DurableServiceStore
GO
CREATE DATABASE NetFx35Samples_DurableServiceStore
GO

