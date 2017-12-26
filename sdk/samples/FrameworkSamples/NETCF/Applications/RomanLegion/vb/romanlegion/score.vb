'
' File: Score.vb
'
'---------------------------------------------------------------------
' This file is part of the Microsoft .NET Framework SDK Code Samples.
'
'  Copyright (C) Microsoft Corporation.  All rights reserved.
' 
' This source code is intended only as a supplement to Microsoft
' Development Tools and/or on-line documentation.  See these other
' materials for detailed information regarding Microsoft code samples.
' 
' THIS CODE AND INFORMATION ARE PROVIDED AS IS WITHOUT WARRANTY OF ANY
' KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
' IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
' PARTICULAR PURPOSE.
'---------------------------------------------------------------------

Namespace Microsoft.Samples.RomanLegion

   Public Class Score
       Inherits BaseObj
   
       ' Score Font
       Dim drawFont As Font
       Dim drawBrush As SolidBrush
       Dim currentScore As Integer
       Dim currentLevel As Integer
   
       Public Sub New(ByVal game As Game)
           MyBase.New(game)
   
           ' Score Font
           drawFont = New Font("Arial", 16, System.Drawing.FontStyle.Bold)
   
           drawBrush = New SolidBrush(Color.BlueViolet)
   
           ' Create rectangle for drawing.
           CellLeft = 0
           CellTop  = 0
   
           LeftOld = CellLeft
           TopOld  = CellTop
           currentScore = 0
           currentLevel = 1
           IsAlive = True
           CellWidth  = 240
           CellHeight = 50
       End Sub
   
   
       Public Sub Process()
       End Sub
   
   
       Public Shadows Sub Draw(ByVal gx As Graphics)
           Dim Score As String
           Score = "Score: " + CStr(currentScore) + " Level: " + CStr(currentLevel)
           gx.DrawString(Score, drawFont, drawBrush, 5.0F, 10.0F)
       End Sub
   
   
       ' Expose the Score
       Public Property Value() As Integer
           Get
               Return (currentScore)
           End Get
           Set(ByVal Value As Integer)
               currentScore = Value
           End Set
       End Property
   
       ' Expose the Level
       Public Property Level() As Integer
           Get
               Return (currentLevel)
           End Get
           Set(ByVal Value As Integer)
               currentLevel = Value
           End Set
       End Property
   End Class

End Namespace

