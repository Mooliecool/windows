CREATE TABLE [Images] (
	[ImageID] [int] IDENTITY (1, 1) NOT NULL ,
	[Description] [nvarchar] (100) COLLATE SQL_Latin1_General_CP1_CI_AS NULL ,
	[ImageFile] [image] NULL ,
	[ImageSize] [int] NULL ,
	[ImageType] [nvarchar] (100) COLLATE SQL_Latin1_General_CP1_CI_AS NULL 
) ON [PRIMARY] TEXTIMAGE_ON [PRIMARY]
GO


