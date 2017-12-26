ALTER TABLE [dbo].[Score]
	ADD CONSTRAINT [FK_Score_Course] 
	FOREIGN KEY ([cId])
	REFERENCES [dbo].[Course] ([cId])

