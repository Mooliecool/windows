ALTER TABLE [dbo].[CourseInstructor]  WITH CHECK ADD  
   CONSTRAINT [FK_CourseInstructor_Person] FOREIGN KEY([PersonID])
REFERENCES [dbo].[Person] ([PersonID])
GO
ALTER TABLE [dbo].[CourseInstructor] CHECK 
   CONSTRAINT [FK_CourseInstructor_Person]
