'
' File: Arrow.vb
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

   Public Class Arrow
       Inherits BaseObj
       '<summary>
       'This Class handdles the Barbarian arrow and the Romans arrows
       '</summary>
   
       Private fMoving As Boolean
       
       Protected Property IsMoving as Boolean
           Get
               Return fMoving
           End Get
           Set
               fMoving = Value
           End Set
       End Property
   
       Public Sub New(ByVal game As Game)
           MyBase.New(game)
           CellWidth  = 3
           CellHeight = 10
   
           ' Position the arrow
            Start(CInt(MaximumX / 2), MaximumY - 30, 0, -5)
   
           ' Load bitmap
           ReDim AnimationCells(2)
           ReDim CellAttributes(2)
           Load("Arrow.bmp", 0)
           Load("Arrow1.bmp", 1)
       End Sub
   
       Public Sub Process()
   
           LeftOld = CellLeft
           TopOld  = CellTop
   
           ' Arrow ready to be launched
           If (Not fMoving) Then
   
               CellLeft = CurrentGame.Barbarian.Bounds.Left + CInt(CurrentGame.Barbarian.Bounds.Width / 2) - CInt(CellWidth / 2)
               CellTop  = CurrentGame.Barbarian.Bounds.Top  - CellHeight
   
               ' Arrow moving
           Else
   
               ' Move it
               CellTop += VerticalVelocity
   
               ' check if any Roman were killed
               If (CurrentGame.Legion.KillRomanFromXY(CellLeft, CellTop)) Then
   
                   ' Stop the arrow
                   Stop1()
               End If
   
               ' Arrow reach top of the window
               If (CellTop < 50) Then
   
                   ' Stop the arrow
                   Stop1()
               End If
   
               ' Arrow reach the bottom of the window
               If (CellTop > MaximumY) Then
   
                   Me.IsAlive = False
                   Me.Stuck   = True
               End If
   
               ' Check if hit the Barbarian
               If (CurrentGame.Barbarian.CollidesWith(Me)) Then
   
                   ' If the Arrow hit the Barbarian , game over
                   CurrentGame.Lost()
               End If
           End If
       End Sub
   
       ' Can Launch Arrow ?
       Public Property Stuck() As Boolean
   
           Get
   
               Return Not fMoving
           End Get
           Set(ByVal Value As Boolean)
   
               fMoving = Not Value
           End Set
       End Property
   
       Public Sub Start(ByVal x As Integer, ByVal y As Integer, ByVal horizontalVelocity As Integer, ByVal verticalVelocity As Integer)
   
           ' release the arrow
           fMoving = True
           IsAlive = True
   
           ' Arrow initial position
           LeftOld = CellLeft
           TopOld  = CellTop
           CellLeft = x - CInt(CellWidth / 2)
           CellTop  = y
   
           Me.HorizontalVelocity = horizontalVelocity
   
           ' Arrow up
           If (verticalVelocity < 0) Then
   
               CurrentImage = 0
   
               ' Arrow down
           Else
   
               CurrentImage = 1
           End If
   
           ' Arrow direction
           Me.VerticalVelocity = verticalVelocity
       End Sub
   
       Public Sub Stop1()
   
           ' Disable the arrow
           fMoving = False
           HorizontalVelocity = 0
           VerticalVelocity   = 0
       End Sub
   End Class

End Namespace
