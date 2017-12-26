ALTER TABLE [dbo].[OfficeAssignment]  WITH CHECK ADD  
   CONSTRAINT [FK_OfficeAssignment_Person] FOREIGN KEY([InstructorID])
REFERENCES [dbo].[Person] ([PersonID])
GO
ALTER TABLE [dbo].[OfficeAssignment] CHECK 
   CONSTRAINT [FK_OfficeAssignment_Person]
