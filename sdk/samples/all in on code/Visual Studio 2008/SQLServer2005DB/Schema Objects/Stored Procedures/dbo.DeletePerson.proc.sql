CREATE PROCEDURE [dbo].[DeletePerson]
		@PersonID int
		AS
		DELETE FROM Person WHERE PersonID = @PersonID;
