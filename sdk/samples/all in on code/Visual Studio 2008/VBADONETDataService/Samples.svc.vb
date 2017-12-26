'****************************** Module Header ******************************'
' Module Name:	Samples.svc.vb
' Project:		VBADONETDataService
' Copyright (c) Microsoft Corporation.
' 
' Samples.svc demonstrates the ADO.NET Data Service for non-relational data
' source.  The non-relational data source is some in-memory objects which
' hold the sample projects information. The non-relational entity class also
' implements the IUpdatable interface to let the client insert new data.
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
Imports System.Data.Services.Common
Imports System.Reflection

#End Region

Public Class Samples
    Inherits DataService(Of SampleProjects)

    ' This method is called only once to initialize service-wide policies.
    Public Shared Sub InitializeService(ByVal config As  _
                                        IDataServiceConfiguration)
        ' Set rules to indicate which entity sets and service operations
        ' are visible, updatable, etc.
        config.UseVerboseErrors = True
        config.SetEntitySetAccessRule("*", EntitySetRights.All)
    End Sub
End Class


#Region "Non-relational data entity classes"
' Sample project entity class with DataServiceKey 'ProjectName'
<DataServiceKey("ProjectName")> _
Public Class Project

    Private _projectName As String
    Private _owner As String
    Private _projectCategory As Category

    Public Property ProjectName() As String
        Get
            Return _projectName
        End Get
        Set(ByVal value As String)
            _projectName = value
        End Set
    End Property

    Public Property Owner() As String
        Get
            Return _owner
        End Get
        Set(ByVal value As String)
            _owner = value
        End Set
    End Property

    Public Property ProjectCategory() As Category
        Get
            Return _projectCategory
        End Get
        Set(ByVal value As Category)
            _projectCategory = value
        End Set
    End Property

End Class

' Sample project category entity class with DataServiceKey 'CategoryName'
<DataServiceKey("CategoryName")> _
Public Class Category

    Private _categoryName As String

    Public Property CategoryName() As String
        Get
            Return _categoryName
        End Get
        Set(ByVal value As String)
            _categoryName = value
        End Set
    End Property

End Class

' Sample data entity class
Public Class SampleProjects
    Implements IUpdatable

    Shared _categories As List(Of Category)
    Shared _projects As List(Of Project)

    ' Static constructor
    Shared Sub New()

        ' Initialize the sample project category list
        _categories = New List(Of Category)()
        _categories.Add(New Category With {.CategoryName = "COM"})
        _categories.Add(New Category With {.CategoryName = "Data Access"})
        _categories.Add(New Category With {.CategoryName = "Office"})
        _categories.Add(New Category With {.CategoryName = "IPC and RPC"})
        _categories.Add(New Category With {.CategoryName = "WinForm"})
        _categories.Add(New Category With {.CategoryName = "Hook"})

        ' Initialize the sample project list
        _projects = New List(Of Project)()
        _projects.Add(New Project With _
        {.ProjectName = "CSDllCOMServer", .Owner = "Jialiang Ge", .ProjectCategory = _categories(0)})
        _projects.Add(New Project With _
        {.ProjectName = "VBDllCOMServer", .Owner = "Jialiang Ge", .ProjectCategory = _categories(0)})
        _projects.Add(New Project With _
        {.ProjectName = "ATLDllCOMServer", .Owner = "Jialiang Ge", .ProjectCategory = _categories(0)})
        _projects.Add(New Project With _
        {.ProjectName = "CSUseADONET", .Owner = "Lingzhi Sun", .ProjectCategory = _categories(1)})
        _projects.Add(New Project With _
        {.ProjectName = "CppUseADONET", .Owner = "Jialiang Ge", .ProjectCategory = _categories(1)})
        _projects.Add(New Project With _
        {.ProjectName = "CSLinqToObject", .Owner = "Colbert Zhou", .ProjectCategory = _categories(1)})
        _projects.Add(New Project With _
        {.ProjectName = "CSLinqToSQL", .Owner = "Rongchun Zhang", .ProjectCategory = _categories(1)})
        _projects.Add(New Project With _
        {.ProjectName = "CSOutlookUIDesigner", .Owner = "Jie Wang", .ProjectCategory = _categories(2)})
        _projects.Add(New Project With _
        {.ProjectName = "CSOutlookRibbonXml", .Owner = "Jie Wang", .ProjectCategory = _categories(2)})
        _projects.Add(New Project With _
        {.ProjectName = "CSAutomateExcel", .Owner = "Jialiang Ge", .ProjectCategory = _categories(2)})
        _projects.Add(New Project With _
        {.ProjectName = "VBAutomateExcel", .Owner = "Jialiang Ge", .ProjectCategory = _categories(2)})
        _projects.Add(New Project With _
        {.ProjectName = "CppFileMappingServer", .Owner = "Hongye Sun", .ProjectCategory = _categories(3)})
        _projects.Add(New Project With _
        {.ProjectName = "CppFileMappingClient", .Owner = "Hongye Sun", .ProjectCategory = _categories(3)})
        _projects.Add(New Project With _
        {.ProjectName = "CSReceiveWM_COPYDATA", .Owner = "Riquel Dong", .ProjectCategory = _categories(3)})
        _projects.Add(New Project With _
        {.ProjectName = "CSSendWM_COPYDATA", .Owner = "Riquel Dong", .ProjectCategory = _categories(3)})
        _projects.Add(New Project With _
        {.ProjectName = "CSWinFormGeneral", .Owner = "Zhixin Ye", .ProjectCategory = _categories(4)})
        _projects.Add(New Project With _
        {.ProjectName = "CSWinFormDataBinding", .Owner = "Zhixin Ye", .ProjectCategory = _categories(4)})
        _projects.Add(New Project With _
        {.ProjectName = "CSWindowsHook", .Owner = "Rongchun Zhang", .ProjectCategory = _categories(5)})
    End Sub

    ' Public property to get the sample projects information from the
    ' ADO.NET Data Service client side
    Public ReadOnly Property Projects() As IQueryable(Of Project)
        Get
            Return _projects.AsQueryable()
        End Get
    End Property

    ' Public property to get the sample projects categoryies information 
    ' from the ADO.NET Data Service client side
    Public ReadOnly Property Categories() As IQueryable(Of Category)
        Get
            Return _categories.AsQueryable()
        End Get
    End Property

