Imports System.Text
Imports System.Collections.Generic
Imports System.Linq
Imports Microsoft.VisualStudio.TestTools.UnitTesting
Imports AzureBingMaps.UnitTest.TravelDataServiceReference

<TestClass()> _
Public Class DataServiceTest
    <TestMethod()> _
    Public Sub AddToTravelTest()
        Dim ctx As New TravelDataServiceContext(New Uri("http://localhost:28452/DataService/TravelDataService.svc"))
        Dim travel As New Travel() With { _
          .PartitionKey = "Lanxing", _
          .RowKey = Guid.NewGuid(), _
          .Place = "Shanghai", _
          .Latitude = 31, _
          .Longitude = 121, _
          .Time = DateTime.Now _
        }
        ctx.AddObject("Travels", travel)
        ctx.SaveChanges()
    End Sub

    <TestMethod()> _
    Public Sub UpdateTravelTest()
        Dim ctx As New TravelDataServiceContext(New Uri("http://localhost:28452/DataService/TravelDataService.svc"))
        Dim travel As Travel = ctx.Travels.First()
        travel.Place = "New York"
        ctx.UpdateObject(travel)
        ctx.SaveChanges()
    End Sub

    <TestMethod()> _
    Public Sub DeleteTravelTest()
        Dim ctx As New TravelDataServiceContext(New Uri("http://localhost:28452/DataService/TravelDataService.svc"))
        Dim travel As Travel = ctx.Travels.First()
        ctx.DeleteObject(travel)
        ctx.SaveChanges()
    End Sub
End Class