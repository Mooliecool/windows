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
Friend Class FormHelp
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
    Friend WithEvents labelDesc As System.Windows.Forms.Label
    Friend WithEvents labelQuestion As System.Windows.Forms.Label
    Friend WithEvents GroupUsageTryParse As System.Windows.Forms.GroupBox
    Friend WithEvents labelUsagePatternTryParse As System.Windows.Forms.Label
    Friend WithEvents GroupUsageParse As System.Windows.Forms.GroupBox
    Friend WithEvents labelUsagePatternParse As System.Windows.Forms.Label

    'Required by the Windows Form Designer
    Private components As System.ComponentModel.IContainer

    'NOTE: The following procedure is required by the Windows Form Designer
    'It can be modified using the Windows Form Designer.  
    'Do not modify it using the code editor.
    <System.Diagnostics.DebuggerNonUserCode()> Private Sub InitializeComponent()
        Me.labelDesc = New System.Windows.Forms.Label
        Me.labelQuestion = New System.Windows.Forms.Label
        Me.GroupUsageTryParse = New System.Windows.Forms.GroupBox
        Me.labelUsagePatternTryParse = New System.Windows.Forms.Label
        Me.GroupUsageParse = New System.Windows.Forms.GroupBox
        Me.labelUsagePatternParse = New System.Windows.Forms.Label
        Me.GroupUsageTryParse.SuspendLayout()
        Me.GroupUsageParse.SuspendLayout()
        Me.SuspendLayout()
        '
        'labelDesc
        '
        Me.labelDesc.Font = New System.Drawing.Font("Microsoft Sans Serif", 9.75!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(0, Byte))
        Me.labelDesc.Location = New System.Drawing.Point(13, 45)
        Me.labelDesc.Name = "labelDesc"
        Me.labelDesc.Size = New System.Drawing.Size(410, 352)
        Me.labelDesc.TabIndex = 10
        '
        'labelQuestion
        '
        Me.labelQuestion.Font = New System.Drawing.Font("Microsoft Sans Serif", 9.75!, System.Drawing.FontStyle.Underline, System.Drawing.GraphicsUnit.Point, CType(0, Byte))
        Me.labelQuestion.Location = New System.Drawing.Point(13, 7)
        Me.labelQuestion.Name = "labelQuestion"
        Me.labelQuestion.Size = New System.Drawing.Size(397, 21)
        Me.labelQuestion.TabIndex = 17
        Me.labelQuestion.Text = "How does TryParse differ from Parse?"
        '
        'GroupUsageTryParse
        '
        Me.GroupUsageTryParse.Controls.Add(Me.labelUsagePatternTryParse)
        Me.GroupUsageTryParse.Location = New System.Drawing.Point(429, 182)
        Me.GroupUsageTryParse.Name = "GroupUsageTryParse"
        Me.GroupUsageTryParse.Size = New System.Drawing.Size(381, 195)
        Me.GroupUsageTryParse.TabIndex = 19
        Me.GroupUsageTryParse.TabStop = False
        Me.GroupUsageTryParse.Text = "Usage Pattern (TryParse)"
        '
        'labelUsagePatternTryParse
        '
        Me.labelUsagePatternTryParse.Font = New System.Drawing.Font("Courier New", 9.75!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(0, Byte))
        Me.labelUsagePatternTryParse.Location = New System.Drawing.Point(7, 17)
        Me.labelUsagePatternTryParse.Name = "labelUsagePatternTryParse"
        Me.labelUsagePatternTryParse.Size = New System.Drawing.Size(366, 161)
        Me.labelUsagePatternTryParse.TabIndex = 15
        '
        'GroupUsageParse
        '
        Me.GroupUsageParse.Controls.Add(Me.labelUsagePatternParse)
        Me.GroupUsageParse.Location = New System.Drawing.Point(430, 9)
        Me.GroupUsageParse.Name = "GroupUsageParse"
        Me.GroupUsageParse.Size = New System.Drawing.Size(381, 167)
        Me.GroupUsageParse.TabIndex = 20
        Me.GroupUsageParse.TabStop = False
        Me.GroupUsageParse.Text = "Usage Pattern (Parse)"
        '
        'labelUsagePatternParse
        '
        Me.labelUsagePatternParse.Font = New System.Drawing.Font("Courier New", 9.75!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(0, Byte))
        Me.labelUsagePatternParse.Location = New System.Drawing.Point(7, 19)
        Me.labelUsagePatternParse.Name = "labelUsagePatternParse"
        Me.labelUsagePatternParse.Size = New System.Drawing.Size(366, 141)
        Me.labelUsagePatternParse.TabIndex = 19
        '
        'FormHelp
        '
        Me.ClientSize = New System.Drawing.Size(816, 402)
        Me.Controls.Add(Me.GroupUsageParse)
        Me.Controls.Add(Me.GroupUsageTryParse)
        Me.Controls.Add(Me.labelQuestion)
        Me.Controls.Add(Me.labelDesc)
        Me.FormBorderStyle = System.Windows.Forms.FormBorderStyle.FixedSingle
        Me.MaximizeBox = False
        Me.MinimizeBox = False
        Me.Name = "FormHelp"
        Me.Text = "Parse Vs. TryParse"
        Me.TopMost = True
        Me.GroupUsageTryParse.ResumeLayout(False)
        Me.GroupUsageParse.ResumeLayout(False)
        Me.ResumeLayout(False)

    End Sub

    Friend Shared ReadOnly Property GetInstance() As FormHelp
        Get
            If m_DefaultInstance Is Nothing OrElse m_DefaultInstance.IsDisposed() Then
                SyncLock m_SyncObject
                    If m_DefaultInstance Is Nothing OrElse m_DefaultInstance.IsDisposed() Then
                        m_DefaultInstance = New FormHelp
                    End If
                End SyncLock
            End If
            Return m_DefaultInstance
        End Get
    End Property

    Private Shared m_DefaultInstance As FormHelp
    Private Shared m_SyncObject As New Object

