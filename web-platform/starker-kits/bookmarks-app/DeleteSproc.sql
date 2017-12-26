USE [Database]
GO

/****** Object:  StoredProcedure [dbo].[DeletePerson]    Script Date: 10/25/2010 21:34:27 ******/
SET ANSI_NULLS ON
GO

SET QUOTED_IDENTIFIER ON
GO

CREATE PROCEDURE [dbo].[DeletePerson]
	@ID INT
AS
BEGIN
	DELETE FROM PERSON WHERE ID=@ID	
END


GO


