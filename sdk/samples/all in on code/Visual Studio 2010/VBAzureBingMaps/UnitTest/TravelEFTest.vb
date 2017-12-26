Imports AzureBingMaps.DAL
Imports Microsoft.VisualStudio.TestTools.UnitTesting
Imports System.Linq
Imports Microsoft.SqlServer.Types


''' <summary>
'''This is a test class for TravelViewTest and is intended
'''to contain all TravelViewTest Unit Tests
'''</summary>
<TestClass()> _
Public Class TravelEFTest


    Private testContextInstance As TestContext

    ''' <summary>
    '''Gets or sets the test context which provides
    '''information about and functionality for the current test run.
    '''</summary>
    Public Property TestContext() As TestContext
        Get
            Return testContextInstance
        End Get
        Set(ByVal value As TestContext)
            testContextInstance = value
        End Set
    End Property

#Region "Additional test attributes"
    ' 
    'You can use the following additional attributes as you write your tests:
    '
    'Use ClassInitialize to run code before running the first test in the class
    '[ClassInitialize()]
    'public static void MyClassInitialize(TestContext testContext)
    '{
    '}
    '
    'Use ClassCleanup to run code after all tests in a class have run
    '[ClassCleanup()]
    'public static void MyClassCleanup()
    '{
    '}
    '
    'Use TestInitialize to run code before running each test
    '[TestInitialize()]
    'public void MyTestInitialize()
    '{
    '}
    '
    'Use TestCleanup to run code after each test has run
    '[TestCleanup()]
    'public void MyTestCleanup()
    '{
    '}
    '
#End Region


    ''' <summary>
    '''A test for TravelView Constructor
    '''</summary>
    <TestMethod()> _
    Public Sub TravelViewConstructorTest()
        Dim target As New Travel()
        'Assert.Inconclusive("TODO: Implement code to verify target");
    End Sub

    <TestMethod()> _
    Public Sub InsertIntoTravelTest()
        Dim ctx As New TravelModelContainer()
        Dim travel = CreateTravelViewTest()
        ctx.Travels.AddObject(travel)
        Dim result As Integer = ctx.SaveChanges()
        Assert.AreEqual(result, 1)
    End Sub

    <TestMethod()> _
    Public Sub UpdateTravelTest()
        Dim ctx As New TravelModelContainer()
        Dim travel As Travel = ctx.Travels.First()
        travel.Place = "New York"
        Dim result As Integer = ctx.SaveChanges()
        Assert.AreEqual(result, 1)
        Dim returnedTravel As Travel = Me.QueryTravelItemTest(travel.PartitionKey, travel.RowKey)
        Assert.AreEqual(returnedTravel.Place, "New York")
    End Sub

    <TestMethod()> _
    Public Sub DeleteFromTravelTest()
        Dim ctx As New TravelModelContainer()
        Dim travel As Travel = ctx.Travels.First()
        ctx.Travels.DeleteObject(travel)
        Dim result As Integer = ctx.SaveChanges()
        Assert.AreEqual(result, 1)
        Dim returnedTravel As Travel = Me.QueryTravelItemTest(travel.PartitionKey, travel.RowKey, True)
        Assert.IsNull(returnedTravel)
    End Sub

    <TestMethod()> _
    Public Function QueryTravelItemTest(ByVal partitionKey As String, ByVal rowKey As Guid, Optional ByVal allowNull As Boolean = False) As Travel
        Dim ctx As New TravelModelContainer()
        Dim query = From t In ctx.Travels Where t.PartitionKey = partitionKey AndAlso t.RowKey = rowKey
        Dim travelView As Travel = query.FirstOrDefault()
        If Not allowNull Then
            Assert.IsNotNull(travelView)
        End If
        Return travelView
    End Function

    <TestMethod()> _
    Public Sub QueryFirstTravelItemTest()
        Dim ctx As New TravelModelContainer()
        Dim query = From t In ctx.Travels
        Dim travelView As Travel = query.FirstOrDefault()
        Assert.IsNotNull(travelView)
    End Sub

    ''' <summary>
    '''A test for CreateTravelView
    '''</summary>
    <TestMethod()> _
    Public Function CreateTravelViewTest() As Travel
        Dim partitionKey As String = "Lanxing"
        ' TODO: Initialize to an appropriate value
        Dim rowKey As Guid = Guid.NewGuid()
        ' TODO: Initialize to an appropriate value
        Dim place As String = "Shanghai"
        ' TODO: Initialize to an appropriate value
        Dim time As DateTime = DateTime.Now
        ' TODO: Initialize to an appropriate value
        Dim sqlGeography__1 As SqlGeography = SqlGeography.Point(31, 121, 4326)
        Dim travelView As New Travel() With { _
          .PartitionKey = partitionKey, _
          .RowKey = rowKey, _
          .Place = place, _
          .Time = time, _
          .GeoLocationText = sqlGeography__1.ToString() _
        }
        Return travelView
    End Function

    ''' <summary>
    '''A test for GeoLocation
    '''</summary>
    <TestMethod()> _
    Public Sub GeoLocationTest()
        Dim target As New Travel()
        ' TODO: Initialize to an appropriate value
        Dim expected As Byte() = Nothing
        ' TODO: Initialize to an appropriate value
        Dim actual As Byte()
        target.GeoLocation = expected
        actual = target.GeoLocation
        Assert.AreEqual(expected, actual)
        Assert.Inconclusive("Verify the correctness of this test method.")
    End Sub

    ''' <summary>
    '''A test for PartitionKey
    '''</summary>
    <TestMethod()> _
    Public Sub PartitionKeyTest()
        Dim target As New Travel()
        ' TODO: Initialize to an appropriate value
        Dim expected As String = String.Empty
        ' TODO: Initialize to an appropriate value
        Dim actual As String
        target.PartitionKey = expected
        actual = target.PartitionKey
        Assert.AreEqual(expected, actual)
        Assert.Inconclusive("Verify the correctness of this test method.")
    End Sub

    ''' <summary>
    '''A test for Place
    '''</summary>
    <TestMethod()> _
    Public Sub PlaceTest()
        Dim target As New Travel()
        ' TODO: Initialize to an appropriate value
        Dim expected As String = String.Empty
        ' TODO: Initialize to an appropriate value
        Dim actual As String
        target.Place = expected
        actual = target.Place
        Assert.AreEqual(expected, actual)
        Assert.Inconclusive("Verify the correctness of this test method.")
    End Sub

    ''' <summary>
    '''A test for RowKey
    '''</summary>
    <TestMethod()> _
    Public Sub RowKeyTest()
        Dim target As New Travel()
        ' TODO: Initialize to an appropriate value
        Dim expected As New Guid()
        ' TODO: Initialize to an appropriate value
        Dim actual As Guid
        target.RowKey = expected
        actual = target.RowKey
        Assert.AreEqual(expected, actual)
    End Sub

    ''' <summary>
    '''A test for Time
    '''</summary>
    <TestMethod()> _
    Public Sub TimeTest()
        Dim target As New Travel()
        ' TODO: Initialize to an appropriate value
        Dim expected As New DateTime()
        ' TODO: Initialize to an appropriate value
        Dim actual As DateTime
        target.Time = expected
        actual = target.Time
        Assert.AreEqual(expected, actual)
        Assert.Inconclusive("Verify the correctness of this test method.")
    End Sub
End Class