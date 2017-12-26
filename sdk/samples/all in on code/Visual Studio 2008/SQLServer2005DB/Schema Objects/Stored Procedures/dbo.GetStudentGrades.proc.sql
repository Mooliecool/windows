CREATE PROCEDURE [dbo].[GetStudentGrades]
            @StudentID int
            AS
            SELECT EnrollmentID, Grade FROM dbo.CourseGrade
            WHERE StudentID = @StudentID
