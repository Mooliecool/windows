CREATE TABLE [dbo].[Accounts](
    [Id] [uniqueidentifier] NOT NULL,
    [Name] [nvarchar](max) NULL,
    [Date] [datetime] NULL,
 CONSTRAINT [PK_Accounts] PRIMARY KEY NONCLUSTERED 
(
    [Id] ASC
)
)


ALTER TABLE [dbo].[Accounts] ADD  CONSTRAINT [DF__Accounts__Date__7C8480AE]  DEFAULT (getdate()) FOR [Date]


CREATE CLUSTERED INDEX [idxDate] ON [dbo].[Accounts] 
(
    [Date] ASC
)