CREATE PROCEDURE [dbo].[UpdateTravel]
	@PartitionKey nvarchar(200),
    @RowKey  uniqueidentifier,
    @Place NVarchar(200),
    @GeoLocation NVarchar(max),
    @Time datetime
AS
BEGIN
      Update [dbo].[Travel]
      Set [Place] = @Place,
      [GeoLocation] = Geography::STGeomFromText(@GeoLocation, 4326),
      [Time] = @Time
    Where PartitionKey = @PartitionKey AND RowKey = @RowKey
END
