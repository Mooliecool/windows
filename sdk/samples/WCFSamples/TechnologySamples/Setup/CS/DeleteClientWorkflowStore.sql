-- Copyright (c) Microsoft Corporation.  All rights reserved.
Use Master
Go
IF EXISTS (SELECT * 
	   FROM   master..sysdatabases 
	   WHERE  name = N'NetFx35Samples_ClientWorkflowStore')
	DROP DATABASE NetFx35Samples_ClientWorkflowStore
GO


