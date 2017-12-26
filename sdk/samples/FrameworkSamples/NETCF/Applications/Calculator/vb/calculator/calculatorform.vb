' =====================================================================
'  File:      CalculatorForm.vb
'
' 
'  ---------------------------------------------------------------------
'   Copyright (C) Microsoft Corporation.  All rights reserved.
'
'  This source code is intended only as a supplement to Microsoft
'  Development Tools and/or on-line documentation.  See these other
'  materials for detailed information regarding Microsoft code samples.
'
'  THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
'  KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
'  IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
'  PARTICULAR PURPOSE.
' =====================================================================

Imports System
Imports System.Globalization
Imports Microsoft.VisualBasic


Namespace Microsoft.Samples.CompactFramework
    
    Public Class OperatorNotDefinedException
        Inherits System.Exception
    End Class
    
    Public Class ModeNotDefinedException
        Inherits System.Exception
    End Class
    
    Public Class CalculatorForm
        Inherits System.Windows.Forms.Form

        Private Enum Mode
            digit_noDec
            digit_withDec
            opBinary
            opUnary
            opAfterEqual
            equals
            mem
            err
        End Enum

    #Region "Designer Member Variables"
        Friend WithEvents lMem As System.Windows.Forms.Label
        Friend WithEvents pMem As System.Windows.Forms.Panel
        Friend WithEvents bEquals As System.Windows.Forms.Button
        Friend WithEvents bDecimal As System.Windows.Forms.Button
        Friend WithEvents b0 As System.Windows.Forms.Button
        Friend WithEvents bPlus As System.Windows.Forms.Button
        Friend WithEvents b3 As System.Windows.Forms.Button
        Friend WithEvents b2 As System.Windows.Forms.Button
        Friend WithEvents b1 As System.Windows.Forms.Button
        Friend WithEvents bMinus As System.Windows.Forms.Button
        Friend WithEvents b5 As System.Windows.Forms.Button
        Friend WithEvents b4 As System.Windows.Forms.Button
        Friend WithEvents bMultiply As System.Windows.Forms.Button
        Friend WithEvents b8 As System.Windows.Forms.Button
        Friend WithEvents b7 As System.Windows.Forms.Button
        Friend WithEvents bDivide As System.Windows.Forms.Button
        Friend WithEvents bInverse As System.Windows.Forms.Button
        Friend WithEvents bSqrt As System.Windows.Forms.Button
        Friend WithEvents bMR As System.Windows.Forms.Button
        Friend WithEvents bC As System.Windows.Forms.Button
        Friend WithEvents bCE As System.Windows.Forms.Button
        Friend WithEvents bPlusMinus As System.Windows.Forms.Button
        Friend WithEvents bMPlus As System.Windows.Forms.Button
        Friend WithEvents bMC As System.Windows.Forms.Button
        Friend WithEvents lDisplay As System.Windows.Forms.Label
        Friend WithEvents pDisplay As System.Windows.Forms.Panel
        Friend WithEvents b6 As System.Windows.Forms.Button
        Friend WithEvents b9 As System.Windows.Forms.Button
        Friend WithEvents bBackspace As System.Windows.Forms.Button
    #End Region

    #Region "Other Member Variables"
        Private memoryValue As Double
        Private memoryIncrement As Double
        Private internalResult As Double
        Private inputValue As Double
        Private currentEntry As Double
        Private currentNumberOfDecimalPlaces As Integer
        Private operand As String
        Private previousMode As Mode
        Private errorString As String
    #End Region

        Public Sub New()
            MyBase.New()
            'This call is required by the Windows Form Designer.
            InitializeComponent()
            ' Other initializations
            Me.memoryValue = 0
            Me.memoryIncrement = 0
            Me.internalResult = 0
            Me.inputValue = 0
            Me.currentEntry = 0
            Me.currentNumberOfDecimalPlaces = 0
            Me.operand = Nothing
            Me.previousMode = Mode.digit_noDec
            Me.errorString = Nothing
        End Sub

    #Region " Windows Form Designer generated code "

        'Form overrides dispose to clean up the component list.
        Protected Overloads Overrides Sub Dispose(ByVal disposing As Boolean)
            MyBase.Dispose(disposing)
        End Sub

        'NOTE: The following procedure is required by the Windows Form Designer
        'It can be modified using the Windows Form Designer.  
        'Do not modify it using the code editor.
        Private Sub InitializeComponent()
            Me.bBackspace = New System.Windows.Forms.Button
            Me.lMem = New System.Windows.Forms.Label
            Me.pMem = New System.Windows.Forms.Panel
            Me.bEquals = New System.Windows.Forms.Button
            Me.bDecimal = New System.Windows.Forms.Button
            Me.b0 = New System.Windows.Forms.Button
            Me.bPlus = New System.Windows.Forms.Button
            Me.b3 = New System.Windows.Forms.Button
            Me.b2 = New System.Windows.Forms.Button
            Me.b1 = New System.Windows.Forms.Button
            Me.bMinus = New System.Windows.Forms.Button
            Me.b6 = New System.Windows.Forms.Button
            Me.b5 = New System.Windows.Forms.Button
            Me.b4 = New System.Windows.Forms.Button
            Me.bMultiply = New System.Windows.Forms.Button
            Me.b9 = New System.Windows.Forms.Button
            Me.b8 = New System.Windows.Forms.Button
            Me.b7 = New System.Windows.Forms.Button
            Me.bDivide = New System.Windows.Forms.Button
            Me.bInverse = New System.Windows.Forms.Button
            Me.bSqrt = New System.Windows.Forms.Button
            Me.bMR = New System.Windows.Forms.Button
            Me.bC = New System.Windows.Forms.Button
            Me.bCE = New System.Windows.Forms.Button
            Me.bPlusMinus = New System.Windows.Forms.Button
            Me.bMPlus = New System.Windows.Forms.Button
            Me.bMC = New System.Windows.Forms.Button
            Me.lDisplay = New System.Windows.Forms.Label
            Me.pDisplay = New System.Windows.Forms.Panel
            '
            'bBackspace
            '
            Me.bBackspace.Font = New System.Drawing.Font("Microsoft Sans Serif", _
                8.25!, System.Drawing.FontStyle.Regular)
            Me.bBackspace.Location = New System.Drawing.Point(208, 16)
            Me.bBackspace.Size = New System.Drawing.Size(26, 28)
            Me.bBackspace.Text = "<"
            '
            'lMem
            '
            Me.lMem.Font = New System.Drawing.Font("Arial", 16.75!, _
                System.Drawing.FontStyle.Regular)
            Me.lMem.Location = New System.Drawing.Point(9, 18)
            Me.lMem.Size = New System.Drawing.Size(24, 26)
            Me.lMem.TextAlign = System.Drawing.ContentAlignment.TopCenter
            '
            'pMem
            '
            Me.pMem.BackColor = System.Drawing.Color.Black
            Me.pMem.Location = New System.Drawing.Point(8, 17)
            Me.pMem.Size = New System.Drawing.Size(26, 28)
            '
            'bEquals
            '
            Me.bEquals.Location = New System.Drawing.Point(149, 248)
            Me.bEquals.Size = New System.Drawing.Size(36, 30)
            Me.bEquals.Text = "="
            '
            'bDecimal
            '
            Me.bDecimal.Location = New System.Drawing.Point(102, 248)
            Me.bDecimal.Size = New System.Drawing.Size(36, 30)
            Me.bDecimal.Text = "."
            '
            'b0
            '
            Me.b0.Location = New System.Drawing.Point(55, 248)
            Me.b0.Size = New System.Drawing.Size(36, 30)
            Me.b0.Text = "0"
            '
            'bPlus
            '
            Me.bPlus.Location = New System.Drawing.Point(196, 202)
            Me.bPlus.Size = New System.Drawing.Size(36, 76)
            Me.bPlus.Text = "+"
            '
            'b3
            '
            Me.b3.Location = New System.Drawing.Point(149, 202)
            Me.b3.Size = New System.Drawing.Size(36, 30)
            Me.b3.Text = "3"
            '
            'b2
            '
            Me.b2.Location = New System.Drawing.Point(102, 202)
            Me.b2.Size = New System.Drawing.Size(36, 30)
            Me.b2.Text = "2"
            '
            'b1
            '
            Me.b1.Location = New System.Drawing.Point(55, 202)
            Me.b1.Size = New System.Drawing.Size(36, 30)
            Me.b1.Text = "1"
            '
            'bMinus
            '
            Me.bMinus.Location = New System.Drawing.Point(196, 156)
            Me.bMinus.Size = New System.Drawing.Size(36, 30)
            Me.bMinus.Text = "-"
            '
            'b6
            '
            Me.b6.Location = New System.Drawing.Point(149, 156)
            Me.b6.Size = New System.Drawing.Size(36, 30)
            Me.b6.Text = "6"
            '
            'b5
            '
            Me.b5.Location = New System.Drawing.Point(102, 156)
            Me.b5.Size = New System.Drawing.Size(36, 30)
            Me.b5.Text = "5"
            '
            'b4
            '
            Me.b4.Location = New System.Drawing.Point(55, 156)
            Me.b4.Size = New System.Drawing.Size(36, 30)
            Me.b4.Text = "4"
            '
            'bMultiply
            '
            Me.bMultiply.Location = New System.Drawing.Point(196, 110)
            Me.bMultiply.Size = New System.Drawing.Size(36, 30)
            Me.bMultiply.Text = "X"
            '
            'b9
            '
            Me.b9.Location = New System.Drawing.Point(149, 110)
            Me.b9.Size = New System.Drawing.Size(36, 30)
            Me.b9.Text = "9"
            '
            'b8
            '
            Me.b8.Location = New System.Drawing.Point(102, 110)
            Me.b8.Size = New System.Drawing.Size(36, 30)
            Me.b8.Text = "8"
            '
            'b7
            '
            Me.b7.Location = New System.Drawing.Point(55, 110)
            Me.b7.Size = New System.Drawing.Size(36, 30)
            Me.b7.Text = "7"
            '
            'bDivide
            '
            Me.bDivide.Location = New System.Drawing.Point(196, 64)
            Me.bDivide.Size = New System.Drawing.Size(36, 30)
            Me.bDivide.Text = "/"
            '
            'bInverse
            '
            Me.bInverse.Location = New System.Drawing.Point(149, 64)
            Me.bInverse.Size = New System.Drawing.Size(36, 30)
            Me.bInverse.Text = "1/x"
            '
            'bSqrt
            '
            Me.bSqrt.Location = New System.Drawing.Point(102, 64)
            Me.bSqrt.Size = New System.Drawing.Size(36, 30)
            Me.bSqrt.Text = "sqrt"
            '
            'bMR
            '
            Me.bMR.Location = New System.Drawing.Point(55, 64)
            Me.bMR.Size = New System.Drawing.Size(36, 30)
            Me.bMR.Text = "MR"
            '
            'bC
            '
            Me.bC.Location = New System.Drawing.Point(8, 248)
            Me.bC.Size = New System.Drawing.Size(36, 30)
            Me.bC.Text = "C"
            '
            'bCE
            '
            Me.bCE.Location = New System.Drawing.Point(8, 202)
            Me.bCE.Size = New System.Drawing.Size(36, 30)
            Me.bCE.Text = "CE"
            '
            'bPlusMinus
            '
            Me.bPlusMinus.Location = New System.Drawing.Point(8, 156)
            Me.bPlusMinus.Size = New System.Drawing.Size(36, 30)
            Me.bPlusMinus.Text = "+/-"
            '
            'bMPlus
            '
            Me.bMPlus.Location = New System.Drawing.Point(8, 110)
            Me.bMPlus.Size = New System.Drawing.Size(36, 30)
            Me.bMPlus.Text = "M+"
            '
            'bMC
            '
            Me.bMC.Location = New System.Drawing.Point(8, 64)
            Me.bMC.Size = New System.Drawing.Size(36, 30)
            Me.bMC.Text = "MC"
            '
            'lDisplay
            '
            Me.lDisplay.Font = New System.Drawing.Font("Microsoft Sans Serif", _
                15.75!, System.Drawing.FontStyle.Regular)
            Me.lDisplay.Location = New System.Drawing.Point(43, 18)
            Me.lDisplay.Size = New System.Drawing.Size(154, 26)
            Me.lDisplay.Text = "0"
            Me.lDisplay.TextAlign = System.Drawing.ContentAlignment.TopRight
            '
            'pDisplay
            '
            Me.pDisplay.BackColor = System.Drawing.Color.Black
            Me.pDisplay.Location = New System.Drawing.Point(42, 17)
            Me.pDisplay.Size = New System.Drawing.Size(156, 28)
            '
            'CalculatorForm
            '
            Me.ClientSize = New System.Drawing.Size(240, 295)
            Me.Controls.Add(Me.bBackspace)
            Me.Controls.Add(Me.lMem)
            Me.Controls.Add(Me.pMem)
            Me.Controls.Add(Me.bEquals)
            Me.Controls.Add(Me.bDecimal)
            Me.Controls.Add(Me.b0)
            Me.Controls.Add(Me.bPlus)
            Me.Controls.Add(Me.b3)
            Me.Controls.Add(Me.b2)
            Me.Controls.Add(Me.b1)
            Me.Controls.Add(Me.bMinus)
            Me.Controls.Add(Me.b6)
            Me.Controls.Add(Me.b5)
            Me.Controls.Add(Me.b4)
            Me.Controls.Add(Me.bMultiply)
            Me.Controls.Add(Me.b9)
            Me.Controls.Add(Me.b8)
            Me.Controls.Add(Me.b7)
            Me.Controls.Add(Me.bDivide)
            Me.Controls.Add(Me.bInverse)
            Me.Controls.Add(Me.bSqrt)
            Me.Controls.Add(Me.bMR)
            Me.Controls.Add(Me.bC)
            Me.Controls.Add(Me.bCE)
            Me.Controls.Add(Me.bPlusMinus)
            Me.Controls.Add(Me.bMPlus)
            Me.Controls.Add(Me.bMC)
            Me.Controls.Add(Me.lDisplay)
            Me.Controls.Add(Me.pDisplay)
            Me.Text = "Calculator"

        End Sub

        Public Shared Sub Main()
            Application.Run(New CalculatorForm())
        End Sub

    #End Region

        Private Sub digitClickHandler_Click(ByVal sender As System.Object, _
        ByVal e As System.EventArgs) Handles b0.Click, b1.Click, _
        b2.Click, b3.Click, b4.Click, b5.Click, b6.Click, b7.Click, _
        b8.Click, b9.Click
            If Me.previousMode <> Mode.err Then
                If Me.previousMode = Mode.equals Then
                    Me.operand = Nothing
                End If
                If (Me.previousMode <> Mode.digit_noDec) AndAlso _
                    (Me.previousMode <> Mode.digit_withDec) Then
                        Me.currentEntry = 0
                        Me.previousMode = Mode.digit_noDec
                End If
                Dim clickedButton As Button = CType(sender, Button)
                Me.currentEntry = (Me.currentEntry * 10) + _
                    System.Convert.ToDouble(clickedButton.Text, new NumberFormatInfo())
                If Me.previousMode = Mode.digit_withDec Then
                    Me.currentNumberOfDecimalPlaces = _
                        Me.currentNumberOfDecimalPlaces + 1
                End If
                Me.updateDisplay(Me.previousMode)
            End If
        End Sub

        Private Sub updateDisplay(ByVal currentMode As Mode)
            If Me.previousMode <> Mode.err Then
                Dim outputStr As String
                Select Case currentMode
                    Case Mode.digit_noDec
                        outputStr = Me.currentEntry.ToString("F" + _
                            Me.currentNumberOfDecimalPlaces.ToString(new NumberFormatInfo()))
                    Case Mode.digit_withDec
                        Dim valueDisplay As Double = Me.currentEntry
                        valueDisplay = valueDisplay / _
                            System.Math.Pow(10, Me.currentNumberOfDecimalPlaces)
                        outputStr = valueDisplay.ToString("F" + _
                            Me.currentNumberOfDecimalPlaces.ToString(new NumberFormatInfo()))
                    Case Mode.opBinary, Mode.equals
                        outputStr = Me.internalResult.ToString("G8")
                    Case Mode.opUnary, Mode.mem
                        outputStr = Me.inputValue.ToString("G8")
                    Case Else
                        Throw New ModeNotDefinedException()
                End Select
                Me.lDisplay.Text = outputStr
            Else
                Me.lDisplay.Text = Me.errorString
            End If
        End Sub

        Private Sub bDecimal_Click(ByVal sender As System.Object, _
        ByVal e As System.EventArgs) Handles bDecimal.Click
            If Me.previousMode <> Mode.err Then
                Me.previousMode = Mode.digit_withDec
            End If
        End Sub

        Private Sub bC_Click(ByVal sender As System.Object, _
        ByVal e As System.EventArgs) Handles bC.Click
            Me.currentEntry = 0
            Me.currentNumberOfDecimalPlaces = 0
            Me.internalResult = 0
            Me.inputValue = 0
            Me.operand = Nothing
            Me.previousMode = Mode.digit_noDec
            Me.updateDisplay(Mode.digit_noDec)
        End Sub

        Private Sub bCE_Click(ByVal sender As System.Object, _
        ByVal e As System.EventArgs) Handles bCE.Click
            Me.currentEntry = 0
            Me.inputValue = 0
            Me.currentNumberOfDecimalPlaces = 0
            Me.previousMode = Mode.digit_noDec
            Me.updateDisplay(Mode.digit_noDec)
        End Sub

        Private Sub bPlusMinus_Click(ByVal sender As System.Object, _
        ByVal e As System.EventArgs) Handles bPlusMinus.Click
            Select Case Me.previousMode
                Case Mode.digit_noDec, Mode.digit_withDec
                    Me.currentEntry = Me.currentEntry * -1
                Case Mode.equals
                    Me.internalResult = Me.internalResult * -1
                Case Mode.opUnary, Mode.mem
                    Me.inputValue = Me.inputValue * -1
                Case Else
                    ' do nothing
            End Select
            Me.updateDisplay(Me.previousMode)
        End Sub

        Private Sub binaryOperatorClickHandler_Click( _
        ByVal sender As System.Object, ByVal e As System.EventArgs) _
        Handles bPlus.Click, bMinus.Click, bMultiply.Click, bDivide.Click
            Dim opButton As Button = CType(sender, Button)
            If (Me.previousMode = Mode.digit_noDec) OrElse _
                (Me.previousMode = Mode.digit_withDec) OrElse _
                (Me.previousMode = Mode.mem) Then
                    If (Me.previousMode = Mode.digit_noDec) OrElse _
                        (Me.previousMode = Mode.digit_withDec) Then
                            Me.inputValue = Me.currentEntry / _
                                System.Math.Pow(10, Me.currentNumberOfDecimalPlaces)
                    End If
                    Me.previousMode = Mode.opBinary
                    If Me.operand Is Nothing Then
                        ' store the inputted op and value
                        Me.operand = opButton.Text
                        Me.internalResult = Me.inputValue
                    Else
                        ' execute the previous op
                        Me.execOp(Me.operand)
                        Me.updateDisplay(Mode.opBinary)
                        ' store the inputted op
                        Me.operand = opButton.Text
                    End If
            ElseIf Me.previousMode = Mode.opUnary Then
                Me.previousMode = Mode.opBinary
                ' store the inputted op and value
                Me.operand = opButton.Text
                Me.internalResult = Me.inputValue
            ElseIf Me.previousMode = Mode.opBinary Then
                Me.previousMode = Mode.opBinary
                If Not Me.operand Is Nothing Then
                    ' if we saved an op last time, execute it
                    Me.execOp(Me.operand)
                    Me.updateDisplay(Mode.opBinary)
                    ' store the inputted op
                    Me.operand = opButton.Text
                End If
            ElseIf Me.previousMode = Mode.equals Then
                ' store the input op
                Me.operand = opButton.Text
                Me.previousMode = Mode.opAfterEqual
            ElseIf Me.previousMode = Mode.opAfterEqual Then
                ' store the input op
                Me.operand = opButton.Text
                Me.previousMode = Mode.opAfterEqual
            Else
                ' do nothing
            End If
        End Sub

        Private Sub execOp(ByRef opString As String)
            Select Case opString
                Case "+"
                    Me.internalResult = Me.internalResult + Me.inputValue
                Case "-"
                    Me.internalResult = Me.internalResult - Me.inputValue
                Case "X"
                    Me.internalResult = Me.internalResult * Me.inputValue
                Case "/"
                    If Me.inputValue = 0 Then
                        Me.previousMode = Mode.err
                        Me.errorString = "Error: Divide by zero"
                        Return
                    End If
                    Me.internalResult = Me.internalResult / Me.inputValue
                Case Else
                    Throw New OperatorNotDefinedException()
            End Select
        End Sub

        Private Sub bEquals_Click(ByVal sender As System.Object, _
        ByVal e As System.EventArgs) Handles bEquals.Click
            If Me.previousMode <> Mode.err Then
                Dim equalsButton As Button = CType(sender, Button)
                If (Me.previousMode = Mode.digit_noDec) OrElse _
                    (Me.previousMode = Mode.digit_withDec) Then
                        Me.inputValue = Me.currentEntry / _
                            System.Math.Pow(10, Me.currentNumberOfDecimalPlaces)
                        Me.previousMode = Mode.equals
                ElseIf Me.previousMode = Mode.opAfterEqual Then
                    Me.inputValue = Me.internalResult
                    Me.previousMode = Mode.equals
                End If
                If Not Me.operand Is Nothing Then
                    Me.previousMode = Mode.equals
                    ' do op
                    Me.execOp(Me.operand)
                    Me.updateDisplay(Mode.equals)
                End If
            End If
        End Sub

        Private Sub unaryOperatorClickHandler_ClickByVal( _
        ByVal sender As System.Object, ByVal e As System.EventArgs) _
        Handles bInverse.Click, bSqrt.Click
            Dim opButton As Button = CType(sender, Button)
            If (Me.previousMode = Mode.digit_noDec) OrElse _
                (Me.previousMode = Mode.digit_withDec) Then
                    Me.previousMode = Mode.opUnary
                    Me.inputValue = Me.currentEntry / _
                        System.Math.Pow(10, Me.currentNumberOfDecimalPlaces)
                    ' execute this op
                    If Not Me.execUnaryOp(opButton.Text) Then
                        ' an error occured
                        Me.updateDisplay(Mode.err)
                        Return
                    End If
                    If Me.operand Is Nothing Then
                        ' no op stored
                        Me.updateDisplay(Mode.opUnary)
                    Else
                        ' execute the previous op
                        Me.execOp(Me.operand)
                        ' store result in input for future unary ops
                        Me.inputValue = Me.internalResult
                        Me.updateDisplay(Mode.opBinary)
                        ' clear the op store
                        Me.operand = Nothing
                    End If
            ElseIf (Me.previousMode = Mode.opUnary) OrElse _
                ((Me.operand Is Nothing) AndAlso _
                ((Me.previousMode = Mode.opBinary) OrElse _
                (Me.previousMode = Mode.mem))) Then
                    Me.previousMode = Mode.opUnary
                    Me.operand = Nothing
                    If Not Me.execUnaryOp(opButton.Text) Then
                        ' an error occured
                        Me.updateDisplay(Mode.err)
                        Return
                    End If
                    Me.updateDisplay(Mode.opUnary)
            ElseIf (Not Me.operand Is Nothing) AndAlso _
                ((Me.previousMode = Mode.opBinary) OrElse _
                (Me.previousMode = Mode.mem)) Then
                    Me.previousMode = Mode.opUnary
                    ' execute this op
                    If Not Me.execUnaryOp(opButton.Text) Then
                        ' an error occured
                        Me.updateDisplay(Mode.err)
                        Return
                    End If
                    ' execute the previous op
                    Me.execOp(Me.operand)
                    ' store result in input for future unary ops
                    Me.inputValue = Me.internalResult
                    Me.updateDisplay(Mode.opBinary)
                    ' clear the op store
                    Me.operand = Nothing
            ElseIf (Me.previousMode = Mode.equals) OrElse _
                (Me.previousMode = Mode.opAfterEqual) Then
                    Me.previousMode = Mode.opUnary
                    Me.operand = Nothing
                    Me.inputValue = Me.internalResult
                    If Not Me.execUnaryOp(opButton.Text) Then
                        ' an error occured
                        Me.updateDisplay(Mode.err)
                        Return
                    End If
                    Me.updateDisplay(Mode.opUnary)
            Else
                ' do nothing
            End If
        End Sub

        Private Function execUnaryOp(ByRef opString As String) As Boolean
            Select Case opString
                Case "1/x"
                    If Me.inputValue = 0 Then
                        Me.previousMode = Mode.err
                        Me.errorString = "Error"
                        Return False
                    End If
                    Me.inputValue = 1 / Me.inputValue
                Case "sqrt"
                    If Me.inputValue < 0 Then
                        Me.previousMode = Mode.err
                        Me.errorString = "Undefined"
                        Return False
                    End If
                    Me.inputValue = Math.Sqrt(Me.inputValue)
                Case Else
                    Throw New OperatorNotDefinedException()
            End Select
            Return True
        End Function

        Private Sub bMPlus_Click(ByVal sender As System.Object, _
        ByVal e As System.EventArgs) Handles bMPlus.Click
            Select Case Me.previousMode
                Case Mode.digit_noDec, Mode.digit_withDec
                    If Me.currentEntry = 0 Then
                        Return ' do nothing
                    End If
                    Me.inputValue = Me.currentEntry / _
                        System.Math.Pow(10, Me.currentNumberOfDecimalPlaces)
                    Me.memoryIncrement = Me.inputValue
                    Me.previousMode = Mode.mem
                Case Mode.opUnary
                    If Me.inputValue = 0 Then
                        Return ' do nothing
                    End If
                    Me.memoryIncrement = Me.inputValue
                Case Mode.opBinary, Mode.opAfterEqual, Mode.equals
                    If Me.internalResult = 0 Then
                        Return ' do nothing
                    End If
                    Me.memoryIncrement = Me.internalResult
                Case Mode.mem
                    ' mem increment is already stored
                Case Mode.err
                    Return ' do nothing
                Case Else
                    Throw New ModeNotDefinedException()
            End Select
            Me.memoryValue += Me.memoryIncrement
            Me.lMem.Text = "M"
        End Sub

        Private Sub bMR_Click(ByVal sender As System.Object, _
        ByVal e As System.EventArgs) Handles bMR.Click
            If Me.previousMode <> Mode.err Then
                If Me.previousMode = Mode.equals Then
                    Me.operand = Nothing
                End If
                Me.inputValue = Me.memoryValue
                Me.memoryIncrement = Me.memoryValue
                Me.previousMode = Mode.mem
                Me.updateDisplay(Mode.mem)
            End If
        End Sub

        Private Sub bMC_Click(ByVal sender As System.Object, _
        ByVal e As System.EventArgs) Handles bMC.Click
            If Me.previousMode <> Mode.err Then
                Me.lMem.Text = ""
                Me.memoryIncrement = 0
                Me.memoryValue = 0
            End If
        End Sub

        Private Sub bBackspace_Click(ByVal sender As System.Object, _
        ByVal e As System.EventArgs) Handles bBackspace.Click
            If (Me.previousMode = Mode.digit_noDec) OrElse _
                (Me.previousMode = Mode.digit_withDec) Then
                If Me.currentEntry >= 0 Then
                    Me.currentEntry = Math.Floor(Me.currentEntry / 10)
                Else
                    Me.currentEntry = Math.Ceiling(Me.currentEntry / 10)
                End If

                If Me.currentNumberOfDecimalPlaces > 0 Then
                    Me.currentNumberOfDecimalPlaces = _
                        Me.currentNumberOfDecimalPlaces - 1
                    If Me.currentNumberOfDecimalPlaces = 0 Then
                        Me.previousMode = Mode.digit_noDec
                    End If
                End If
                Me.updateDisplay(Me.previousMode)
            End If
        End Sub

    End Class
End Namespace