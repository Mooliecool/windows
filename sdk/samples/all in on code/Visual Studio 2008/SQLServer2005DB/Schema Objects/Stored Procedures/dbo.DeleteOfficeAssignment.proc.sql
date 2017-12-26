CREATE PROCEDURE [dbo].[DeleteOfficeAssignment]
		@InstructorID int
		AS
		DELETE FROM OfficeAssignment
		WHERE InstructorID=@InstructorID;
