-----------------------------------------------------------------------
--  This file is part of the Windows Workflow Foundation SDK Code Samples.
-- 
--  Copyright (C) Microsoft Corporation.  All rights reserved.
-- 
--This source code is intended only as a supplement to Microsoft
--Development Tools and/or on-line documentation.  See these other
--materials for detailed information regarding Microsoft code samples.
-- 
--THIS CODE AND INFORMATION ARE PROVIDED AS IS WITHOUT WARRANTY OF ANY
--KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
--IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
--PARTICULAR PURPOSE.
-----------------------------------------------------------------------
/****** Object:  Database TransactionServiceSampleDB    Script Date: 7/31/2005 12:16:22 AM ******/
IF EXISTS (SELECT name FROM master.dbo.sysdatabases WHERE name = N'TransactionServiceSampleDB')
	DROP DATABASE [TransactionServiceSampleDB]
GO

CREATE DATABASE [TransactionServiceSampleDB]  ON (NAME = N'TransactionServiceSampleDB_Data', FILENAME = N'C:\Program Files\Microsoft SQL Server\MSSQL\data\TransactionServiceSampleDB_Data.MDF' , SIZE = 2, FILEGROWTH = 10%) LOG ON (NAME = N'TransactionServiceSampleDB_Log', FILENAME = N'C:\Program Files\Microsoft SQL Server\MSSQL\data\TransactionServiceSampleDB_Log.LDF' , SIZE = 1, FILEGROWTH = 10%)
 COLLATE SQL_Latin1_General_CP1_CI_AS
GO

exec sp_dboption N'TransactionServiceSampleDB', N'autoclose', N'false'
GO

exec sp_dboption N'TransactionServiceSampleDB', N'bulkcopy', N'false'
GO

exec sp_dboption N'TransactionServiceSampleDB', N'trunc. log', N'false'
GO

exec sp_dboption N'TransactionServiceSampleDB', N'torn page detection', N'true'
GO

exec sp_dboption N'TransactionServiceSampleDB', N'read only', N'false'
GO

exec sp_dboption N'TransactionServiceSampleDB', N'dbo use', N'false'
GO

exec sp_dboption N'TransactionServiceSampleDB', N'single', N'false'
GO

exec sp_dboption N'TransactionServiceSampleDB', N'autoshrink', N'false'
GO

exec sp_dboption N'TransactionServiceSampleDB', N'ANSI null default', N'false'
GO

exec sp_dboption N'TransactionServiceSampleDB', N'recursive triggers', N'false'
GO

exec sp_dboption N'TransactionServiceSampleDB', N'ANSI nulls', N'false'
GO

exec sp_dboption N'TransactionServiceSampleDB', N'concat null yields null', N'false'
GO

exec sp_dboption N'TransactionServiceSampleDB', N'cursor close on commit', N'false'
GO

exec sp_dboption N'TransactionServiceSampleDB', N'default to local cursor', N'false'
GO

exec sp_dboption N'TransactionServiceSampleDB', N'quoted identifier', N'false'
GO

exec sp_dboption N'TransactionServiceSampleDB', N'ANSI warnings', N'false'
GO

exec sp_dboption N'TransactionServiceSampleDB', N'auto create statistics', N'true'
GO

exec sp_dboption N'TransactionServiceSampleDB', N'auto update statistics', N'true'
GO

use [TransactionServiceSampleDB]
GO

/****** Object:  Stored Procedure dbo.CreditAmount    Script Date: 7/31/2005 12:16:22 AM ******/
if exists (select * from dbo.sysobjects where id = object_id(N'[dbo].[CreditAmount]') and OBJECTPROPERTY(id, N'IsProcedure') = 1)
drop procedure [dbo].[CreditAmount]
GO