#Region "IUpdatable Members"

    ' Save the added object temporarily
    Private tempObj As Object = Nothing

    ''' <summary>
    ''' Creates the resource of the given tpe and belonging to the given
    ''' container
    ''' </summary>
    ''' <param name="containerName">container name to which the resource 
    ''' needs to be added</param>
    ''' <param name="fullTypeName">full type name i.e. Namespace qualified 
    ''' type name of the resource</param>
    ''' <returns>object representing a resource of given type and 
    ''' belonging to the given container</returns>
    ''' <remarks></remarks>
    Function CreateResource(ByVal containerName As String, ByVal _
                            fullTypeName As String) As Object Implements _
                            IUpdatable.CreateResource

        ' Get the type of the resource
        Dim t As Type = Type.GetType(fullTypeName, True)

        ' Create an instance of the resource type
        Dim resource As Object = Activator.CreateInstance(t)

        ' Return the resource object
        Return resource
    End Function

    ''' <summary>
    ''' Adds the given value to the collection
    ''' </summary>
    ''' <param name="targetResource">target object which defines the 
    ''' property</param>
    ''' <param name="propertyName">name of the property whose value needs 
    ''' to be updated</param>
    ''' <param name="resourceToBeAdded">value of the property which needs 
    ''' to be added</param>
    ''' <remarks></remarks>
    Public Sub AddReferenceToCollection(ByVal targetResource As Object, _
                                        ByVal propertyName As String, _
                                        ByVal resourceToBeAdded As Object) _
                                        Implements System.Data.Services. _
                                        IUpdatable.AddReferenceToCollection
        ' Get the target object type
        Dim t As Type = targetResource.GetType()

        ' Get the property to be updated
        Dim pi = t.GetProperty(propertyName)
        If Not pi Is Nothing Then
            ' Retrieve the collection property value
            Dim collection As IList = DirectCast(pi.GetValue(targetResource, _
                                                         Nothing), IList)
            ' Add the resource into the collection
            collection.Add(resourceToBeAdded)
        End If
    End Sub

    ''' <summary>
    ''' REvert all the pending changes
    ''' </summary>
    Public Sub ClearChanges() Implements System.Data.Services.IUpdatable. _
    ClearChanges
        Throw New NotImplementedException
    End Sub

    ''' <summary>
    ''' Delete the given resource
    ''' </summary>
    ''' <param name="targetResource">resource that needs to be deleted
    ''' </param>
    Public Sub DeleteResource(ByVal targetResource As Object) Implements _
    System.Data.Services.IUpdatable.DeleteResource
        Throw New NotImplementedException
    End Sub

    ''' <summary>
    ''' Gets the resource of the given type that the query points to
    ''' </summary>
    ''' <param name="query">query pointing to a particular resource
    ''' </param>
    ''' <param name="fullTypeName">full name i.e. Namespace qualified 
    ''' type name of the resource</param>
    ''' <returns>object representing a resource of given type and as 
    ''' referenced by the query</returns>
    Public Function GetResource(ByVal query As System.Linq.IQueryable, _
                                ByVal fullTypeName As String) As Object _
                                Implements System.Data.Services.IUpdatable. _
                                GetResource
        Throw New NotImplementedException
    End Function

    ''' <summary>
    ''' Gets the value of the given property on the target object
    ''' </summary>
    ''' <param name="targetResource">target object which defines the 
    ''' property</param>
    ''' <param name="propertyName">name of the property whose value needs 
    ''' to be updated</param>
    ''' <returns>the value of the property for the given target resource
    ''' </returns>
    Public Function GetValue(ByVal targetResource As Object, ByVal _
                             propertyName As String) As Object Implements _
                             System.Data.Services.IUpdatable.GetValue
        ' Get the target object type
        Dim t As Type = targetResource.GetType()

        ' Get the property
        Dim pi = t.GetProperty(propertyName)

        If Not pi Is Nothing Then
            ' Return property value
            Return pi.GetValue(targetResource, Nothing)
        Else
            Return Nothing
        End If
    End Function

    ''' <summary>
    ''' Removes the given value from the collection
    ''' </summary>
    ''' <param name="targetResource">target object which defines the 
    ''' property</param>
    ''' <param name="propertyName">name of the property whose value needs 
    ''' to be updated</param>
    ''' <param name="resourceToBeRemoved">value of the property which needs 
    ''' to be removed</param>
    Public Sub RemoveReferenceFromCollection(ByVal targetResource As Object, _
                                             ByVal propertyName As String, _
                                             ByVal resourceToBeRemoved As Object) _
                                             Implements System.Data.Services. _
                                             IUpdatable.RemoveReferenceFromCollection
        Throw New NotImplementedException
    End Sub

    ''' <summary>
    ''' REsets the value of the given resource to its default value
    ''' </summary>
    ''' <param name="resource">resource whose value needs to be reset
    ''' </param>
    ''' <returns>same resource with its value reset</returns>
    Public Function ResetResource(ByVal resource As Object) As Object _
    Implements System.Data.Services.IUpdatable.ResetResource
        Throw New NotImplementedException
    End Function

    ''' <summary>
    ''' Returns the actual instance of the resource represented by the 
    ''' given resource object
    ''' </summary>
    ''' <param name="resource">object representing the resource whose 
    ''' instance needs to be fetched</param>
    ''' <returns>The actual instance of the resource represented by the 
    ''' given resource object</returns>
    Public Function ResolveResource(ByVal resource As Object) As Object _
    Implements System.Data.Services.IUpdatable.ResolveResource
        Return resource
    End Function

    ''' <summary>
    ''' Saves all the pending changes made till now
    ''' </summary>
    Public Sub SaveChanges() Implements System.Data.Services.IUpdatable. _
    SaveChanges
        ' Add the temp object into the local collection
        If Not tempObj Is Nothing Then
            Dim t = tempObj.GetType()
            If t.Name = "Category" Then
                SampleProjects._categories.Add(DirectCast(tempObj, Category))
            ElseIf t.Name = "Project" Then
                SampleProjects._projects.Add(DirectCast(tempObj, Project))
            End If
        End If
    End Sub

    ''' <summary>
    ''' Sets the value of the given reference property on the target 
    ''' object
    ''' </summary>
    ''' <param name="targetResource">target object which defines the 
    ''' property</param>
    ''' <param name="propertyName">name of the property whose value 
    ''' needs to be updated</param>
    ''' <param name="propertyValue">value of the property</param>
    ''' <remarks></remarks>
    Public Sub SetReference(ByVal targetResource As Object, ByVal _
                            propertyName As String, ByVal propertyValue _
                            As Object) Implements System.Data.Services. _
                            IUpdatable.SetReference
        DirectCast(Me, IUpdatable).SetValue(targetResource, _
                                            propertyName, propertyValue)
    End Sub

    ''' <summary>
    ''' Sets the value of the given property on the target object
    ''' </summary>
    ''' <param name="targetResource">target object which defines the 
    ''' property</param>
    ''' <param name="propertyName">name of the property whose value 
    ''' needs to be updated</param>
    ''' <param name="propertyValue">value of the property</param>
    ''' <remarks></remarks>
    Public Sub SetValue(ByVal targetResource As Object, ByVal _
                        propertyName As String, ByVal propertyValue _
                        As Object) Implements System.Data.Services. _
                        IUpdatable.SetValue
        ' Get the resource object type
        Dim t As Type = targetResource.GetType()

        ' Get the property to be updated
        Dim pi = t.GetProperty(propertyName)

        If Not pi Is Nothing Then
            ' Set the property value
            pi.SetValue(targetResource, propertyValue, Nothing)
        End If

        ' Save the target object to temp added object
        tempObj = targetResource
    End Sub
#End Region

End Class
#End Region
