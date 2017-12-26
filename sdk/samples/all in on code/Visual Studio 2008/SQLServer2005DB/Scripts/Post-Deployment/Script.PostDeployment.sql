/*
Post-Deployment Script Template							
--------------------------------------------------------------------------------------
 This file contains SQL statements that will be appended to the build script		
 Use SQLCMD syntax to include a file into the post-deployment script			
 Example:      :r .\filename.sql								
 Use SQLCMD syntax to reference a variable in the post-deployment script		
 Example:      :setvar TableName MyTable							
               SELECT * FROM [$(TableName)]					
--------------------------------------------------------------------------------------
*/
:r .\Permissions.sql

:r .\RoleMemberships.sql

:r .\RulesAndDefaults.sql

:r .\DatabaseObjectOptions.sql

:r .\Signatures.sql

/*

--------------------------------------------------------------------------------------
This part is used to insert records to tables after the deployment.	
--------------------------------------------------------------------------------------
*/
GO
SET IDENTITY_INSERT dbo.Person ON
GO
INSERT INTO dbo.Person (PersonID, LastName, FirstName, PersonCategory, HireDate, EnrollmentDate)
VALUES (1, 'Abercrombie', 'Kim', 2, '1995-03-11', null);
INSERT INTO dbo.Person (PersonID, LastName, FirstName, PersonCategory, HireDate, EnrollmentDate)
VALUES (2, 'Barzdukas', 'Gytis', 1, null, '2005-09-01');
INSERT INTO dbo.Person (PersonID, LastName, FirstName, PersonCategory, HireDate, EnrollmentDate)
VALUES (3, 'Justice', 'Peggy', 1, null, '2001-09-01');
INSERT INTO dbo.Person (PersonID, LastName, FirstName, PersonCategory, HireDate, EnrollmentDate)
VALUES (4, 'Fakhouri', 'Fadi', 2, '2002-08-06', null);
INSERT INTO dbo.Person (PersonID, LastName, FirstName, PersonCategory, HireDate, EnrollmentDate)
VALUES (5, 'Harui', 'Roger', 2, '1998-07-01', null);
INSERT INTO dbo.Person (PersonID, LastName, FirstName, PersonCategory, HireDate, EnrollmentDate)
VALUES (6, 'Li', 'Yan', 1, null, '2002-09-01');
INSERT INTO dbo.Person (PersonID, LastName, FirstName, PersonCategory, HireDate, EnrollmentDate)
VALUES (7, 'Norman', 'Laura', 1, null, '2003-09-01');
INSERT INTO dbo.Person (PersonID, LastName, FirstName, PersonCategory, HireDate, EnrollmentDate)
VALUES (8, 'Olivotto', 'Nino', 1, null, '2005-09-01');
INSERT INTO dbo.Person (PersonID, LastName, FirstName, PersonCategory, HireDate, EnrollmentDate)
VALUES (9, 'Tang', 'Wayne', 1, null, '2005-09-01');
INSERT INTO dbo.Person (PersonID, LastName, FirstName, PersonCategory, HireDate, EnrollmentDate)
VALUES (10, 'Alonso', 'Meredith', 1, null, '2002-09-01');
INSERT INTO dbo.Person (PersonID, LastName, FirstName, PersonCategory, HireDate, EnrollmentDate)
VALUES (11, 'Lopez', 'Sophia', 1, null, '2004-09-01');
INSERT INTO dbo.Person (PersonID, LastName, FirstName, PersonCategory, HireDate, EnrollmentDate)
VALUES (12, 'Browning', 'Meredith', 1, null, '2000-09-01');
INSERT INTO dbo.Person (PersonID, LastName, FirstName, PersonCategory, HireDate, EnrollmentDate)
VALUES (13, 'Anand', 'Arturo', 1, null, '2003-09-01');
INSERT INTO dbo.Person (PersonID, LastName, FirstName, PersonCategory, HireDate, EnrollmentDate)
VALUES (14, 'Walker', 'Alexandra', 1, null, '2000-09-01');
INSERT INTO dbo.Person (PersonID, LastName, FirstName, PersonCategory, HireDate, EnrollmentDate)
VALUES (15, 'Powell', 'Carson', 1, null, '2004-09-01');
INSERT INTO dbo.Person (PersonID, LastName, FirstName, PersonCategory, HireDate, EnrollmentDate)
VALUES (16, 'Jai', 'Damien', 1, null, '2001-09-01');
INSERT INTO dbo.Person (PersonID, LastName, FirstName, PersonCategory, HireDate, EnrollmentDate)
VALUES (17, 'Carlson', 'Robyn', 1, null, '2005-09-01');
INSERT INTO dbo.Person (PersonID, LastName, FirstName, PersonCategory, HireDate, EnrollmentDate)
VALUES (18, 'Zheng', 'Roger', 2, '2004-02-12', null);
INSERT INTO dbo.Person (PersonID, LastName, FirstName, PersonCategory, HireDate, EnrollmentDate)
VALUES (19, 'Bryant', 'Carson', 1, null, '2001-09-01');
INSERT INTO dbo.Person (PersonID, LastName, FirstName, PersonCategory, HireDate, EnrollmentDate)
VALUES (20, 'Suarez', 'Robyn', 1, null, '2004-09-01');
INSERT INTO dbo.Person (PersonID, LastName, FirstName, PersonCategory, HireDate, EnrollmentDate)
VALUES (21, 'Holt', 'Roger', 1, null, '2004-09-01');
INSERT INTO dbo.Person (PersonID, LastName, FirstName, PersonCategory, HireDate, EnrollmentDate)
VALUES (22, 'Alexander', 'Carson', 1, null, '2005-09-01');
INSERT INTO dbo.Person (PersonID, LastName, FirstName, PersonCategory, HireDate, EnrollmentDate)
VALUES (23, 'Morgan', 'Isaiah', 1, null, '2001-09-01');
INSERT INTO dbo.Person (PersonID, LastName, FirstName, PersonCategory, HireDate, EnrollmentDate)
VALUES (24, 'Martin', 'Randall', 1, null, '2005-09-01');
INSERT INTO dbo.Person (PersonID, LastName, FirstName, PersonCategory, HireDate, EnrollmentDate)
VALUES (25, 'Kapoor', 'Candace', 2, '2001-01-15', null);
INSERT INTO dbo.Person (PersonID, LastName, FirstName, PersonCategory, HireDate, EnrollmentDate)
VALUES (26, 'Rogers', 'Cody', 1, null, '2002-09-01');
INSERT INTO dbo.Person (PersonID, LastName, FirstName, PersonCategory, HireDate, EnrollmentDate)
VALUES (27, 'Serrano', 'Stacy', 2, '1999-06-01', null);
INSERT INTO dbo.Person (PersonID, LastName, FirstName, PersonCategory, HireDate, EnrollmentDate)
VALUES (28, 'White', 'Anthony', 1, null, '2001-09-01');
INSERT INTO dbo.Person (PersonID, LastName, FirstName, PersonCategory, HireDate, EnrollmentDate)
VALUES (29, 'Griffin', 'Rachel', 1, null, '2004-09-01');
INSERT INTO dbo.Person (PersonID, LastName, FirstName, PersonCategory, HireDate, EnrollmentDate)
VALUES (30, 'Shan', 'Alicia', 1, null, '2003-09-01');
INSERT INTO dbo.Person (PersonID, LastName, FirstName, PersonCategory, HireDate, EnrollmentDate)
VALUES (31, 'Stewart', 'Jasmine', 2, '1997-10-12', null);
INSERT INTO dbo.Person (PersonID, LastName, FirstName, PersonCategory, HireDate, EnrollmentDate)
VALUES (32, 'Xu', 'Kristen', 2, '2001-7-23', null);
INSERT INTO dbo.Person (PersonID, LastName, FirstName, PersonCategory, HireDate, EnrollmentDate)
VALUES (33, 'Gao', 'Erica', 1, null, '2003-01-30');
INSERT INTO dbo.Person (PersonID, LastName, FirstName, PersonCategory, HireDate, EnrollmentDate)
VALUES (34, 'Van Houten', 'Roger', 2, '2000-12-07', null);
GO
SET IDENTITY_INSERT dbo.Person OFF
GO