/****** Object:  Stored Procedure dbo.DebitAmount    Script Date: 7/31/2005 12:16:22 AM ******/
if exists (select * from dbo.sysobjects where id = object_id(N'[dbo].[DebitAmount]') and OBJECTPROPERTY(id, N'IsProcedure') = 1)
drop procedure [dbo].[DebitAmount]
GO

/****** Object:  Stored Procedure dbo.GetAccountBalances    Script Date: 7/31/2005 12:16:22 AM ******/
if exists (select * from dbo.sysobjects where id = object_id(N'[dbo].[GetAccountBalances]') and OBJECTPROPERTY(id, N'IsProcedure') = 1)
drop procedure [dbo].[GetAccountBalances]
GO

/****** Object:  Table [dbo].[AccountTable]    Script Date: 7/31/2005 12:16:22 AM ******/
if exists (select * from dbo.sysobjects where id = object_id(N'[dbo].[AccountTable]') and OBJECTPROPERTY(id, N'IsUserTable') = 1)
drop table [dbo].[AccountTable]
GO

/****** Object:  Table [dbo].[AccountTable]    Script Date: 7/31/2005 12:16:22 AM ******/
CREATE TABLE [dbo].[AccountTable] (
	[AccountNumber] [int] IDENTITY (1, 1) NOT NULL ,
	[CheckingAmount] [int] NOT NULL ,
	[SavingsAmount] [int] NOT NULL 
) ON [PRIMARY]
GO

ALTER TABLE [dbo].[AccountTable] WITH NOCHECK ADD 
	CONSTRAINT [PK_AccountTable] PRIMARY KEY  CLUSTERED 
	(
		[AccountNumber]
	)  ON [PRIMARY] 
GO

ALTER TABLE [dbo].[AccountTable] ADD 
	CONSTRAINT [DF_AccountTable_CheckingAmount] DEFAULT (1000) FOR [CheckingAmount],
	CONSTRAINT [DF_AccountTable_SavingsAmount] DEFAULT (1000) FOR [SavingsAmount]
GO


INSERT INTO [dbo].[AccountTable]
VALUES (1000,1000)

GO


SET QUOTED_IDENTIFIER ON 
GO
SET ANSI_NULLS ON 
GO

/****** Object:  Stored Procedure dbo.CreditAmount    Script Date: 7/31/2005 12:16:22 AM ******/



/****** Object:  Stored Procedure dbo.CreditAmount    Script Date: 7/26/2005 11:54:08 AM ******/
CREATE  PROCEDURE [dbo].[CreditAmount] 	@Amount	int
AS
 BEGIN
	SET NOCOUNT ON

	SET TRANSACTION ISOLATION LEVEL READ COMMITTED


	declare @localized_string_CreditAmount_Failed_Params_Amount  nvarchar(256)
	set @localized_string_CreditAmount_Failed_Params_Amount = N'Amount Cannot be non null'


	declare @localized_string_CreditAmount_AccountNumberNotFound  nvarchar(256)
	set @localized_string_CreditAmount_AccountNumberNotFound = N'AccountNumber Not Found'


	declare @localized_string_CreditAmount_AccountNumberFailed  nvarchar(256)
	set @localized_string_CreditAmount_AccountNumberFailed = N'Failed to get Account Number'


	declare @localized_string_CreditAmount_UpdateStatusFailed  nvarchar(256)
	set @localized_string_CreditAmount_UpdateStatusFailed = N'Creadit Amount Failed'


	DECLARE @local_tran		bit
			,@error			int
			,@error_desc	nvarchar(256)
			,@ret			smallint
			,@checkingAmount  int

	SET @ret = 0
	IF @Amount IS NULL 	
	 BEGIN
		SET @error_desc = @localized_string_CreditAmount_Failed_Params_Amount
		GOTO FAILED
	 END


	IF @@TRANCOUNT > 0
		SET @local_tran = 0
	ELSE
	 BEGIN
		BEGIN TRANSACTION
		SET @local_tran = 1		
	 END
	
	SELECT 	@checkingAmount  = CheckingAmount
	FROM		[dbo].[AccountTable] WITH (ROWLOCK,HOLDLOCK)
             WHERE	[AccountNumber] = 1     

	IF @@ROWCOUNT = 0
	 BEGIN
		SET @error_desc = @localized_string_CreditAmount_AccountNumberNotFound
		GOTO FAILED
	 END

	IF @@ERROR <> 0 
	 BEGIN
		SET @error_desc = @localized_string_CreditAmount_AccountNumberFailed 
		GOTO FAILED
	 END



	/* Now Add Amount and Update the Row */

	set @checkingAmount = @checkingAmount + @Amount	

	UPDATE [dbo].[AccountTable] 
	SET [CheckingAmount] = @checkingAmount
	WHERE [AccountNumber] =  1


	IF @@ERROR <> 0 
	 BEGIN
		SET @error_desc = @localized_string_CreditAmount_UpdateStatusFailed
		GOTO FAILED
	 END

	GOTO DONE