#End Region

    Private Sub FormHelp_Load(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles MyBase.Load
        labelDesc.Text = _
                "Parse and TryParse take a string which is expected to be a " & Environment.NewLine & _
                "representation " & _
                "for a specific type, and build the given type based " & Environment.NewLine & _
                "on the string value. For example, " & Convert.ToChar(34) & "23" & Convert.ToChar(34) & _
                " can be parsed into an integer." & Environment.NewLine & Environment.NewLine & _
                "When Parse fails it will throw an exception, indicating that the string" & Environment.NewLine & _
                "could not be successfully translated into the given type. Obviously, " & Environment.NewLine & _
                "if your string can be parsed, this situation does not arise. But if it" & Environment.NewLine & _
                "cannot, then the exception route is a slow one." & Environment.NewLine & Environment.NewLine & _
                "TryParse takes a slightly different approach. If the string cannot be" & Environment.NewLine & _
                "parsed, then false is returned, and the out parameter value becomes " & Environment.NewLine & _
                "meaningless. This means that the act of throwing an exception is " & Environment.NewLine & _
                "avoided, and therefore, the slow route is avoided." & Environment.NewLine & Environment.NewLine & _
                "You can see this by attempting to Parse/TryParse strings into a given" & Environment.NewLine & _
                "type, and changing what percentage of the parsing should succeed." & Environment.NewLine & _
                "When at 100%, you will notice the APIs perform almost identically. " & Environment.NewLine & _
                "However, when you start reducing the success rate, Parse becomes" & Environment.NewLine & _
                "markedly worse than TryParse." & Environment.NewLine & Environment.NewLine & _
                "The outcome? We urge you to prefer TryParse over Parse!"

        labelUsagePatternParse.Text = "Try" & vbCrLf & _
                "    Dim i As Integer = Int32.Parse(someString)" & vbCrLf & _
                "    ' take successful action, on i" & vbCrLf & _
                "Catch ex As Exception" & vbCrLf & _
                "    ' take unsuccessful action" & vbCrLf & _
                "    ' this route is slow" & vbCrLf & _
                "End Try"


        labelUsagePatternTryParse.Text = "Dim i as Integer" & vbCrLf & _
                "If Int32.TryParse(someString, i) Then" & vbCrLf & _
                "    ' take successful action" & vbCrLf & _
                "    ' note, this includes processing i" & vbCrLf & _
                "Else" & vbCrLf & _
                "    ' take unsuccessful action." & vbCrLf & _
                "    ' Do NOT process outValue" & vbCrLf & _
                "    ' this route is generally as fast" & vbCrLf & _
                "    ' as the success route" & vbCrLf & _
                "End If"

    End Sub
End Class
