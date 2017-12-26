'****************************** Module Header ******************************'
' Module Name:	SchoolLinqToEntities.svc.vb
' Project:		VBADONETDataService
' Copyright (c) Microsoft Corporation.
' 
' SchoolLinqToEntities.svc demonstrates the ADO.NET Data Service for ADO.NET
' Entity Data Model.  The ADO.NET Entity Data Model connects to the SQL 
' Server database deployed by SQLServer2005DB.  It contains data tables: 
' Person, Course, CourseGrade, and CourseInstructor. The service also exports
' a service operation CoursesByPersonID to retrieve the instructor's Course 
' list by PersonID, a service operation GetPersonUpdateException to get the 
' person update exception, a service query interceptor to filter the Course 
' objects, and a servie change interceptor to check PersonCategory value of 
' Added or Changed Person objects.
' 
' 
' This source is subject to the Microsoft Public License.
' See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
' All other rights reserved.
' 
' THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND, 
' EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED 
' WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
'***************************************************************************'

#Region "Imports directive"
Imports System.Data.Services
Imports System.Linq
Imports System.ServiceModel.Web
Imports VBADONETDataService.LinqToEntities
Imports System.Linq.Expressions
#End Region


Public Class SchoolLinqToEntities
    Inherits DataService(Of SQLServer2005DBEntities)

    ' This method is called only once to initialize service-wide policies.
    Public Shared Sub InitializeService(ByVal config As  _
                                        IDataServiceConfiguration)
        ' Set rules to indicate which entity sets and service operations 
        ' are visible, updatable, etc.
        config.UseVerboseErrors = True
        config.SetEntitySetAccessRule("*", EntitySetRights.All)
        config.SetServiceOperationAccessRule("CoursesByPersonID", _
                                             ServiceOperationRights.All)
        config.SetServiceOperationAccessRule("GetPersonUpdateException", _
                                             ServiceOperationRights.All)
    End Sub

    ''' <summary>
    ''' A service operation that retrieve the instructor's courses list 
    ''' by primary key PersonID
    ''' </summary>
    ''' <param name="ID">The primary key PersonID</param>
    ''' <returns>An IQueryable collection contains Course objects
    ''' </returns>
    <WebGet()> _
    Public Function CoursesByPersonID(ByVal ID As Integer) As IQueryable( _
    Of Course)

        ' Check whether the PersonID is valid and it is instructor's ID 
        If Me.CurrentDataSource.Person.Any(Function(i) i.PersonID = ID And _
                                               i.PersonCategory = 2) Then
            ' Retrieve the instructor's course list
            Dim courses = From p In Me.CurrentDataSource.Person _
                          Where p.PersonID = ID _
                          Select p.Course

            ' Return the query result
            Return courses.First().AsQueryable()
        Else
            ' Throw DataServiceException if the PersonID is invalid or
            ' it is student's ID
            Throw New DataServiceException(400, _
                "PersonID is incorrect or the person is not instructor!")
        End If
    End Function

    ''' <summary>
    ''' A service operation that retrieve the person update exception
    ''' information
    ''' </summary>
    ''' <returns>An IQueryable collection contains Person objects
    ''' </returns>
    <WebGet()> _
    Public Function GetPersonUpdateException() As IQueryable(Of Person)
        Throw New DataServiceException(400, _
            "The valid value of PersonCategory is 1(for students) or " & _
            "2(for instructors).")
    End Function

    ''' <summary>
    ''' Override the HandleException method to throw 400 Bad Request
    ''' exception to the client side.
    ''' </summary>
    ''' <param name="args">The HandleException argument</param>
    Protected Overrides Sub HandleException(ByVal args As  _
                                System.Data.Services.HandleExceptionArgs)

        ' Check if the InnerException is Nothing
        If args.Exception.InnerException IsNot Nothing Then

            ' Check if the InnerException is in type of DataServiceException
            If TypeOf args.Exception.InnerException Is  _
            DataServiceException Then

                ' Convert the InnerException to DataServiceException
                Dim ex As DataServiceException = _
                DirectCast(args.Exception.InnerException, DataServiceException)

                ' Return the DataServiceException to the client
                args.Exception = ex

            End If
        End If

        MyBase.HandleException(args)
    End Sub

    ''' <summary>
    ''' A service query interceptor that filters the course objects to
    ''' return the course which CourseID is larger than 4000
    ''' </summary>
    ''' <returns>LINQ lambda expression to filter the course objects
    ''' </returns>
    <QueryInterceptor("Course")> _
    Public Function QueryCourse() As Expression(Of Func(Of Course, Boolean))

        ' LINQ lambda expression to filter the course objects
        Return Function(c) c.CourseID > 4000

    End Function

    ''' <summary>
    ''' A service change interceptor that checks the PersonCategory value
    ''' of the added or changed Person object
    ''' </summary>
    ''' <param name="p">The added or changed Person object</param>
    ''' <param name="operation">The update operation</param>
    <ChangeInterceptor("Person")> _
    Public Sub OnChangePerson(ByVal p As Person, ByVal operation As  _
                              UpdateOperations)

        ' Check whether the update operation is Add or Change
        If operation = UpdateOperations.Add Or operation = _
        UpdateOperations.Change Then

            ' Check invalid value of PersonCategory
            If Not p.PersonCategory = 1 And Not p.PersonCategory = 2 Then

                ' Throw DataServieException
                Throw New DataServiceException(400, _
                    "The valid value of PersonCategory is 1(for students)" & _
                    " or 2(for instructors).")
            End If
        End If
    End Sub
End Class
