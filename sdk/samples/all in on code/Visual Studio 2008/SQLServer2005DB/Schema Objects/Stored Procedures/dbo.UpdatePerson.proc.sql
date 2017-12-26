CREATE PROCEDURE [dbo].[UpdatePerson]
		@PersonID int,
		@LastName nvarchar(50),
		@FirstName nvarchar(50),
		@HireDate datetime,
		@EnrollmentDate datetime
		AS
		UPDATE Person SET LastName=@LastName, 
				FirstName=@FirstName,
				HireDate=@HireDate,
				EnrollmentDate=@EnrollmentDate
		WHERE PersonID=@PersonID;
