ALTER TABLE [dbo].[CourseGrade]  WITH CHECK ADD  
       CONSTRAINT [FK_CourseGrade_Student] FOREIGN KEY([StudentID])
REFERENCES [dbo].[Person] ([PersonID])
GO
ALTER TABLE [dbo].[CourseGrade] CHECK 
       CONSTRAINT [FK_CourseGrade_Student]