-- Insert data into the Department table.
INSERT INTO dbo.Department (DepartmentID, [Name], Budget, StartDate, Administrator)
VALUES (1, 'Engineering', 350000.00, '2007-09-01', 2);
INSERT INTO dbo.Department (DepartmentID, [Name], Budget, StartDate, Administrator)
VALUES (2, 'English', 120000.00, '2007-09-01', 6);
INSERT INTO dbo.Department (DepartmentID, [Name], Budget, StartDate, Administrator)
VALUES (4, 'Economics', 200000.00, '2007-09-01', 4);
INSERT INTO dbo.Department (DepartmentID, [Name], Budget, StartDate, Administrator)
VALUES (7, 'Mathematics', 250000.00, '2007-09-01', 3);
GO


-- Insert data into the Course table.
INSERT INTO dbo.Course (CourseID, Title, Credits, DepartmentID)
VALUES (1050, 'Chemistry', 4, 1);
INSERT INTO dbo.Course (CourseID, Title, Credits, DepartmentID)
VALUES (1061, 'Physics', 4, 1);
INSERT INTO dbo.Course (CourseID, Title, Credits, DepartmentID)
VALUES (1045, 'Calculus', 4, 7);
INSERT INTO dbo.Course (CourseID, Title, Credits, DepartmentID)
VALUES (2030, 'Poetry', 2, 2);
INSERT INTO dbo.Course (CourseID, Title, Credits, DepartmentID)
VALUES (2021, 'Composition', 3, 2);
INSERT INTO dbo.Course (CourseID, Title, Credits, DepartmentID)
VALUES (2042, 'Literature', 4, 2);
INSERT INTO dbo.Course (CourseID, Title, Credits, DepartmentID)
VALUES (4022, 'Microeconomics', 3, 4);
INSERT INTO dbo.Course (CourseID, Title, Credits, DepartmentID)
VALUES (4041, 'Macroeconomics', 3, 4);
INSERT INTO dbo.Course (CourseID, Title, Credits, DepartmentID)
VALUES (4061, 'Quantitative', 2, 4);
INSERT INTO dbo.Course (CourseID, Title, Credits, DepartmentID)
VALUES (3141, 'Trigonometry', 4, 7);
GO

