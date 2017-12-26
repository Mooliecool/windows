'****************************** Module Header ******************************'
' Module Name:	IUpdatableLinqToSQL.vb
' Project:		VBADONETDataService
' Copyright (c) Microsoft Corporation.
' 
' IUpdatableLinqToSQL.cs is the implementation of ADO.NET Data Service
' IUpdatable interface for Linq to SQL data sources.  It also contains
' partial Linq to SQL entity classes to set the DataServiceKey fields. 
' The source code of the implementation of ADO.NET Data Service IUpdatable
' interface for Linq to SQL is downloaded from: 
' http://code.msdn.microsoft.com/IUpdateableLinqToSql
' 
' This source is subject to the Microsoft Public License.
' See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
' All other rights reserved.
' 
' THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND, 
' EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED 
' WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
'***************************************************************************'

#Region "Imports directives"
Imports Microsoft.VisualBasic
Imports System.Data.Services
Imports System.Data.Linq
Imports System.Data.Linq.Mapping
Imports System.Reflection
Imports System.Data.Services.Common
#End Region


Namespace LinqToSQL

#Region "DataContext partial class"
    Partial Public Class SchoolLinqToSQLDataContext
        Implements IUpdatable

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
            Dim t As Type = Type.GetType(fullTypeName, True)
            Dim table As ITable = GetTable(t)
            Dim resource As Object = Activator.CreateInstance(t)
            table.InsertOnSubmit(resource)
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
            Dim pi = targetResource.GetType.GetProperty(propertyName)
            If pi Is Nothing Then Throw New Exception("Can't find property")
            Dim collection As IList = DirectCast(pi.GetValue(targetResource, _
                                                             Nothing), IList)
            collection.Add(resourceToBeAdded)
        End Sub

        ''' <summary>
        ''' REvert all the pending changes
        ''' </summary>
        Public Sub ClearChanges() Implements System.Data.Services.IUpdatable. _
        ClearChanges
            'See issue # 2 in Code Gallery
            'No clear way to reset this with LINQ to SQL

            'Comment out the following line if you'd prefer a silent failure
            Throw New NotSupportedException
        End Sub

        ''' <summary>
        ''' Delete the given resource
        ''' </summary>
        ''' <param name="targetResource">resource that needs to be deleted
        ''' </param>
        Public Sub DeleteResource(ByVal targetResource As Object) Implements _
        System.Data.Services.IUpdatable.DeleteResource
            Dim table As ITable = GetTable(targetResource.GetType)
            table.DeleteOnSubmit(targetResource)
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
            Dim resource As Object = query.Cast(Of Object).SingleOrDefault

            ' fullTypeName can be Nothing for delegates
            If fullTypeName IsNot Nothing AndAlso resource.GetType.FullName _
            <> fullTypeName Then _
                Throw New Exception("Unexpected type for resource")
            Return resource
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
            Dim table As MetaTable = Mapping.GetTable(targetResource.GetType)
            Dim member As MetaDataMember = table.RowType.DataMembers.Single( _
            Function(x) x.Name = propertyName)
            Return member.MemberAccessor.GetBoxedValue(targetResource)
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
            Dim pi As PropertyInfo = targetResource.GetType.GetProperty( _
            propertyName)
            If pi Is Nothing Then Throw New Exception("Can't find property")
            Dim collection As IList = DirectCast(pi.GetValue(targetResource, _
                                                             Nothing), IList)
            collection.Remove(resourceToBeRemoved)
        End Sub

        ''' <summary>
        ''' REsets the value of the given resource to its default value
        ''' </summary>
        ''' <param name="resource">resource whose value needs to be reset
        ''' </param>
        ''' <returns>same resource with its value reset</returns>
        Public Function ResetResource(ByVal resource As Object) As Object _
        Implements System.Data.Services.IUpdatable.ResetResource
            Dim t As Type = resource.GetType
            Dim table As MetaTable = Mapping.GetTable(t)
            Dim dummyResource As Object = Activator.CreateInstance(t)
            For Each member In table.RowType.DataMembers
                If Not member.IsPrimaryKey AndAlso Not member.IsDeferred AndAlso _
                    Not member.IsAssociation AndAlso Not member.IsDbGenerated Then

                    Dim defaultValue As Object = member.MemberAccessor. _
                    GetBoxedValue(dummyResource)
                    member.MemberAccessor.SetBoxedValue(resource, defaultValue)
                End If
            Next
            Return resource
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
            SubmitChanges()
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
            Dim table As MetaTable = Mapping.GetTable(targetResource.GetType)
            Dim member As MetaDataMember = table.RowType.DataMembers.Single( _
            Function(x) x.Name = propertyName)
            member.MemberAccessor.SetBoxedValue(targetResource, propertyValue)
        End Sub
    End Class
#End Region

#Region "Linq to SQL entity partial classes"
    ' Set key filed of the Course class to 'CourseID'
    <DataServiceKey("CourseID")> _
    Partial Public Class Course

    End Class

    ' Set key filed of the CourseGrade class to 'EnrollmentID'
    <DataServiceKey("EnrollmentID")> _
    Partial Public Class CourseGrade

    End Class

    ' Set key filed of the CourseInstructor class to 'CourseID' and 'PersonID'
    <DataServiceKey("CourseID", "PersonID")> _
    Partial Public Class CourseInstructor

    End Class

    ' Set key filed of the Person class to 'PersonID'
    <DataServiceKey("PersonID")> _
    Partial Public Class Person

    End Class
#End Region

End Namespace