FAILED:
	IF @local_tran = 1
		ROLLBACK TRANSACTION

	RAISERROR( @error_desc, 16, -1 )

	SET @ret = -1
	RETURN @ret

DONE:
	IF @local_tran = 1
		COMMIT TRANSACTION


	SET TRANSACTION ISOLATION LEVEL READ COMMITTED
	

 END

GO

SET QUOTED_IDENTIFIER OFF 
GO
SET ANSI_NULLS ON 
GO

SET QUOTED_IDENTIFIER ON 
GO
SET ANSI_NULLS ON 
GO

/****** Object:  Stored Procedure dbo.DebitAmount    Script Date: 7/31/2005 12:16:22 AM ******/

/****** Object:  Stored Procedure dbo.DebitAmount    Script Date: 7/26/2005 11:54:08 AM ******/
CREATE  PROCEDURE [dbo].[DebitAmount] 	@Amount	int
AS
 BEGIN
	SET NOCOUNT ON

	SET TRANSACTION ISOLATION LEVEL READ COMMITTED


	declare @localized_string_DebitAmount_Failed_Params_Amount  nvarchar(256)
	set @localized_string_DebitAmount_Failed_Params_Amount = N'Amount Cannot be non null'


	declare @localized_string_DebitAmount_AccountNumberNotFound  nvarchar(256)
	set @localized_string_DebitAmount_AccountNumberNotFound = N'AccountNumber Not Found'


	declare @localized_string_DebitAmount_AccountNumberFailed  nvarchar(256)
	set @localized_string_DebitAmount_AccountNumberFailed = N'Failed to get Account Number'


	declare @localized_string_DebitAmount_UpdateStatusFailed  nvarchar(256)
	set @localized_string_DebitAmount_UpdateStatusFailed = N'Creadit Amount Failed'

             declare @localized_string_DebitAmount_DebitAmountGreater nvarchar(256)
             set @localized_string_DebitAmount_DebitAmountGreater = N'Debit amount Greater than savings Account'


	DECLARE @local_tran		bit
			,@error			int
			,@error_desc	nvarchar(256)
			,@ret			smallint
			,@savingsAmount  int

	SET @ret = 0
	IF @Amount IS NULL 	
	 BEGIN
		SET @error_desc = @localized_string_DebitAmount_Failed_Params_Amount
		GOTO FAILED
	 END


	IF @@TRANCOUNT > 0
		SET @local_tran = 0
	ELSE
	 BEGIN
		BEGIN TRANSACTION
		SET @local_tran = 1		
	 END
	
	SELECT 	@savingsAmount  = SavingsAmount
	FROM		[dbo].[AccountTable] WITH (ROWLOCK,HOLDLOCK)
             WHERE	[AccountNumber] = 1                -- Hardcoding the account number to be 1 always

	IF @@ROWCOUNT = 0
	 BEGIN
		SET @error_desc = @localized_string_DebitAmount_AccountNumberNotFound
		GOTO FAILED
	 END

	IF @@ERROR <> 0 
	 BEGIN
		SET @error_desc = @localized_string_DebitAmount_AccountNumberFailed 
		GOTO FAILED
	 END



	/* Now Add Amount and Update the Row */

	IF @Amount > @savingsAmount
	  BEGIN
		SET @error_desc = @localized_string_DebitAmount_DebitAmountGreater
		GOTO FAILED
               END
   
	set @savingsAmount = @savingsAmount - @Amount	

	UPDATE [dbo].[AccountTable] 
	SET [SavingsAmount] = @savingsAmount
	WHERE [AccountNumber] =  1


	IF @@ERROR <> 0 
	 BEGIN
		SET @error_desc = @localized_string_DebitAmount_UpdateStatusFailed
		GOTO FAILED
	 END

	GOTO DONE