-- Insert data into the OnlineCourse table.
INSERT INTO dbo.OnlineCourse (CourseID, URL)
VALUES (2030, 'http://www.fineartschool.net/Poetry');
INSERT INTO dbo.OnlineCourse (CourseID, URL)
VALUES (2021, 'http://www.fineartschool.net/Composition');
INSERT INTO dbo.OnlineCourse (CourseID, URL)
VALUES (4041, 'http://www.fineartschool.net/Macroeconomics');
INSERT INTO dbo.OnlineCourse (CourseID, URL)
VALUES (3141, 'http://www.fineartschool.net/Trigonometry');

--Insert data into OnsiteCourse table.
INSERT INTO dbo.OnsiteCourse (CourseID, Location, Days, [Time])
VALUES (1050, '123 Smith', 'MTWH', '11:30');
INSERT INTO dbo.OnsiteCourse (CourseID, Location, Days, [Time])
VALUES (1061, '234 Smith', 'TWHF', '13:15');
INSERT INTO dbo.OnsiteCourse (CourseID, Location, Days, [Time])
VALUES (1045, '121 Smith','MWHF', '15:30');
INSERT INTO dbo.OnsiteCourse (CourseID, Location, Days, [Time])
VALUES (4061, '22 Williams', 'TH', '11:15');
INSERT INTO dbo.OnsiteCourse (CourseID, Location, Days, [Time])
VALUES (2042, '225 Adams', 'MTWH', '11:00');
INSERT INTO dbo.OnsiteCourse (CourseID, Location, Days, [Time])
VALUES (4022, '23 Williams', 'MWF', '9:00');

-- Insert data into the CourseInstructor table.
INSERT INTO dbo.CourseInstructor(CourseID, PersonID)
VALUES (1050, 1);
INSERT INTO dbo.CourseInstructor(CourseID, PersonID)
VALUES (1061, 31);
INSERT INTO dbo.CourseInstructor(CourseID, PersonID)
VALUES (1045, 5);
INSERT INTO dbo.CourseInstructor(CourseID, PersonID)
VALUES (2030, 4);
INSERT INTO dbo.CourseInstructor(CourseID, PersonID)
VALUES (2021, 27);
INSERT INTO dbo.CourseInstructor(CourseID, PersonID)
VALUES (2042, 25);
INSERT INTO dbo.CourseInstructor(CourseID, PersonID)
VALUES (4022, 18);
INSERT INTO dbo.CourseInstructor(CourseID, PersonID)
VALUES (4041, 32);
INSERT INTO dbo.CourseInstructor(CourseID, PersonID)
VALUES (4061, 34);
GO

--Insert data into the OfficeAssignment table.
INSERT INTO dbo.OfficeAssignment(InstructorID, Location)
VALUES (1, '17 Smith');
INSERT INTO dbo.OfficeAssignment(InstructorID, Location)
VALUES (4, '29 Adams');
INSERT INTO dbo.OfficeAssignment(InstructorID, Location)
VALUES (5, '37 Williams');
INSERT INTO dbo.OfficeAssignment(InstructorID, Location)
VALUES (18, '143 Smith');
INSERT INTO dbo.OfficeAssignment(InstructorID, Location)
VALUES (25, '57 Adams');
INSERT INTO dbo.OfficeAssignment(InstructorID, Location)
VALUES (27, '271 Williams');
INSERT INTO dbo.OfficeAssignment(InstructorID, Location)
VALUES (31, '131 Smith');
INSERT INTO dbo.OfficeAssignment(InstructorID, Location)
VALUES (32, '203 Williams');
INSERT INTO dbo.OfficeAssignment(InstructorID, Location)
VALUES (34, '213 Smith');

