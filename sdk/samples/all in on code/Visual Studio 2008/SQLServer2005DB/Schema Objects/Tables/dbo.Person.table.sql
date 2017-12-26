CREATE TABLE [dbo].[Person](
	[PersonID] [int] IDENTITY(1,1) NOT NULL,
	[LastName] [nvarchar](50) NOT NULL,
	[FirstName] [nvarchar](50) NOT NULL,
	[PersonCategory] [smallint] NOT NULL CONSTRAINT [DF_Person_PersonCategory]  DEFAULT ((1)),
	[HireDate] [datetime] NULL,
	[EnrollmentDate] [datetime] NULL,
	[Picture] [image] NULL,
 CONSTRAINT [PK_School.Student] PRIMARY KEY CLUSTERED 
(
	[PersonID] ASC
)WITH (IGNORE_DUP_KEY = OFF) ON [PRIMARY]
) ON [PRIMARY]
