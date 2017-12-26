-- Run this script against your database to create the Travel table.

CREATE TABLE [dbo].[Travel](
      [PartitionKey] [nvarchar](200) NOT NULL,
      [RowKey] [uniqueidentifier] NOT NULL,
      [Place] [nvarchar](200) NOT NULL,
      [GeoLocation] [geography] NOT NULL,
      [Time] [datetime] NOT NULL,
CONSTRAINT [PK_Travel] PRIMARY KEY CLUSTERED 
(
      [PartitionKey] ASC, [RowKey] ASC
),
CONSTRAINT [IX_Travel] UNIQUE NONCLUSTERED 
(
      [Place] ASC,
      [Time] ASC
)
)
