CREATE VIEW [dbo].[TravelView]
AS
SELECT     PartitionKey, RowKey, Place, CAST(GeoLocation AS varbinary(MAX)) AS GeoLocation, Time
FROM         dbo.Travel
