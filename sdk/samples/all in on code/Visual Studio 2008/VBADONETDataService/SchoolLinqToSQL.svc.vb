'****************************** Module Header ******************************'
' Module Name:	SchoolLinqToSQL.svc.vb
' Project:		VBADONETDataService
' Copyright (c) Microsoft Corporation.
' 
' SchoolLinqToSQL.svc demonstrates the ADO.NET Data Service for Linq to SQL
' Data Classes.  The Linq to SQL Data Class connects to the SQL Server
' database deployed by SQLServer2005DB.  It contains data tables: Person, 
' Course, CourseGrade, and CourseInstructor. The service also exports a 
' service operation SearchCourses to let the client search course objects via
' SQL commands.
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
Imports VBADONETDataService.LinqToSQL
#End Region


Public Class SchoolLinqToSQL
    Inherits DataService(Of SchoolLinqToSQLDataContext)

    ' This method is called only once to initialize service-wide policies.
    Public Shared Sub InitializeService(ByVal config As  _
                                        IDataServiceConfiguration)
        ' Set rules to indicate which entity sets and service operations 
        ' are visible, updatable, etc.
        config.UseVerboseErrors = True
        config.SetEntitySetAccessRule("*", EntitySetRights.All)
        config.SetServiceOperationAccessRule("SearchCourses", _
                                             ServiceOperationRights.All)
    End Sub

    ''' <summary>
    ''' A service operation that searches the courses via SQL commands
    ''' and returns an IQueryable collection of Course objects
    ''' </summary>
    ''' <param name="searchText">The query SQL commands</param>
    ''' <returns>An IQueryable collection contains Course objects
    ''' </returns>
    <WebGet()> _
    Public Function SearchCourses(ByVal searchText As String) As  _
    IQueryable(Of Course)

        ' Call DataContext.ExecuteQuery to call the search SQL commands
        Return Me.CurrentDataSource.ExecuteQuery(Of Course) _
        (searchText).AsQueryable()

    End Function
End Class
