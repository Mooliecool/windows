ALTER TABLE [dbo].[CourseGrade]  WITH CHECK ADD  
       CONSTRAINT [FK_CourseGrade_Course] FOREIGN KEY([CourseID])
REFERENCES [dbo].[Course] ([CourseID])
GO
ALTER TABLE [dbo].[CourseGrade] CHECK 
       CONSTRAINT [FK_CourseGrade_Course]
