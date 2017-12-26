'
' File: Legion.vb
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

   Public Class Legion
   
       ' The Game
       Private currentGame As Game
   
       ' Which Roman's image is to draw
       Private idxDirection As Integer = 0
   
       ' Legion Maximum Top position
       Private StartTop As Integer = 50
   
       ' Column number 
       Private Columns As Integer = 6
       ' Row number 
       Private Rows As Integer = 6
   
       ' Left speed
       Private horizontalVelocity As Integer = 1
       ' Down speed
       Private verticalVelocity As Integer = 0
   
       ' Total Romans number in the legion
       Private CountOfRomans As Integer = Columns * Rows - 1
   
       ' Array of Romans
       Private romans() As Roman
   
       ' Bounding Box for the Legion
       Dim boundingBox As Rectangle
   
       Public Sub New(ByVal game As Game)
   
           Dim row As Integer
           Dim col As Integer
   
           '  Roman adding
           Dim newRoman As Roman
   
           currentGame = game
   
           ' Create Romans
           ReDim romans(CountOfRomans + 1)
           For row = 0 To Rows - 1
   
               For col = 0 To Columns - 1
   
                   newRoman = New Roman(currentGame, StartTop, row, col)
                   romans(row * Columns + col) = newRoman
               Next
           Next
       End Sub
   
       Public Sub Process()
           Dim iRoman As Integer
           Dim iLastRoman As Integer
   
           verticalVelocity = 0
   
           ' Check walls
   
           ' If reach the right wall
           If ((boundingBox.Left + horizontalVelocity) < 0) Then
               horizontalVelocity = Math.Abs(horizontalVelocity)
               verticalVelocity = 10
           End If
   
           ' If reach the left wall
           If ((boundingBox.Right + horizontalVelocity) >= currentGame.WorldBounds.Width) Then
               horizontalVelocity = -Math.Abs(horizontalVelocity)
               verticalVelocity = 10
           End If
   
           ' If the Legion reach the Barbarian
           If (boundingBox.Bottom >= currentGame.Barbarian.Bounds.Top) Then
               currentGame.Lost()
           End If
   
           ' Process all Romans alive
           iLastRoman = -1
           For iRoman = 0 To CountOfRomans
   
               If (True = romans(iRoman).IsAlive) Then
                   romans(iRoman).Process(horizontalVelocity, verticalVelocity)
                   iLastRoman = iRoman
               End If
   
           Next
   
           ' The last Roman launch the Arrow
           If ((Not currentGame.RomanArrow.IsAlive) And (iLastRoman >= 0)) Then
                currentGame.RomanArrow.Start( _
                    romans(iLastRoman).CellLeft + CInt(romans(iLastRoman).CellWidth / 2), _
                    romans(iLastRoman).CellTop + romans(iLastRoman).CellHeight, _
                    0, _
                    5)
           End If
       End Sub
   
       Public Sub Draw(ByVal gx As Graphics)
           Dim iRoman As Integer
   
           ' Try to draw all Romans
           For iRoman = 0 To CountOfRomans
               romans(iRoman).CurrentImage = idxDirection
               romans(iRoman).Draw(gx)
           Next
   
           '  Switch the Romans Bitmap
           If (idxDirection = 0) Then
               idxDirection = 1
           Else
               idxDirection = 0
           End If
       End Sub
   
   
       Public Property DirtyArea() As Rectangle
           ' Return the Bounding Box as the Dirty area
           Get
               Dim rc As Rectangle
   
               rc = boundingBox
               If (horizontalVelocity > 0) Then
   
                   rc.Width += horizontalVelocity
   
               Else
                   rc.Width -= horizontalVelocity
                   rc.X += horizontalVelocity
               End If
               Return (rc)
           End Get
           Set(ByVal Value As Rectangle)
   
           End Set
       End Property
   
       Public Sub Restore()
   
           '  All Romans killed ?
           Dim fAllDead As Boolean = True
           ' Is the first
           Dim fFirstAlive As Boolean = True
   
           ' all Romans
           Dim iRoman As Integer
           For iRoman = 0 To CountOfRomans
               ' If Roman not dead
               If (romans(iRoman).IsAlive) Then
                   ' If is the first Roman init the Bound box as him
                   If (fFirstAlive) Then
                       boundingBox = romans(iRoman).DirtyArea
                       fFirstAlive = False
                       '  Add the Roman to the Bound Box
                   Else
                       boundingBox = Rectangle.Union(boundingBox, romans(iRoman).DirtyArea)
                   End If
                   fAllDead = False
                   romans(iRoman).Restore()
               End If
           Next
           ' If no Romans alive, the level is finished
           If (fAllDead) Then
               currentGame.Won()
           End If
       End Sub
   
       Public Sub Reset(ByVal level As Integer)
   
           Dim iRoman As Integer
           Dim column As Integer
   
           ' increase the Legion speed
           horizontalVelocity = level
   
           ' Reset All Legion
           boundingBox = romans(0).DirtyArea
           For iRoman = 0 To CountOfRomans
               column = iRoman Mod Columns
                romans(iRoman).Reset(StartTop, CInt((iRoman - column) / Columns), column)
               boundingBox = Rectangle.Union(boundingBox, romans(iRoman).DirtyArea)
           Next
       End Sub
   
   
       Public Function KillRomanFromXY(ByVal x As Integer, ByVal y As Integer) As Boolean
           Dim iRoman As Integer
   
           KillRomanFromXY = False
   
           ' check all Romans
           For iRoman = 0 To CountOfRomans
               '  If the Roman is Alive
               If (True = romans(iRoman).IsAlive) Then
                   ' If tha Arrow hit the Roman
                   If (romans(iRoman).Bounds.Contains(x, y)) Then
                       KillRomanFromXY = True
                       ' Disable the Roman
                       romans(iRoman).IsAlive = False
   
                       ' Increase the Score
                        currentGame.Score.Value += 10 * (6 - CInt(iRoman / Columns))
                       Exit For
                   End If
               End If
           Next
           Return (KillRomanFromXY)
       End Function
   End Class

End Namespace
   
