'
' File: Roman.vb
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

   Public Class Roman
       Inherits BaseObj
   
       Public Sub New(ByVal game As Game, ByVal top As Integer, ByVal row As Integer, ByVal col As Integer)
           MyBase.New(game)
           ' Bmp Size
           CellWidth  = 16
           CellHeight = 20
   
           ' Load Roman Bmps 
           ReDim AnimationCells(2)
           ReDim CellAttributes(2)
           Load("RomanA" + CStr(row) + ".bmp", 0)
           Load("RomanB" + CStr(row) + ".bmp", 1)
   
           ' Reset Roman Position and turn it alive
           Reset(top, row, col)
   
       End Sub
   
       Public Sub Process(ByVal horizontalDelta As Integer, ByVal verticalDelta As Integer)
   
           LeftOld = CellLeft
           TopOld  = CellTop
           ' Move
           CellLeft += horizontalDelta
           CellTop  += verticalDelta
       End Sub
   
       Public Sub Reset(ByVal newTop As Integer, ByVal row As Integer, ByVal col As Integer)
   
           ' Turn the Roman on
           Me.IsAlive = True
   
           ' Reset the position
           CellLeft = (CellWidth * 2) * col
           CellTop  = newTop + (row * CellHeight)
       End Sub
   End Class

End Namespace