-- Insert data into the CourseGrade table.
INSERT INTO dbo.CourseGrade (CourseID, StudentID, Grade)
VALUES (2021, 2, 4);
INSERT INTO dbo.CourseGrade (CourseID, StudentID, Grade)
VALUES (2030, 2, 3.5);
INSERT INTO dbo.CourseGrade (CourseID, StudentID, Grade)
VALUES (2021, 3, 3);
INSERT INTO dbo.CourseGrade (CourseID, StudentID, Grade)
VALUES (2030, 3, 4);
INSERT INTO dbo.CourseGrade (CourseID, StudentID, Grade)
VALUES (2021, 6, 2.5);
INSERT INTO dbo.CourseGrade (CourseID, StudentID, Grade)
VALUES (2042, 6, 3.5);
INSERT INTO dbo.CourseGrade (CourseID, StudentID, Grade)
VALUES (2021, 7, 3.5);
INSERT INTO dbo.CourseGrade (CourseID, StudentID, Grade)
VALUES (2042, 7, 4);
INSERT INTO dbo.CourseGrade (CourseID, StudentID, Grade)
VALUES (2021, 8, 3);
INSERT INTO dbo.CourseGrade (CourseID, StudentID, Grade)
VALUES (2042, 8, 3);
INSERT INTO dbo.CourseGrade (CourseID, StudentID, Grade)
VALUES (4041, 9, 3.5);
INSERT INTO dbo.CourseGrade (CourseID, StudentID, Grade)
VALUES (4041, 10, null);
INSERT INTO dbo.CourseGrade (CourseID, StudentID, Grade)
VALUES (4041, 11, 2.5);
INSERT INTO dbo.CourseGrade (CourseID, StudentID, Grade)
VALUES (4041, 12, null);
INSERT INTO dbo.CourseGrade (CourseID, StudentID, Grade)
VALUES (4061, 12, null);
INSERT INTO dbo.CourseGrade (CourseID, StudentID, Grade)
VALUES (4022, 14, 3);
INSERT INTO dbo.CourseGrade (CourseID, StudentID, Grade)
VALUES (4022, 13, 4);
INSERT INTO dbo.CourseGrade (CourseID, StudentID, Grade)
VALUES (4061, 13, 4);
INSERT INTO dbo.CourseGrade (CourseID, StudentID, Grade)
VALUES (4041, 14, 3);
INSERT INTO dbo.CourseGrade (CourseID, StudentID, Grade)
VALUES (4022, 15, 2.5);
INSERT INTO dbo.CourseGrade (CourseID, StudentID, Grade)
VALUES (4022, 16, 2);
INSERT INTO dbo.CourseGrade (CourseID, StudentID, Grade)
VALUES (4022, 17, null);
INSERT INTO dbo.CourseGrade (CourseID, StudentID, Grade)
VALUES (4022, 19, 3.5);
INSERT INTO dbo.CourseGrade (CourseID, StudentID, Grade)
VALUES (4061, 20, 4);
INSERT INTO dbo.CourseGrade (CourseID, StudentID, Grade)
VALUES (4061, 21, 2);
INSERT INTO dbo.CourseGrade (CourseID, StudentID, Grade)
VALUES (4022, 22, 3);
INSERT INTO dbo.CourseGrade (CourseID, StudentID, Grade)
VALUES (4041, 22, 3.5);
INSERT INTO dbo.CourseGrade (CourseID, StudentID, Grade)
VALUES (4061, 22, 2.5);
INSERT INTO dbo.CourseGrade (CourseID, StudentID, Grade)
VALUES (4022, 23, 3);
INSERT INTO dbo.CourseGrade (CourseID, StudentID, Grade)
VALUES (1045, 23, 1.5);
INSERT INTO dbo.CourseGrade (CourseID, StudentID, Grade)
VALUES (1061, 24, 4);
INSERT INTO dbo.CourseGrade (CourseID, StudentID, Grade)
VALUES (1061, 25, 3);
INSERT INTO dbo.CourseGrade (CourseID, StudentID, Grade)
VALUES (1050, 26, 3.5);
INSERT INTO dbo.CourseGrade (CourseID, StudentID, Grade)
VALUES (1061, 26, 3);
INSERT INTO dbo.CourseGrade (CourseID, StudentID, Grade)
VALUES (1061, 27, 3);
INSERT INTO dbo.CourseGrade (CourseID, StudentID, Grade)
VALUES (1045, 28, 2.5);
INSERT INTO dbo.CourseGrade (CourseID, StudentID, Grade)
VALUES (1050, 28, 3.5);
INSERT INTO dbo.CourseGrade (CourseID, StudentID, Grade)
VALUES (1061, 29, 4);
INSERT INTO dbo.CourseGrade (CourseID, StudentID, Grade)
VALUES (1050, 30, 3.5);
INSERT INTO dbo.CourseGrade (CourseID, StudentID, Grade)
VALUES (1061, 30, 4);
GO
