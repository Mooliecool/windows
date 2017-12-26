CREATE PROCEDURE [dbo].[DeleteFromTravel]
@PartitionKey nvarchar(200),
@RowKey uniqueidentifier
AS
BEGIN
      Delete From Travel Where PartitionKey = @PartitionKey AND RowKey = @RowKey
END
