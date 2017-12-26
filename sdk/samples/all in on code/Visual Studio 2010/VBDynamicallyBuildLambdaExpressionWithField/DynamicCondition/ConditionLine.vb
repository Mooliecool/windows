'******************************** Module Header **************************************************'
' Module Name:  ConditionLine.vb
' Project:      VBDynamicallyBuildLambdaExpressionWithField
' Copyright (c) Microsoft Corporation.
'
' The ConditionLine.vb file defines some sub-condition connection operators and some property boxes.
'
' This source is subject to the Microsoft Public License.
' See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
' All other rights reserved.
'
' THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND, EITHER 
' EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED WARRANTIES OF 
' MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
'*************************************************************************************************'
Imports System.Reflection

Partial Friend Class ConditionLine

    Friend Sub New()
        InitializeComponent()
    End Sub

    ''' <summary>
    ''' property DataType
    ''' </summary>
    Private _dataType As Type
    Public Property DataType() As Type
        Get
            Return _dataType
        End Get
        Set(ByVal value As Type)
            _dataType = value
        End Set
    End Property

    ''' <summary>
    ''' Property DataSource
    ''' </summary>
    Private _dataSource() As PropertyInfo
    Public Property DataSource() As PropertyInfo()
        Get
            Return _dataSource
        End Get
        Set(ByVal value As PropertyInfo())
            _dataSource = value
            cmbColumn.DataSource = value
            cmbColumn.DisplayMember = "Name"
        End Set
    End Property

    ''' <summary>
    ''' Condition compare operator
    ''' </summary>
    Public Property OperatorType() As DynamicQuery.Condition.Compare
        Get
            Return (If(lb.Text = "AND", DynamicQuery.Condition.Compare.And, DynamicQuery.Condition.Compare.Or))
        End Get
        Set(ByVal value As DynamicQuery.Condition.Compare)
            If value <> DynamicQuery.Condition.Compare.And And value <> DynamicQuery.Condition.Compare.Or Then
                Throw New ArgumentException("OperatorType must be ""And"" or ""Or""")
            End If
            lb.Text = value.ToString().ToUpper()
        End Set
    End Property

    ''' <summary>
    ''' Returns a Condition(Of T) which represents the criteria stored in the UserControl
    ''' </summary>
    Public Function GetCondition(Of T)(ByVal dataSrc As T) As DynamicCondition.DynamicQuery.Condition(Of T)

        Dim pType = (CType(cmbColumn.SelectedItem, PropertyInfo)).PropertyType

        ' CheckType ensures that T and T? are treated the same
        If CheckType(Of Boolean)(pType) Then
            Return MakeCond(dataSrc, pType, chkValue.Checked)

        ElseIf CheckType(Of Date)(pType) Then
            Return MakeCond(dataSrc, pType, dtpValue.Value)
        ElseIf CheckType(Of Char)(pType) Then
            Return MakeCond(dataSrc, pType, Convert.ToChar(tbValue.Text))
        ElseIf CheckType(Of Long)(pType) Then
            Return MakeCond(dataSrc, pType, Convert.ToInt64(tbValue.Text))
        ElseIf CheckType(Of Short)(pType) Then
            Return MakeCond(dataSrc, pType, Convert.ToInt16(tbValue.Text))
        ElseIf CheckType(Of ULong)(pType) Then
            Return MakeCond(dataSrc, pType, Convert.ToUInt64(tbValue.Text))
        ElseIf CheckType(Of UShort)(pType) Then
            Return MakeCond(dataSrc, pType, Convert.ToUInt16(tbValue.Text))
        ElseIf CheckType(Of Single)(pType) Then
            Return MakeCond(dataSrc, pType, Convert.ToSingle(tbValue.Text))
        ElseIf CheckType(Of Double)(pType) Then
            Return MakeCond(dataSrc, pType, Convert.ToDouble(tbValue.Text))
        ElseIf CheckType(Of Decimal)(pType) Then
            Return MakeCond(dataSrc, pType, Convert.ToDecimal(tbValue.Text))
        ElseIf CheckType(Of Integer)(pType) Then
            Return MakeCond(dataSrc, pType, Convert.ToInt32(SimulateVal.Val(tbValue.Text)))
        ElseIf CheckType(Of UInteger)(pType) Then
            Return MakeCond(dataSrc, pType, Convert.ToUInt32(tbValue.Text))

            ' This can only ever be String, since we filtered the types that we added to the ComboBox
        Else
            Return MakeCond(dataSrc, pType, tbValue.Text)
        End If
    End Function


    Public Shared typeList As List(Of Type)

    ''' <summary>
    ''' contract behind the where keyword
    ''' </summary>
    Public Shared Function GetSupportedTypes() As List(Of Type)
        If typeList Is Nothing Then
            typeList = New List(Of Type)()
            typeList.AddRange(New Type() {GetType(Date), GetType(Date?), GetType(Char), GetType(Char?), GetType(Long), GetType(Long?), GetType(Short), GetType(Short?), GetType(ULong), GetType(ULong?), GetType(UShort), GetType(UShort?), GetType(Single), GetType(Single?), GetType(Double), GetType(Double?), GetType(Decimal), GetType(Decimal?), GetType(Boolean), GetType(Boolean?), GetType(Integer), GetType(Integer?), GetType(UInteger), GetType(UInteger?), GetType(String)})
        End If

        Return typeList
    End Function

    ''' <summary>
    ''' Combine condition 
    ''' </summary>
    Private Sub ConditionLine_Load(ByVal sender As Object, ByVal e As EventArgs) Handles MyBase.Load
        cmbOperator.DisplayMember = "Name"
        cmbOperator.ValueMember = "Value"
        Dim opList = MakeList(New With {Key .Name = "Equal", Key .Value = DynamicQuery.Condition.Compare.Equal}, New With {Key .Name = "Not Equal", Key .Value = DynamicQuery.Condition.Compare.NotEqual}, New With {Key .Name = ">", Key .Value = DynamicQuery.Condition.Compare.GreaterThan}, New With {Key .Name = ">=", Key .Value = DynamicQuery.Condition.Compare.GreaterThanOrEqual}, New With {Key .Name = "<", Key .Value = DynamicQuery.Condition.Compare.LessThan}, New With {Key .Name = "<=", Key .Value = DynamicQuery.Condition.Compare.LessThanOrEqual}, New With {Key .Name = "Like", Key .Value = DynamicQuery.Condition.Compare.Like})
        cmbOperator.DataSource = opList
    End Sub

    ''' <summary>
    ''' select which control to demonstrate when get property from the user picked 
    ''' </summary>
    Private Sub cboColumn_SelectedIndexChanged(ByVal sender As Object, ByVal e As EventArgs) Handles cmbColumn.SelectedIndexChanged

        ' Get the underlying type for the property the user picked
        Dim propType = (CType(cmbColumn.SelectedItem, PropertyInfo)).PropertyType

        ' Display appropriate control (CheckBox/TextBox/DateTimePicker) for property type
        If CheckType(Of Boolean)(propType) Then
            SetVisibility(True, False, False)

        ElseIf CheckType(Of Date)(propType) Then
            SetVisibility(False, True, False)
        Else
            SetVisibility(False, False, True)
        End If
    End Sub

    ''' <summary>
    ''' Set which control is visible
    ''' </summary>
    Private Sub SetVisibility(ByVal chkBox As Boolean, ByVal datePicker As Boolean, ByVal txtBox As Boolean)
        chkValue.Visible = chkBox
        tbValue.Visible = txtBox
        dtpValue.Visible = datePicker
    End Sub

    ''' <summary>
    ''' Toggle between AND/OR
    ''' </summary>
    Private Sub lblOperator_Click(ByVal sender As Object, ByVal e As EventArgs) Handles lb.Click
        lb.Text = (If(lb.Text = "AND", "OR", "AND"))
    End Sub

    ''' <summary>
    ''' MakeCond Operator
    ''' </summary>
    Private Function MakeCond(Of T, S)(ByVal dataSource As T, ByVal propType As Type, ByVal value As S) As DynamicQuery.Condition(Of T)
        Dim dataSourceType As IEnumerable(Of T) = Nothing
        Return DynamicCondition.DynamicQuery.Condition.Create(Of T)(dataSourceType, cmbColumn.Text, CType(cmbOperator.SelectedValue, DynamicQuery.Condition.Compare), value, propType)
    End Function

    ''' <summary>
    ''' Returns true if propType is of type T or Nullable(Of T)
    ''' </summary>
    Private Shared Function CheckType(Of T As Structure)(ByVal propType As Type) As Boolean
        Return (propType.Equals(GetType(T)) Or propType.Equals(GetType(T?)))
    End Function

    ''' <summary>
    ''' Turns list of parameters into an IEnumerable(Of T) (where T is an anonymous type in this case)
    ''' </summary>
    Private Shared Function MakeList(Of T)(ByVal ParamArray items() As T) As IEnumerable(Of T)
        Return items
    End Function
End Class
