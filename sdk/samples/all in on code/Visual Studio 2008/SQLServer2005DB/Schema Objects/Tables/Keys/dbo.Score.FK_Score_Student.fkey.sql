ALTER TABLE [dbo].[Score]
	ADD CONSTRAINT [FK_Score_Student] 
	FOREIGN KEY ([sId])
	REFERENCES [dbo].[Student] ([sId])

