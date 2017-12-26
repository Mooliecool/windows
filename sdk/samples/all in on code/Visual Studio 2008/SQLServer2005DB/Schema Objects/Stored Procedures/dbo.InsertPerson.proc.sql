CREATE PROCEDURE [dbo].[InsertPerson]
		@LastName nvarchar(50),
		@FirstName nvarchar(50),
		@HireDate datetime,
		@EnrollmentDate datetime
		AS
		INSERT INTO dbo.Person (LastName, 
					FirstName, 
					HireDate, 
					EnrollmentDate)
		VALUES (@LastName, 
			@FirstName, 
			@HireDate, 
			@EnrollmentDate);
		SELECT SCOPE_IDENTITY() as NewPersonID;
