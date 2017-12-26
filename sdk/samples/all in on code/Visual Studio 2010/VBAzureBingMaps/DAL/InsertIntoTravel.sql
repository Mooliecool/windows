CREATE PROCEDURE [dbo].[InsertIntoTravel]
      @PartitionKey nvarchar(200),
      @Place nvarchar(200), 
      @GeoLocation varchar(max),
      @Time datetime
AS
BEGIN
      Insert Into Travel
      Values(@PartitionKey, NEWID(), @Place, Geography::STGeomFromText(@GeoLocation, 4326), @Time)
END
