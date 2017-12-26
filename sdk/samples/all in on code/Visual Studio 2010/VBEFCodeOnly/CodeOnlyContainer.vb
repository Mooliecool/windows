'****************************** Module Header ******************************\
' Module Name:    CodeOnlyContainer.vb
' Project:        VBEFCodeOnly
' Copyright (c) Microsoft Corporation.
'
' This code file contains the POCO entity classes and their custom 
' ObjectContext object.  The POCO entity classes include Type-per-Hierarchy
' inheritance entities, Type-per-Table inheritance entities, other relational 
' entities, and Complex Type classes.
'
' This source is subject to the Microsoft Public License.
' See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
' All other rights reserved.
'
' THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND, 
' EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED 
' WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
'***************************************************************************/

#Region "Imports Directive"
Imports System.Data.Objects
Imports System.Data.EntityClient
#End Region

' The custom ObjectContext for the POCO entities
Public Class CodeOnlyContainer
    Inherits ObjectContext
    Sub New(ByVal connection As EntityConnection)
        MyBase.New(connection)
        ' Set the DefaultContainerName
        DefaultContainerName = "CodeOnlyContainer"
    End Sub

#Region "IObjectSet<> collections"
    Public ReadOnly Property Courses As IObjectSet(Of Course)
        Get
            Return MyBase.CreateObjectSet(Of Course)()
        End Get
    End Property
    Public ReadOnly Property CourseStudents As IObjectSet(Of CourseStudent)
        Get
            Return MyBase.CreateObjectSet(Of CourseStudent)()
        End Get
    End Property

    Public ReadOnly Property Departments As IObjectSet(Of Department)
        Get
            Return MyBase.CreateObjectSet(Of Department)()
        End Get
    End Property

    Public ReadOnly Property PeopleTPH As IObjectSet(Of PersonTPH)
        Get
            Return MyBase.CreateObjectSet(Of PersonTPH)()
        End Get
    End Property

    Public ReadOnly Property PeopleTPT As IObjectSet(Of PersonTPT)
        Get
            Return MyBase.CreateObjectSet(Of PersonTPT)()
        End Get
    End Property
#End Region
End Class

#Region "Type-per-Table inheritance POCO entities"
Public Class PersonTPT
    ' PK property
    Public Property PersonID As Integer

    ' Complex Type properties
    Public Property Name As Name
    Public Property Address As Address
End Class

Public Class InstructorTPT
    Inherits PersonTPT
    Public Property HireDate As DateTime

    ' Navigation property
    Public Property Courses As ICollection(Of Course)
End Class

Public Class AdminTPT
    Inherits PersonTPT
    Public Property AdminDate As DateTime
End Class

Public Class StudentTPT
    Inherits PersonTPT
    Public Property EnrollmentDate As DateTime
    Public Property Degree As Integer?
    Public Property Credits As Integer?

    ' Navigation property
    Public Property CourseStudents As ICollection(Of CourseStudent)
End Class

Public Class BusinessStudentTPT
    Inherits StudentTPT
    Public Property BusinessCredits As Integer?
End Class
#End Region

#Region "Other relational POCO entities"
Public Class Department
    ' PK property
    Public Property DepartmentID As Integer

    Public Property Name As String
    Public Property Budget As Decimal?
    Public Property StartDate As DateTime

    ' Navigation property
    Public Property Courses As ICollection(Of Course)
End Class

Public Class Course
    ' PK property
    Public Property CourseID As Integer

    Public Property Title As String
    Public Property Credits As Integer

    ' Navigation properties
    Public Property Department As Department
    Public Property Instructors As ICollection(Of InstructorTPT)
    Public Property CourseStudents As ICollection(Of CourseStudent)

    ' FK Association property
    Public Property DepartmentID As Integer
End Class

Public Class CourseStudent
    ' Composite PK properties 
    Public Property PersonID As Integer
    Public Property CourseID As Integer

    Public Property Score As Integer?

    ' Navigation properties
    Public Property Student As StudentTPT
    Public Property Course As Course
End Class
#End Region

#Region "Type-per-Hierarchy inheritance POCO entities"
Public Class PersonTPH
    ' PK property
    Public Property PersonID As Integer

    ' Complex Type properties
    Public Property Name As Name
    Public Property Address As Address
End Class

Public Class InstructorTPH
    Inherits PersonTPH
    Public Property HireDate As DateTime?
End Class

Public Class StudentTPH
    Inherits PersonTPH
    Public Property EnrollmentDate As DateTime?
End Class

Public Class AdminTPH
    Inherits PersonTPH
    Public Property AdminDate As DateTime?
End Class
#End Region

#Region "Complex Type entities"
Public Class Name
    Public Property FirstName As String
    Public Property LastName As String

    ' Override the ToString method
    Public Overrides Function ToString() As String
        Return Me.FirstName & " " & Me.LastName
    End Function
End Class

Public Class Address
    Public Property City As String
    Public Property Country As String
    Public Property Zipcode As String
End Class
#End Region