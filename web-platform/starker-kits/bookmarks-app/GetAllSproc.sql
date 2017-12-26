USE [Favorites]
GO

/****** Object:  StoredProcedure [dbo].[GetAllSites]    Script Date: 10/25/2010 21:34:12 ******/
SET ANSI_NULLS ON
GO

SET QUOTED_IDENTIFIER ON
GO

CREATE PROCEDURE [dbo].[GetAllSites]	
AS
BEGIN
	SELECT SiteID,Name,City,States,Country FROM Favorites.Sites
END


GO