USE [Site Bookmarks]
GO

/****** Object:  Table [dbo].[Bookmarks]    Script Date: 01/27/2012 17:35:28 ******/
SET ANSI_NULLS ON
GO

SET QUOTED_IDENTIFIER ON
GO

CREATE TABLE [dbo].[Bookmarks](
	[SiteID] [nchar](10) NOT NULL,
	[SiteName] [nchar](10) NULL,
	[Long.] [nchar](10) NULL,
	[Lat.] [nchar](10) NULL,
	[URL] [nchar](10) NULL,
	[Address] [nchar](10) NULL,
	[State] [nchar](10) NULL,
	[Zip Code] [nchar](10) NULL
) ON [PRIMARY]

GO


