'************************************* Module Header **************************************\
' Module Name:  MainForm.vb
' Project:      VBWinFormDataBinding
' Copyright (c) Microsoft Corporation.
' 
' The VBWinFormDataBinding sample demonstrates the Windows Forms Data Binding technology.
' Data binding in Windows Forms gives you the means to display and make changes to 
' information from a data source in controls on the form. You can bind to both traditional 
' data sources as well as almost any structure that contains data.
' Windows Forms can take advantage of two types of data binding: 
' simple binding and complex binding. You can refer to this document for more information:
' http://msdn.microsoft.com/en-us/library/c8aebh9k.aspx 
' 
' This source is subject to the Microsoft Public License.
' See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
' All other rights reserved.
' 
' History:
' * 1/16/2010 10:00 AM Kira Qian Created
'******************************************************************************************/

Imports System
Imports System.Collections.Generic
Imports System.ComponentModel
Imports System.Data
Imports System.Drawing
Imports System.Linq
Imports System.Text
Imports System.Windows.Forms

Public Class MainForm

    Private Sub MainForm_Load(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles MyBase.Load
        SimpleBindingExp1()
        SimpleBindingExp2()
        SimpleBindingExp3()
        ComplexBindingExp2()
        ComplexBindingExp3()
    End Sub

    ' ///////////////////////////////////////////
    ' Simple Binding Examples
    ' //

#Region "Simple Binding Example 1 (Bind to class property)"
    Private Sub SimpleBindingExp1()
        ' In this example the CheckBox is the data source
        Me.textBox1.DataBindings.Add("Text", Me.checkBox1, "Checked")
    End Sub
#End Region

#Region "Simple Binding Example 2 (Bind to class property)"
    Private Sub SimpleBindingExp2()
        ' In this example the Form itself is the data source.
        ' With the update mode set to DataSourceUpdateMode.Never the data source won't
        ' update unless we explicitly call the Binding.WriteValue() method.
        Dim bdSize As Binding = New Binding("Text", Me, "Size", True, DataSourceUpdateMode.Never)
        Me.textBox2.DataBindings.Add(bdSize)
    End Sub

    Private Sub btnSet_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles btnSet.Click
        Dim bdSize As Binding = textBox2.DataBindings(0)
        ' Force the value to store in the data source
        bdSize.WriteValue()
    End Sub
#End Region

#Region "Simple Binding Example 3 (Bind to DataTable field)"
    Public Sub SimpleBindingExp3()

        '  DataSource setup:
        '  
        '  Create a Table named Test and add 2 columns
        '   ID:     int
        '   Name:   string        '
        Dim dtTest As DataTable = New DataTable()
        dtTest.Columns.Add("ID", GetType(Integer))
        dtTest.Columns.Add("Name", GetType(String))

        dtTest.Rows.Add(1, "John")
        dtTest.Rows.Add(2, "Amy")
        dtTest.Rows.Add(3, "Tony")

        Dim bsTest As BindingSource = New BindingSource()
        bsTest.DataSource = dtTest

        ' Bind the TextBoxes
        Me.textBox3.DataBindings.Add("Text", bsTest, "ID")
        Me.textBox4.DataBindings.Add("Text", bsTest, "Name")
    End Sub

    ' Handle the button's click event to navigate the binding.
    Private Sub btnPrev_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles btnPrev.Click
        Dim bsTest As BindingSource = CType(textBox3.DataBindings(0).DataSource, BindingSource)
        bsTest.MovePrevious()
    End Sub

    Private Sub btnNext_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles btnNext.Click
        Dim bsTest As BindingSource = CType(textBox3.DataBindings(0).DataSource, BindingSource)
        bsTest.MoveNext()
    End Sub
#End Region

    ' /////////////////////////////////////////////
    ' Complex Binding Examples
    ' //

#Region "Complex Binding Example 1 (Diplaying data from database)"
    ' This example is done by the Visual Studio designer.
    '
    ' Steps:
    ' 1. Click the smart tag glyph (Smart Tag Glyph) on the upper-right corner of 
    '    the DataGridView control.
    ' 2. Click the drop-down arrow for the Choose Data Source option.
    ' 3. If your project does not already have a data source, click 
    '    "Add Project Data Source.." and follow the steps indicated by the wizard. 
    ' 4. Expand the Other Data Sources and Project Data Sources nodes if they are 
    '    not already expanded, and then select the data source to bind the control to. 
    ' 5. If your data source contains more than one member, such as if you have 
    '    created a DataSet that contains multiple tables, expand the data source, 
    '    and then select the specific member to bind to. 
#End Region

#Region "Complex Binding Example 2 (Displaying data from business objects)"
    Private Sub ComplexBindingExp2()
        ' Data Source Setup:
        Dim blc As BindingList(Of Customer) = New BindingList(Of Customer)()
        blc.Add(New Customer(1, "John", 10.0))
        blc.Add(New Customer(2, "Amy", 15.0))
        blc.Add(New Customer(3, "Tony", 20.0))

        ' Bind the DataGridView to the list of Customers using complex binding.
        Me.dataGridView2.DataSource = blc
    End Sub
#End Region

#Region "Complex Binding Example 3 (Master/Detail Binding)"
    Public Sub ComplexBindingExp3()
        ' Data Source Setup:
        Dim dtMaster As DataTable = New DataTable("Customer")
        Dim dtDetail As DataTable = New DataTable("Order")
        dtMaster.Columns.Add("CustomerID", GetType(Integer))
        dtMaster.Columns.Add("CustomerName", GetType(String))

        dtDetail.Columns.Add("OrderID", GetType(Integer))
        dtDetail.Columns.Add("OrderDate", GetType(DateTime))
        dtDetail.Columns.Add("CustomerID", GetType(Integer))

        For j As Integer = 0 To 5 - 1
            dtMaster.Rows.Add(j, "Customer " + j.ToString())
            dtDetail.Rows.Add(j, DateTime.Now.AddDays(j), j)
            dtDetail.Rows.Add(j + 5, DateTime.Now.AddDays(j + 5), j)
        Next

        ' Create a DataSet to hold the two DataTables
        Dim ds As DataSet = New DataSet()
        ds.Tables.Add(dtMaster)
        ds.Tables.Add(dtDetail)

        ' Add a relationship to the DataSet
        ds.Relations.Add("CustomerOrder", _
            ds.Tables("Customer").Columns("CustomerID"), _
            ds.Tables("Order").Columns("CustomerID"))

        Dim bsMaster As BindingSource = New BindingSource()
        bsMaster.DataSource = ds
        bsMaster.DataMember = "Customer"

        Dim bsDetail As BindingSource = New BindingSource()
        ' Bind the details data connector to the master data connector,
        ' using the DataRelation name to filter the information in the
        ' details table based on the current row in the master table.
        bsDetail.DataSource = bsMaster
        bsDetail.DataMember = "CustomerOrder"

        Me.dgvMaster.DataSource = bsMaster
        Me.dgvDetail.DataSource = bsDetail
    End Sub
#End Region

End Class

#Region "Customer Class"
Public Class Customer
    ' Private variables
    Private _id As Integer
    Private _name As String
    Private _rate As Decimal

    ' Constructor
    Public Sub New()
        _id = 0
        _name = String.Empty
        _rate = 0.0
    End Sub

    Public Sub New(ByVal id As Integer, ByVal name As String, ByVal rate As Decimal)
        _id = id
        _name = name
        _rate = rate
    End Sub

    ' Properties
    Public Property ID() As Integer
        Get
            Return _id
        End Get
        Set(ByVal value As Integer)
            _id = value
        End Set
    End Property

    Public Property Name() As String
        Get
            Return _name
        End Get
        Set(ByVal value As String)
            _name = value
        End Set
    End Property

    Public Property Rate() As Decimal
        Get
            Return _rate
        End Get
        Set(ByVal value As Decimal)
            _rate = value
        End Set
    End Property
End Class
#End Region