FAILED:
	IF @local_tran = 1
		ROLLBACK TRANSACTION

	RAISERROR( @error_desc, 16, -1 )

	SET @ret = -1
	RETURN @ret

DONE:
	IF @local_tran = 1
		COMMIT TRANSACTION


	SET TRANSACTION ISOLATION LEVEL READ COMMITTED
	RETURN @ret

 END

GO

SET QUOTED_IDENTIFIER OFF 
GO
SET ANSI_NULLS ON 
GO

SET QUOTED_IDENTIFIER OFF 
GO
SET ANSI_NULLS OFF 
GO

/****** Object:  Stored Procedure dbo.GetAccountBalances    Script Date: 7/31/2005 12:16:22 AM ******/

/****** Object:  Stored Procedure dbo.GetAccountBalances    Script Date: 7/26/2005 11:54:08 AM ******/
CREATE  PROCEDURE [dbo].[GetAccountBalances] 	@AccountNumber	int
AS
 BEGIN
	SET NOCOUNT ON

	SET TRANSACTION ISOLATION LEVEL READ COMMITTED


	declare @localized_string_GetAccountNumber_Failed_Params_AccountNumber nvarchar(256)
	set @localized_string_GetAccountNumber_Failed_Params_AccountNumber = N'Account Number Cannot be non null'

	declare @localized_string_GetAccountNumber_AccountNotFound  nvarchar(256)
	set @localized_string_GetAccountNumber_AccountNotFound = N'Account Not Found'

	declare @localized_string_GetAccountNumber_GeAccountFailed  nvarchar(256)
	set @localized_string_GetAccountNumber_GeAccountFailed  = N'Get Account Balance Failed'



	DECLARE 	@error			int
			,@error_desc	nvarchar(256)
			,@ret			smallint

	SET @ret = 0
	IF @AccountNumber IS NULL 	
	 BEGIN
		SET @error_desc = @localized_string_GetAccountNumber_Failed_Params_AccountNumber 
		GOTO FAILED
	 END

	
	SELECT 	 CheckingAmount
                                        ,SavingsAmount
	FROM		[dbo].[AccountTable] WITH (ROWLOCK,HOLDLOCK)
	WHERE	[AccountNumber] = @AccountNumber 

	IF @@ROWCOUNT = 0
	 BEGIN
		SET @error_desc = @localized_string_GetAccountNumber_AccountNotFound
		GOTO FAILED
	 END

	IF @@ERROR <> 0 
	 BEGIN
		SET @error_desc = @localized_string_GetAccountNumber_GeAccountFailed
		GOTO FAILED
	 END

	GOTO DONE

FAILED:
	RAISERROR( @error_desc, 16, -1 )

	SET @ret = -1
	GOTO DONE

DONE:
	SET TRANSACTION ISOLATION LEVEL READ COMMITTED
	RETURN @ret

 END

GO

SET QUOTED_IDENTIFIER OFF 
GO
SET ANSI_NULLS ON 
GO

