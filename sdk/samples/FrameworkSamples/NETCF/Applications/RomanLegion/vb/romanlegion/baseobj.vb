'
' File: BaseObj.vb
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

   Public Class BaseObj
   
       ' Image index to draw
       Private myActiveImage As Integer = 0
   
       ' Object's Left
       Private myLeft As Integer
       ' Object's Top
       Private myTop As Integer
   
       ' Object's Left before move
       Private myLeftOld As Integer
       ' Object's Top before move
       Private myTopOld As Integer
   
       ' Object's Speed to right 
       Private myHorizontalVelocity As Integer
       ' Object's Speed to down
       Private myVerticalVelocity As Integer
   
       ' Object's Width 
       Private myWidth As Integer
       ' Object's Height
       Private myHeight As Integer
   
       ' Max x Position
       Private myMaximumX As Integer
       ' Max y Position
       Private myMaximumY As Integer
   
       ' Current game
       Private myCurrentGame As Game
   
       ' is to draw ?
       Private fActive As Boolean
   
       ' Object Bitmap
       Private myCells() As Bitmap
   
       ' Object Attributes (which color range is transparent ?)
       Private myCellAttributes() As Imaging.ImageAttributes
   
        Protected Property LeftOld As Integer
            Get
                Return myLeftOld
            End Get
            Set
                myLeftOld = Value
            End Set
        End Property

        Protected Property TopOld As Integer
            Get
                Return myTopOld
            End Get
            Set
                myTopOld = Value
            End Set
        End Property

        Protected Property HorizontalVelocity As Integer
            Get
                Return myHorizontalVelocity 
            End Get
            Set
                myHorizontalVelocity = Value
            End Set
        End Property

        Protected Property VerticalVelocity As Integer
            Get
                Return myVerticalVelocity
            End Get
            Set
                myVerticalVelocity = Value
            End Set
        End Property

        Protected Property MaximumX As Integer
            Get
                Return myMaximumX 
            End Get
            Set
                myMaximumX = Value
            End Set
        End Property

        Protected Property MaximumY As Integer
            Get
                Return myMaximumY
            End Get
            Set
                myMaximumY = Value
            End Set
        End Property

        Protected Property CurrentGame As Game
            Get
                Return myCurrentGame
            End Get
            Set
                myCurrentGame = Value
            End Set
        End Property

        Protected Property AnimationCells As Bitmap()
            Get
                Return myCells
            End Get
            Set
                myCells = Value
            End Set
        End Property

        Protected Property CellAttributes As System.Drawing.Imaging.ImageAttributes()
            Get
                Return myCellAttributes
            End Get
            Set
                myCellAttributes = Value
            End Set
        End Property

       Public Sub New(ByVal game As Game)
   
           myCurrentGame = game
   
           myMaximumX = myCurrentGame.WorldBounds.Width
           myMaximumY = myCurrentGame.WorldBounds.Height
   
           myLeftOld = 0
           myTopOld = 0
       End Sub
   
       Public Sub Draw(ByVal gx As Graphics)
   
           ' if is to draw
           If (fActive) Then
   
               gx.DrawImage( _
                   myCells(myActiveImage), _
                   New Rectangle(myLeft, myTop, myWidth, myHeight), _
                   0, _
                   0, _
                   myWidth, _
                   myHeight, _
                   GraphicsUnit.Pixel, _
                   myCellAttributes(myActiveImage))
           End If
       End Sub
   
       Public Sub Load(ByVal bmpName As String, ByVal bitmapIndex As Integer)
   
           Try
   
               ' Load BMP from file
                myCells(bitmapIndex) = New Bitmap(myCurrentGame.ExecutablePath + "Content\" + bmpName)
               myWidth = myCells(bitmapIndex).Width
               myHeight = myCells(bitmapIndex).Height
   
           Catch
   
               ' Any error reading the BMP, create a dummy one
               myCells(bitmapIndex) = New Bitmap(myWidth, myHeight)
               Graphics.FromImage(myCells(bitmapIndex)).FillEllipse(New SolidBrush(Color.White), New Rectangle(0, 0, myWidth - 1, myHeight - 1))
           End Try
   
           ' Get the color of a background pixel as the Pixel 0,0 
           Dim TranspColor As Color = myCells(bitmapIndex).GetPixel(0, 0)
   
           ' Set the Attributes for the Transparent color
           myCellAttributes(bitmapIndex) = New Imaging.ImageAttributes()
           myCellAttributes(bitmapIndex).SetColorKey(TranspColor, TranspColor)
       End Sub
   
       Public Sub Restore()
   
           ' Restore the Game Background
           myCurrentGame.BufferGraphics.DrawImage( _
               myCurrentGame.BackBmp, _
               myLeft, _
               myTop, _
               New Rectangle(myLeft, myTop, myWidth, myHeight), _
               GraphicsUnit.Pixel)
       End Sub
   
       Public Property Bounds() As Rectangle
           Get
               Return (New Rectangle(myLeft, myTop, myWidth, myHeight))
           End Get
           Set(ByVal Value As Rectangle)
           End Set
       End Property
   
       Public Property DirtyArea() As Rectangle
           Get
               Return (Rectangle.Union(New Rectangle(myLeft, myTop, myWidth, myHeight), New Rectangle(myLeftOld, myTopOld, myWidth, myHeight)))
           End Get
           Set(ByVal Value As Rectangle)
           End Set
       End Property
   
       ' Expose object Draw property
       Public Property IsAlive As Boolean
           Get
               Return (fActive)
           End Get
           Set(ByVal Value As Boolean)
               fActive = Value
           End Set
       End Property
       
       Public Property CurrentImage As Integer
           Get
               Return (myActiveImage)
           End Get
           Set
               myActiveImage = Value
           End Set
       End Property
       
       Public Property CellLeft As Integer
           Get
               Return (myLeft)
           End Get
           Set
               myLeft = Value
           End Set
       End Property
       
       Public Property CellTop As Integer
           Get
               Return (myTop)
           End Get
           Set
               myTop = Value
           End Set
       End Property
       
       Public Property CellWidth As Integer
           Get
               Return (myWidth)
           End Get
           Set
               myWidth = Value
           End Set
       End Property
       
       Public Property CellHeight As Integer
           Get
               Return (myHeight)
           End Get
           Set
               myHeight = Value
           End Set
       End Property
       
   End Class

End Namespace
   
