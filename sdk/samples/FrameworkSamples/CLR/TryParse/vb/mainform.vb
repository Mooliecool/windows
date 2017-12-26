'-----------------------------------------------------------------------
'  This file is part of the Microsoft .NET SDK Code Samples.
' 
'  Copyright (C) Microsoft Corporation.  All rights reserved.
' 
'This source code is intended only as a supplement to Microsoft
'Development Tools and/or on-line documentation.  See these other
'materials for detailed information regarding Microsoft code samples.
' 
'THIS CODE AND INFORMATION ARE PROVIDED AS IS WITHOUT WARRANTY OF ANY
'KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
'IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
'PARTICULAR PURPOSE.
'-----------------------------------------------------------------------
Imports System.Collections.Specialized
Imports System.Collections.Generic
Imports System.Globalization

Namespace Microsoft.Samples
    Friend Class MainForm
        Inherits System.Windows.Forms.Form

#Region " Windows Form Designer generated code "

        Public Sub New()
            MyBase.New()

            'This call is required by the Windows Form Designer.
            InitializeComponent()

            'Add any initialization after the InitializeComponent() call

        End Sub

        'Form overrides dispose to clean up the component list.
        Protected Overloads Overrides Sub Dispose(ByVal disposing As Boolean)
            If disposing Then
                If Not (components Is Nothing) Then
                    components.Dispose()
                End If
            End If
            MyBase.Dispose(disposing)
        End Sub

        'Required by the Windows Form Designer
        Private components As System.ComponentModel.IContainer
        Friend WithEvents groupResults As System.Windows.Forms.GroupBox
        Friend WithEvents numericPercent As System.Windows.Forms.NumericUpDown
        Friend WithEvents labelPercent As System.Windows.Forms.Label
        Friend WithEvents buttonGo As System.Windows.Forms.Button
        Friend WithEvents buttonExit As System.Windows.Forms.Button
        Friend WithEvents labelIterations As System.Windows.Forms.Label
        Friend WithEvents numericIterations As System.Windows.Forms.NumericUpDown
        Friend WithEvents labelTryParseResultValue As System.Windows.Forms.Label
        Friend WithEvents labelParseResultValue As System.Windows.Forms.Label
        Friend WithEvents labelTryParseResult As System.Windows.Forms.Label
        Friend WithEvents labelParseResult As System.Windows.Forms.Label
        Friend WithEvents labelType As System.Windows.Forms.Label
        Friend WithEvents comboType As System.Windows.Forms.ComboBox
        Friend WithEvents labelPercentResult As System.Windows.Forms.Label
        Friend WithEvents labelPercentResultValue As System.Windows.Forms.Label
        Friend WithEvents buttonHelp As System.Windows.Forms.Button
        Friend WithEvents labelDesc As System.Windows.Forms.Label

        'NOTE: The following procedure is required by the Windows Form Designer
        'It can be modified using the Windows Form Designer.  
        'Do not modify it using the code editor.



        <System.Diagnostics.DebuggerStepThrough()> Private Sub InitializeComponent()
            Me.groupResults = New System.Windows.Forms.GroupBox
            Me.labelPercentResultValue = New System.Windows.Forms.Label
            Me.labelPercentResult = New System.Windows.Forms.Label
            Me.labelParseResult = New System.Windows.Forms.Label
            Me.labelTryParseResult = New System.Windows.Forms.Label
            Me.labelParseResultValue = New System.Windows.Forms.Label
            Me.labelTryParseResultValue = New System.Windows.Forms.Label
            Me.numericPercent = New System.Windows.Forms.NumericUpDown
            Me.labelPercent = New System.Windows.Forms.Label
            Me.buttonGo = New System.Windows.Forms.Button
            Me.buttonExit = New System.Windows.Forms.Button
            Me.labelIterations = New System.Windows.Forms.Label
            Me.numericIterations = New System.Windows.Forms.NumericUpDown
            Me.labelDesc = New System.Windows.Forms.Label
            Me.labelType = New System.Windows.Forms.Label
            Me.comboType = New System.Windows.Forms.ComboBox
            Me.buttonHelp = New System.Windows.Forms.Button
            Me.groupResults.SuspendLayout()
            CType(Me.numericPercent, System.ComponentModel.ISupportInitialize).BeginInit()
            CType(Me.numericIterations, System.ComponentModel.ISupportInitialize).BeginInit()
            Me.SuspendLayout()
            '
            'groupResults
            '
            Me.groupResults.Controls.Add(Me.labelPercentResultValue)
            Me.groupResults.Controls.Add(Me.labelPercentResult)
            Me.groupResults.Controls.Add(Me.labelParseResult)
            Me.groupResults.Controls.Add(Me.labelTryParseResult)
            Me.groupResults.Controls.Add(Me.labelParseResultValue)
            Me.groupResults.Controls.Add(Me.labelTryParseResultValue)
            Me.groupResults.Font = New System.Drawing.Font("Microsoft Sans Serif", 9.75!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(0, Byte))
            Me.groupResults.Location = New System.Drawing.Point(8, 134)
            Me.groupResults.Name = "groupResults"
            Me.groupResults.Size = New System.Drawing.Size(274, 133)
            Me.groupResults.TabIndex = 2
            Me.groupResults.TabStop = False
            Me.groupResults.Text = "Results"
            '
            'labelPercentResultValue
            '
            Me.labelPercentResultValue.Location = New System.Drawing.Point(50, 93)
            Me.labelPercentResultValue.Name = "labelPercentResultValue"
            Me.labelPercentResultValue.Size = New System.Drawing.Size(132, 24)
            Me.labelPercentResultValue.TabIndex = 5
            Me.labelPercentResultValue.TextAlign = System.Drawing.ContentAlignment.TopRight
            Me.labelPercentResultValue.Visible = False
            '
            'labelPercentResult
            '
            Me.labelPercentResult.Location = New System.Drawing.Point(191, 92)
            Me.labelPercentResult.Name = "labelPercentResult"
            Me.labelPercentResult.Size = New System.Drawing.Size(68, 25)
            Me.labelPercentResult.TabIndex = 4
            Me.labelPercentResult.Text = "Faster!"
            Me.labelPercentResult.Visible = False
            '
            'labelParseResult
            '
            Me.labelParseResult.Location = New System.Drawing.Point(8, 25)
            Me.labelParseResult.Name = "labelParseResult"
            Me.labelParseResult.Size = New System.Drawing.Size(96, 25)
            Me.labelParseResult.TabIndex = 3
            Me.labelParseResult.Text = "Parse:"
            '
            'labelTryParseResult
            '
            Me.labelTryParseResult.Location = New System.Drawing.Point(8, 59)
            Me.labelTryParseResult.Name = "labelTryParseResult"
            Me.labelTryParseResult.Size = New System.Drawing.Size(96, 25)
            Me.labelTryParseResult.TabIndex = 2
            Me.labelTryParseResult.Text = "TryParse:"
            '
            'labelParseResultValue
            '
            Me.labelParseResultValue.Location = New System.Drawing.Point(107, 25)
            Me.labelParseResultValue.Name = "labelParseResultValue"
            Me.labelParseResultValue.Size = New System.Drawing.Size(152, 24)
            Me.labelParseResultValue.TabIndex = 1
            Me.labelParseResultValue.TextAlign = System.Drawing.ContentAlignment.TopRight
            '
            'labelTryParseResultValue
            '
            Me.labelTryParseResultValue.Location = New System.Drawing.Point(103, 59)
            Me.labelTryParseResultValue.Name = "labelTryParseResultValue"
            Me.labelTryParseResultValue.Size = New System.Drawing.Size(156, 24)
            Me.labelTryParseResultValue.TabIndex = 0
            Me.labelTryParseResultValue.TextAlign = System.Drawing.ContentAlignment.TopRight
            '
            'numericPercent
            '
            Me.numericPercent.Font = New System.Drawing.Font("Microsoft Sans Serif", 9.75!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(0, Byte))
            Me.numericPercent.Location = New System.Drawing.Point(136, 17)
            Me.numericPercent.Name = "numericPercent"
            Me.numericPercent.Size = New System.Drawing.Size(56, 23)
            Me.numericPercent.TabIndex = 3
            Me.numericPercent.Value = New Decimal(New Integer() {100, 0, 0, 0})
            '
            'labelPercent
            '
            Me.labelPercent.Font = New System.Drawing.Font("Microsoft Sans Serif", 9.75!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(0, Byte))
            Me.labelPercent.Location = New System.Drawing.Point(8, 17)
            Me.labelPercent.Name = "labelPercent"
            Me.labelPercent.Size = New System.Drawing.Size(127, 25)
            Me.labelPercent.TabIndex = 4
            Me.labelPercent.Text = "Success Rate"
            '
            'buttonGo
            '
            Me.buttonGo.Font = New System.Drawing.Font("Microsoft Sans Serif", 9.75!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(0, Byte))
            Me.buttonGo.Location = New System.Drawing.Point(201, 17)
            Me.buttonGo.Name = "buttonGo"
            Me.buttonGo.Size = New System.Drawing.Size(80, 33)
            Me.buttonGo.TabIndex = 5
            Me.buttonGo.Text = "Run"
            '
            'buttonExit
            '
            Me.buttonExit.DialogResult = System.Windows.Forms.DialogResult.Cancel
            Me.buttonExit.Font = New System.Drawing.Font("Microsoft Sans Serif", 9.75!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(0, Byte))
            Me.buttonExit.Location = New System.Drawing.Point(201, 100)
            Me.buttonExit.Name = "buttonExit"
            Me.buttonExit.Size = New System.Drawing.Size(80, 34)
            Me.buttonExit.TabIndex = 6
            Me.buttonExit.Text = "Exit"
            '
            'labelIterations
            '
            Me.labelIterations.Font = New System.Drawing.Font("Microsoft Sans Serif", 9.75!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(0, Byte))
            Me.labelIterations.Location = New System.Drawing.Point(16, 59)
            Me.labelIterations.Name = "labelIterations"
            Me.labelIterations.Size = New System.Drawing.Size(96, 25)
            Me.labelIterations.TabIndex = 7
            Me.labelIterations.Text = "Iterations:"
            '
            'numericIterations
            '
            Me.numericIterations.Font = New System.Drawing.Font("Microsoft Sans Serif", 9.75!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(0, Byte))
            Me.numericIterations.Location = New System.Drawing.Point(136, 59)
            Me.numericIterations.Maximum = New Decimal(New Integer() {70, 0, 0, 0})
            Me.numericIterations.Minimum = New Decimal(New Integer() {20, 0, 0, 0})
            Me.numericIterations.Name = "numericIterations"
            Me.numericIterations.Size = New System.Drawing.Size(56, 23)
            Me.numericIterations.TabIndex = 8
            Me.numericIterations.Value = New Decimal(New Integer() {40, 0, 0, 0})
            '
            'labelDesc
            '
            Me.labelDesc.Font = New System.Drawing.Font("Microsoft Sans Serif", 9.75!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(0, Byte))
            Me.labelDesc.Location = New System.Drawing.Point(8, 276)
            Me.labelDesc.Name = "labelDesc"
            Me.labelDesc.Size = New System.Drawing.Size(272, 106)
            Me.labelDesc.TabIndex = 9
            '
            'labelType
            '
            Me.labelType.Font = New System.Drawing.Font("Microsoft Sans Serif", 9.75!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(0, Byte))
            Me.labelType.Location = New System.Drawing.Point(8, 98)
            Me.labelType.Name = "labelType"
            Me.labelType.Size = New System.Drawing.Size(56, 25)
            Me.labelType.TabIndex = 10
            Me.labelType.Text = "Type:"
            '
            'comboType
            '
            Me.comboType.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList
            Me.comboType.Font = New System.Drawing.Font("Microsoft Sans Serif", 9.75!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(0, Byte))
            Me.comboType.FormattingEnabled = True
            Me.comboType.Location = New System.Drawing.Point(64, 100)
            Me.comboType.Name = "comboType"
            Me.comboType.Size = New System.Drawing.Size(129, 24)
            Me.comboType.TabIndex = 11
            '
            'buttonHelp
            '
            Me.buttonHelp.Font = New System.Drawing.Font("Microsoft Sans Serif", 9.75!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(0, Byte))
            Me.buttonHelp.Location = New System.Drawing.Point(202, 57)
            Me.buttonHelp.Name = "buttonHelp"
            Me.buttonHelp.Size = New System.Drawing.Size(80, 34)
            Me.buttonHelp.TabIndex = 12
            Me.buttonHelp.Text = "Help"
            '
            'MainForm
            '
            Me.AcceptButton = Me.buttonGo
            Me.CancelButton = Me.buttonExit
            Me.ClientSize = New System.Drawing.Size(290, 389)
            Me.Controls.Add(Me.buttonHelp)
            Me.Controls.Add(Me.comboType)
            Me.Controls.Add(Me.labelType)
            Me.Controls.Add(Me.labelDesc)
            Me.Controls.Add(Me.numericIterations)
            Me.Controls.Add(Me.labelIterations)
            Me.Controls.Add(Me.buttonExit)
            Me.Controls.Add(Me.buttonGo)
            Me.Controls.Add(Me.labelPercent)
            Me.Controls.Add(Me.numericPercent)
            Me.Controls.Add(Me.groupResults)
            Me.Font = New System.Drawing.Font("Microsoft Sans Serif", 14.25!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(0, Byte))
            Me.MaximizeBox = False
            Me.MinimizeBox = False
            Me.Name = "MainForm"
            Me.StartPosition = System.Windows.Forms.FormStartPosition.CenterScreen
            Me.Text = "TryParse Demo"
            Me.groupResults.ResumeLayout(False)
            CType(Me.numericPercent, System.ComponentModel.ISupportInitialize).EndInit()
            CType(Me.numericIterations, System.ComponentModel.ISupportInitialize).EndInit()
            Me.ResumeLayout(False)

        End Sub

