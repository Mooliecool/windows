CREATE PROCEDURE [dbo].[InsertOfficeAssignment]
		@InstructorID int,
		@Location nvarchar(50)
		AS
		INSERT INTO dbo.OfficeAssignment (InstructorID, Location)
		VALUES (@InstructorID, @Location);
		IF @@ROWCOUNT > 0
		BEGIN
			SELECT [Timestamp] FROM OfficeAssignment 
				WHERE InstructorID=@InstructorID;
		END

