'****************************** Module Header ******************************\
' Module Name:    SchoolDB.vb
' Project:        VBLinqToSQL
' Copyright (c) Microsoft Corporation.
'
' Manually create the object model of Student / Course / CourseGrade.
' 
' This source is subject to the Microsoft Public License.
' See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
' All other rights reserved.
'
' History:
' * 5/10/2009 08:00 PM Lingzhi Sun Created
'***************************************************************************/

#Region "Imports directory"
Imports System.Data.Linq.Mapping
Imports System.Data.Linq
#End Region


Namespace VBLinqToSQL.Manual

    <Table(Name:="Person")> _
    Public Class Student

        Sub New()

            Me._CourseGrades = New EntitySet(Of CourseGrade)()

            ' Set the default value of the column PersonCategory
            Me._PersonCategory = 1

        End Sub

#Region "Private Fields"
        Private _PersonID As Integer

        Private _LastName As String

        Private _FirstName As String

        Private _EnrollmentDate As Nullable(Of DateTime)

        Private _HireDate As Nullable(Of DateTime)

        Private _Picture As Byte()

        Private _PersonCategory As Short

        Private _CourseGrades As EntitySet(Of CourseGrade)
#End Region


        ' Refer to the ID column in database.
        <Column(IsPrimaryKey:=True, Name:="PersonID", IsDbGenerated:=True, _
                AutoSync:=AutoSync.OnInsert, _
                DbType:="Int NOT NULL IDENTITY")> _
        Public Property PersonID() As Integer
            Get
                Return _PersonID
            End Get
            Set(ByVal value As Integer)
                _PersonID = value
            End Set
        End Property


        ' Refer to the LastName column in database
        <Column()> _
        Public Property LastName() As String
            Get
                Return _LastName
            End Get
            Set(ByVal value As String)
                _LastName = value
            End Set
        End Property


        ' Refer to the FirstName column in database
        <Column()> _
        Public Property FirstName() As String
            Get
                Return _FirstName
            End Get
            Set(ByVal value As String)
                _FirstName = value
            End Set
        End Property


        ' Refer to the EnrollmentDate column in database
        <Column()> _
        Public Property EnrollmentDate() As Nullable(Of DateTime)
            Get
                Return _EnrollmentDate
            End Get
            Set(ByVal value As Nullable(Of DateTime))
                _EnrollmentDate = value
            End Set
        End Property


        ' Refer to the HireDate column in database
        <Column()> _
        Public Property HireDate() As Nullable(Of DateTime)
            Get
                Return _HireDate
            End Get
            Set(ByVal value As Nullable(Of DateTime))
                _HireDate = value
            End Set
        End Property


        ' Refer to the Picture column in database
        <Column()> _
        Public Property Picture() As Byte()
            Get
                Return _Picture
            End Get
            Set(ByVal value As Byte())
                _Picture = value
            End Set
        End Property


        ' Refer to the PersonCategory column in database
        <Column()> _
        Public Property PersonCategory() As Short
            Get
                Return _PersonCategory
            End Get
            Set(ByVal value As Short)
                _PersonCategory = value
            End Set
        End Property


        ' Refer to the FK of FK_CourseGrade_Student 
        <Association(Storage:="_CourseGrades", OtherKey:="StudentID")> _
        Public Property CourseGrades() As EntitySet(Of CourseGrade)
            Get
                Return _CourseGrades
            End Get
            Set(ByVal value As EntitySet(Of CourseGrade))
                _CourseGrades.Assign(value)
            End Set
        End Property

    End Class



    <Table(Name:="Course")> _
    Public Class Course

        Sub New()

            Me._Grades = New EntitySet(Of CourseGrade)()

        End Sub

#Region "Private Fields"
        Private _CourseID As Integer

        Private _Title As String

        Private _Credits As Integer

        Private _Grades As EntitySet(Of CourseGrade)
#End Region


        ' Refer to the CourseID column in database
        <Column(IsPrimaryKey:=True, Name:="CourseID")> _
        Public Property CourseID() As Integer
            Get
                Return _CourseID
            End Get
            Set(ByVal value As Integer)
                _CourseID = value
            End Set
        End Property


        ' Refer to the Title column in database
        <Column()> _
        Public Property Title() As String
            Get
                Return _Title
            End Get
            Set(ByVal value As String)
                _Title = value
            End Set
        End Property


        ' Refer to the Credits column in database
        <Column()> _
        Public Property Credits() As Integer
            Get
                Return _Credits
            End Get
            Set(ByVal value As Integer)
                _Credits = value
            End Set
        End Property


        ' Refer to the FK for FK_CourseGrade_Course
        <Association(Storage:="_Grades", OtherKey:="CourseID")> _
        Public Property Grades() As EntitySet(Of CourseGrade)
            Get
                Return _Grades
            End Get
            Set(ByVal value As EntitySet(Of CourseGrade))
                _Grades.Assign(value)
            End Set
        End Property

    End Class



    <Table(Name:="CourseGrade")> _
    Public Class CourseGrade

        Sub New()
            Me._Course = New EntityRef(Of Course)
            Me._Student = New EntityRef(Of Student)
        End Sub

#Region "Private Fields"
        Private _EnrollmentID As Integer

        Private _CourseID As Integer

        Private _StudentID As Integer

        Private _Grade As Decimal

        Private _Course As EntityRef(Of Course)

        Private _Student As EntityRef(Of Student)
#End Region


        ' Refer to the EnrollmentID column in database
        <Column(IsPrimaryKey:=True, Name:="EnrollmentID", _
                IsDbGenerated:=True)> _
        Public Property EnrollmentID() As Integer
            Get
                Return _EnrollmentID
            End Get
            Set(ByVal value As Integer)
                _EnrollmentID = value
            End Set
        End Property


        ' Refer to the StudentID column in database
        <Column()> _
        Public Property StudentID() As Integer
            Get
                Return _StudentID
            End Get
            Set(ByVal value As Integer)
                _StudentID = value
            End Set
        End Property


        ' Refer to the CourseID column in database
        <Column()> _
        Public Property CourseID() As Integer
            Get
                Return _CourseID
            End Get
            Set(ByVal value As Integer)
                _CourseID = value
            End Set
        End Property


        ' Refer to the Grade column in database
        <Column()> _
        Public Property Grade() As Decimal
            Get
                Return _Grade
            End Get
            Set(ByVal value As Decimal)
                _Grade = value
            End Set
        End Property


        ' Refer to the FK of FK_CourseGrade_Course
        <Association(Storage:="_Course", ThisKey:="CourseID")> _
        Public Property Course() As Course
            Get
                Return _Course.Entity
            End Get
            Set(ByVal value As Course)
                _Course.Entity = value
            End Set
        End Property


        ' Refer to the FK of FK_CourseGrade_Student 
        <Association(Storage:="_Student", ThisKey:="StudentID")> _
        Public Property Student() As Student
            Get
                Return _Student.Entity
            End Get
            Set(ByVal value As Student)
                _Student.Entity = value
            End Set
        End Property

    End Class


    Public Class SchoolDataContext
        Inherits DataContext

        Public Students As Table(Of Student)

        Public Courses As Table(Of Course)

        Public CourseGrades As Table(Of CourseGrade)

        Sub New(ByVal connection As String)
            MyBase.New(connection)
        End Sub

    End Class


End Namespace
