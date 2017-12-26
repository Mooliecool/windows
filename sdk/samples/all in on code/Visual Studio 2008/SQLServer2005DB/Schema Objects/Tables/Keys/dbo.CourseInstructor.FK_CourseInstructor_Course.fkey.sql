ALTER TABLE [dbo].[CourseInstructor]  WITH CHECK ADD  
   CONSTRAINT [FK_CourseInstructor_Course] FOREIGN KEY([CourseID])
REFERENCES [dbo].[Course] ([CourseID])
GO
ALTER TABLE [dbo].[CourseInstructor] CHECK 
   CONSTRAINT [FK_CourseInstructor_Course]
