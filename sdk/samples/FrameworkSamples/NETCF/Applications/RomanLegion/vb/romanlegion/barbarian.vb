'
' File: Barbarian.vb
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

   Public Class Barbarian
       Inherits BaseObj
   
       Private Const KeyboardMove As Integer = 5
   
       Private fMoveLeft As Boolean
       Private fMoveRight As Boolean
       Private fKeyboard As Boolean
   
       Public Sub New(ByVal game As Game)
           MyBase.New(game)
   
           'Barbarian Always Alive
           IsAlive = True
   
           ' Bmp Size
           CellWidth  = 14
           CellHeight = 10
   
           ' Start Position
           CellLeft = CInt(MaximumX / 2)
           CellTop  = CurrentGame.WorldBounds.Height - CellHeight - 20
           LeftOld = CellLeft
           TopOld  = CellTop
   
           ' Load Bmp
           ReDim AnimationCells(1)
           ReDim CellAttributes(1)
           Load("Barbarian.bmp", 0)
       End Sub
   
       Public Sub Process()
   
           Dim ptMouse As Point
           LeftOld = CellLeft
   
           ' If input is from Keyboard
           If (fKeyboard) Then
   
               If (fMoveLeft) Then
                   CellLeft -= KeyboardMove
               ElseIf (fMoveRight) Then
                   CellLeft += KeyboardMove
               End If
               ' If the mouse move
           Else
   
               ptMouse = CurrentGame.MouseLoc
               CellLeft = ptMouse.X - CInt(CellWidth / 2)
   
           End If
   
           ' Limit the Barbarian position to the window
           If (CellLeft < 0) Then
               CellLeft = 0
           ElseIf ((CellLeft + CellWidth) >= MaximumX) Then
               CellLeft = MaximumX - CellWidth
           End If
       End Sub
   
       ' Set the Barbarian to move left
       Public Sub GoLeft(move As Boolean)
           fKeyboard = True
           fMoveLeft = move
       End Sub
   
       ' set the Barbarian to move right
       Public Sub GoRight(move as Boolean)
           fKeyboard = True
           fMoveRight = move
       End Sub
   
       Public Sub KeyboardOff()
           fKeyboard = False
       End Sub
   
       Public Function CollidesWith(ByVal arrow As Arrow) As Boolean
   
           CollidesWith = False
           If (Me.Bounds.IntersectsWith(arrow.Bounds)) Then
               CollidesWith = True
           End If
           Return (CollidesWith)
       End Function
   End Class

End Namespace
   