#End Region

        Private str As StringCollection
        Private types(3) As String
        Private sw As New Stopwatch
        Private parseTime As Long
        Private tryParseTime As Long

        Private Sub MainForm_Load(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles MyBase.Load
            labelDesc.Text = "This demo makes 100 strings, of which" & Environment.NewLine & _
                    "the 'percentage' value determines how many" & Environment.NewLine & _
                    "can be validly parsed to the specified type. " & Environment.NewLine & _
                    "The 'iterations' value determines how many " & Environment.NewLine & _
                    "times the strings are parsed"
            types(0) = "Int32"
            types(1) = "Decimal"
            types(2) = "Double"
            types(3) = "DateTime"

            For Each s As String In types
                comboType.Items.Add(s)
            Next

            comboType.SelectedIndex = 0
        End Sub

        Private Sub FillList()
            If comboType.SelectedIndex <> 3 Then
                NumberFill()
            Else
                DateTimeFill()
            End If

            Dim r As New Random(Environment.TickCount)
            For i As Integer = 1 To CInt(100 - numericPercent.Value)
                Dim length As Integer = r.Next(4, 9)
                Dim s As String = ""
                For j As Integer = 1 To length
                    s = s + Chr(r.Next(97, 123))
                Next

                str.Add(s)
            Next
        End Sub

        Private Sub NumberFill()
            Dim r As New Random(Environment.TickCount)
            str = New StringCollection

            For i As Integer = 1 To CInt(numericPercent.Value)
                str.Add(CStr(r.Next(1, 1000)))
            Next
        End Sub

        Private Sub DateTimeFill()
            Dim r As New Random
            str = New StringCollection

            For i As Integer = 1 To CInt(numericPercent.Value)
                str.Add(DateTime.Now.ToString(CultureInfo.InvariantCulture))
            Next
        End Sub

        Private Sub buttonExit_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles buttonExit.Click
            Me.Close()
        End Sub

        Private Sub buttonGo_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles buttonGo.Click

            Cursor = Cursors.WaitCursor
            FillList()

            labelTryParseResultValue.Text = ""
            labelParseResultValue.Text = ""

            PerformTryParse()
            PerformParse()

            If tryParseTime >= 0 And parseTime >= 0 Then
                If parseTime / tryParseTime < 5 Then
                    labelParseResultValue.ForeColor = Color.DarkGreen
                ElseIf parseTime / tryParseTime < 15 Then
                    labelParseResultValue.ForeColor = Color.Purple
                ElseIf parseTime / tryParseTime < 35 Then
                    labelParseResultValue.ForeColor = Color.DarkRed
                Else
                    labelParseResultValue.ForeColor = Color.Red
                End If
            Else
                labelTryParseResultValue.ForeColor = Color.Black
            End If


            If parseTime > 0 Then
                Dim result As Double = (parseTime / tryParseTime) - 1

                If result < 0.1 And result > -0.1 Then 'an arbitrary pick, but this means they're roughly the same
                    labelPercentResult.Visible = False
                    labelPercentResultValue.Visible = False
                Else
                    If result > 0 Then
                        labelPercentResult.Text = "Faster!"
                        labelPercentResult.ForeColor = Color.Black
                    Else

                        labelPercentResult.ForeColor = Color.Red
                        labelPercentResult.Text = "Slower"
                        result = Math.Abs(result)
                    End If
                    labelPercentResult.Visible = True
                    labelPercentResultValue.Visible = True
                End If

                labelPercentResultValue.Text = String.Format(CultureInfo.CurrentCulture, "{0:N2} X", result)
            Else
                labelPercentResult.Visible = False
                labelPercentResultValue.Visible = False
            End If

            Cursor = Cursors.Arrow
        End Sub

        Private Sub PerformParse()
            Select Case comboType.SelectedIndex
                Case 0
                    Int32ParseRoutine()
                Case 1
                    DecimalParseRoutine()
                Case 2
                    DoubleParseRoutine()
                Case 3
                    DateTimeParseRoutine()
            End Select

            'parseTime = sw.ElapsedMilliseconds
            parseTime = sw.ElapsedTicks
            labelParseResultValue.Text = parseTime & " ticks"
        End Sub

        Private Sub PerformTryParse()
            Select Case comboType.SelectedIndex
                Case 0
                    Int32TryParseRoutine()
                Case 1
                    DecimalTryParseRoutine()
                Case 2
                    DoubleTryParseRoutine()
                Case 3
                    DateTimeTryParseRoutine()

            End Select

            'tryParseTime = sw.ElapsedMilliseconds
            tryParseTime = sw.ElapsedTicks
            labelTryParseResultValue.Text = tryParseTime & " ticks"
        End Sub

        Private Sub Int32ParseRoutine()
            Dim i As Integer
            sw = Stopwatch.StartNew()
            For j As Integer = 1 To CInt(numericIterations.Value)
                For Each s As String In str
                    Try
                        i = Int32.Parse(s, NumberStyles.Any, CultureInfo.InvariantCulture)
                        ' print successful
                    Catch fe As FormatException
                        ' print unsuccessful
                    End Try
                Next
            Next
            sw.Stop()
        End Sub

        Private Sub Int32TryParseRoutine()
            Dim i As Integer
            sw = Stopwatch.StartNew()
            For j As Integer = 1 To CInt(numericIterations.Value)
                For Each s As String In str
                    If Int32.TryParse(s, NumberStyles.Any, CultureInfo.InvariantCulture, i) Then
                        ' print successful
                    Else
                        ' print unsuccessful
                    End If
                Next
            Next
            sw.Stop()
        End Sub

        Private Sub DecimalParseRoutine()
            Dim d As Decimal
            sw = Stopwatch.StartNew()
            For j As Integer = 1 To CInt(numericIterations.Value)
                For Each s As String In str
                    Try
                        d = Decimal.Parse(s, NumberStyles.Any, CultureInfo.InvariantCulture)
                        ' print successful
                    Catch fe As FormatException
                        ' print unsuccessful
                    End Try
                Next
            Next
            sw.Stop()
        End Sub

        Private Sub DecimalTryParseRoutine()
            Dim d As Decimal
            sw = Stopwatch.StartNew()
            For j As Integer = 1 To CInt(numericIterations.Value)
                For Each s As String In str
                    If Decimal.TryParse(s, NumberStyles.Any, CultureInfo.InvariantCulture, d) Then
                        ' print successful
                    Else
                        ' print unsuccessful
                    End If
                Next
            Next
            sw.Stop()
        End Sub

        Private Sub DoubleParseRoutine()
            Dim d As Double
            sw = Stopwatch.StartNew()
            For j As Integer = 1 To CInt(numericIterations.Value)
                For Each s As String In str
                    Try
                        d = Double.Parse(s, NumberStyles.Any, CultureInfo.InvariantCulture)
                        ' print successful
                    Catch fe As FormatException
                        ' print unsuccessful
                    End Try
                Next
            Next
            sw.Stop()
        End Sub

        Private Sub DoubleTryParseRoutine()
            Dim d As Double
            sw = Stopwatch.StartNew()
            For j As Integer = 1 To CInt(numericIterations.Value)
                For Each s As String In str
                    If Double.TryParse(s, NumberStyles.Any, CultureInfo.InvariantCulture, d) Then
                        ' print successful
                    Else
                        ' print unsuccessful
                    End If
                Next
            Next
            sw.Stop()
        End Sub

        Private Sub DateTimeParseRoutine()
            Dim d As DateTime
            sw = Stopwatch.StartNew()
            For j As Integer = 1 To CInt(numericIterations.Value)
                For Each s As String In str
                    Try
                        d = DateTime.Parse(s, CultureInfo.InvariantCulture, DateTimeStyles.None)
                        ' print successful
                    Catch fe As FormatException
                        ' print unsuccessful
                    End Try
                Next
            Next
            sw.Stop()
        End Sub

        Private Sub DateTimeTryParseRoutine()
            Dim d As DateTime
            sw = Stopwatch.StartNew()
            For j As Integer = 1 To CInt(numericIterations.Value)
                For Each s As String In str
                    If DateTime.TryParse(s, CultureInfo.InvariantCulture, DateTimeStyles.None, d) Then
                        ' print successful
                    Else
                        ' print unsuccessful
                    End If
                Next
            Next
            sw.Stop()
        End Sub

        Private Sub buttonHelp_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles buttonHelp.Click
            Dim d As New FormHelp()
            d.Show()
        End Sub
    End Class
End Namespace