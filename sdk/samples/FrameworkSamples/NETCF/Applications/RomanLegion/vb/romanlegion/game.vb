'
' File: Game.vb
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

   Public Class Game
       Inherits System.Windows.Forms.Form
   
       ' State of the game
       Private Enum Display As Integer
           Splash = 0
           Won = 1
           Game = 2
       End Enum
   
       ' Where we get the images
       Private baseDirectory As String = ""
   
       ' Width of Game
       Private gameWidth As Integer
   
       ' Height of Game
       Private gameHeight As Integer
   
       '  Game is running ?
       Private fRunning As Boolean
   
       ' Mouse/Pen Position
       Private xMouse As Integer
       Private yMouse As Integer
   
       '  Is to Repaint the game
       Private fRepaint As Boolean
   
       Private backgroundBitmap As Bitmap
       Private offscreenGraphics As Graphics
       Private formGraphics As Graphics
       Private statusBitmaps() As Bitmap
   
       ' The Barbarian launching arrows
       Private player As Barbarian
   
       ' The Arrow
       Private currentPlayerArrow As Arrow
   
       Private currentRomanArrow As Arrow
   
       ' Legion Attacking
       Private attackingLegion As Legion
   
       ' Game Score
       Private currentScore As Score
   
       ' Last update tick
       Private lastTick As Integer
   
       ' Game status
       Private currentStatus As Display
   
       ' Game Paused ?
       Private fPaused As Boolean = False
   
   #Region " Windows Form Designer generated code "
   
       Public Sub New()
           MyBase.New()
   
           'This call is required by the Windows Form Designer.
           InitializeComponent()
   
           Dim strAssmPath As String = System.Reflection.Assembly.GetExecutingAssembly().GetName().CodeBase
           baseDirectory = strAssmPath.Substring(0, strAssmPath.LastIndexOf("\") + 1)
   
           Me.BackColor = Color.Black
   
           ' Initialize screen size
           Me.Size = New Size( _
                   System.Windows.Forms.Screen.PrimaryScreen.Bounds.Width, _
                   System.Windows.Forms.Screen.PrimaryScreen.Bounds.Height - 24)
   
           Me.Visible = True
   
           ' Get game client window size
           Me.gameWidth = Me.ClientSize.Width
           Me.gameHeight = Me.ClientSize.Height
   
           ' Window caption
           Me.Text = "Roman Legion"
   
           Init()
           ' Start running
           fRunning = True
       End Sub
   
       'Form overrides dispose to clean up the component list.
       Protected Overloads Overrides Sub Dispose(ByVal disposing As Boolean)
           MyBase.Dispose(disposing)
       End Sub
   
       'NOTE: The following procedure is required by the Windows Form Designer
       'It can be modified using the Windows Form Designer.  
       'Do not modify it using the code editor.
       Private Sub InitializeComponent()
           '
           'Game
           '
           Me.ClientSize = New System.Drawing.Size(240, 295)
   
       End Sub
   
       Public Shared Sub Main()
           Application.Run(New Game())
       End Sub
   
   #End Region
   
   
       Private Sub GameMainloop()
           Dim y As Integer
           Dim gx As Graphics
   
           ' get current tick count
           lastTick = Environment.TickCount
   
           InitGame()
   
           ' Game Running
           While (fRunning)
   
               ' Do game Step
               DoTick()
   
               ' Process messages to this window
               Application.DoEvents()
           End While
   
           ' Game over, paused or won the level
           gx = Me.CreateGraphics()
   
           ' Draw the bitmap correspondent to game status
           For y = 0 To Me.ClientSize.Height Step 10
               gx.DrawImage(statusBitmaps(currentStatus), 0, -gameHeight + y)
               System.Threading.Thread.Sleep(10)
           Next
   
           gx.Dispose()
       End Sub
   
       Private Sub DoTick()
           Dim tickSleep As Integer
   
           ' Let the game components move
           ProcessAll()
   
           ' Redraw the game components
           DrawAll()
   
           ' Game timming (Wait next step)
            tickSleep = lastTick + CInt(1000 / 70) - Environment.TickCount
           If (tickSleep > 0) Then
               System.Threading.Thread.Sleep(tickSleep)
           End If
           lastTick = Environment.TickCount
   
           RefreshAll()
           RestoreAll()
       End Sub
   
       Private Sub Init()
   
           ' Init game components
           player = New Barbarian(Me)
           currentPlayerArrow = New Arrow(Me)
           currentRomanArrow = New Arrow(Me)
           attackingLegion = New Legion(Me)
           currentScore = New Score(Me)
   
           ' Init the mouse x position as the middle of the screen
            xMouse = CInt(gameWidth / 2)
   
           ' Load all results bitmaps
           ReDim statusBitmaps(4)
   
           Dim Prefix As String = "PPC"
   
           LoadBmp(Prefix + "Splash.bmp", Display.Splash)
           LoadBmp(Prefix + "Won.bmp", Display.Won)
           LoadBmp(Prefix + "Pause.bmp", Display.Game)
           LoadBmp(Prefix + "Background.bmp", 3)
            backgroundBitmap = New Bitmap(baseDirectory + "Content\" + Prefix + "Background.bmp")
   
           'Create offscreen playscren
           offscreenGraphics = Graphics.FromImage(backgroundBitmap)
           formGraphics = Me.CreateGraphics()
       End Sub
   
       Private Sub InitGame()
           ' Game is running
           fRunning = True
   
           ' If the game was not paused
           If (Not fPaused) Then
               ' Disable all arrows
               currentPlayerArrow.Stuck  = True
               currentRomanArrow.Stuck   = True
               currentRomanArrow.IsAlive = False
               ' Game paused
           Else
               ' Release the game
               fPaused = False
           End If
   
           fRepaint = True
       End Sub
   
       Private Sub ProcessAll()
           ' Get mouse position
           xMouse = Me.PointToClient(Control.MousePosition).X
   
           ' Calculate the components new position
           player.Process()
           currentPlayerArrow.Process()
           currentRomanArrow.Process()
           attackingLegion.Process()
       End Sub
   
       Private Sub DrawAll()
   
           ' Redraw all game components in the new position
           currentRomanArrow.Draw(offscreenGraphics)
           currentPlayerArrow.Draw(offscreenGraphics)
           player.Draw(offscreenGraphics)
           attackingLegion.Draw(offscreenGraphics)
           currentScore.Draw(offscreenGraphics)
       End Sub
   
       Public Sub Refresh1(ByVal rcDirty As Rectangle)
           ' Refresh components last position
           formGraphics.DrawImage(backgroundBitmap, rcDirty.X, rcDirty.Y, rcDirty, GraphicsUnit.Pixel)
       End Sub
   
       Private Sub RefreshAll()
   
           ' Refresh the components
           Refresh1(player.DirtyArea)
           Refresh1(currentPlayerArrow.DirtyArea)
           Refresh1(currentRomanArrow.DirtyArea)
           Refresh1(attackingLegion.DirtyArea)
           Refresh1(currentScore.DirtyArea)
   
           ' Redraw everything
           If (fRepaint) Then
               formGraphics.DrawImage(backgroundBitmap, 0, 0)
               fRepaint = False
           End If
       End Sub
   
       Private Sub RestoreAll()
           ' Clean where the components were
           player.Restore()
           currentPlayerArrow.Restore()
           currentRomanArrow.Restore()
           attackingLegion.Restore()
           currentScore.Restore()
       End Sub
   
       Public Sub Run()
           Application.Run(Me)
       End Sub
   
       Public Sub Lost()
           ' Game Over 
           ' Reset score and level
           currentScore.Value = 0
           currentScore.Level = 1
   
           ' Reset the Legion position
           attackingLegion.Reset(currentScore.Level)
   
           ' Not running
           fRunning = False
   
           ' Show splash image
           currentStatus = Display.Splash
       End Sub
   
       Public Sub Won()
           ' Game won
   
           ' Increase the level
           currentScore.Level = currentScore.Level + 1
   
           ' Reset platoon position
           attackingLegion.Reset(currentScore.Level)
   
           ' Not running
           fRunning = False
   
           ' Show Won Image
           currentStatus = Display.Won
       End Sub
   
       ' Expose arrow so we can start it from the legion
       Public Property RomanArrow() As Arrow
           Get
               Return currentRomanArrow
           End Get
           Set(ByVal Value As Arrow)
   
           End Set
       End Property
   
       ' Expose the Barbarian to chech game over (hit on Barbarian)
       Public Property Barbarian() As Barbarian
           Get
               Return player
           End Get
           Set(ByVal Value As Barbarian)
   
           End Set
       End Property
       
       Public ReadOnly Property ExecutablePath() As String
           Get
               Return baseDirectory
           End Get
       End Property
   
       ' Expose the Legion to check which roman where killed
       Public Property Legion() As Legion
           Get
               Return attackingLegion
           End Get
           Set(ByVal Value As Legion)
   
           End Set
       End Property
   
       ' Expose the Score so we can update it when the Romans where killed
       Public Property Score() As Score
           Get
               Return currentScore
           End Get
           Set(ByVal Value As Score)
   
           End Set
       End Property
   
   
       ' Graphics Offline
       Public Property BufferGraphics() As Graphics
           Get
               Return offscreenGraphics
           End Get
           Set(ByVal Value As Graphics)
   
           End Set
       End Property
   
       ' Background BMP
       Public Property BackBmp() As Bitmap
           Get
               Return statusBitmaps(3)
           End Get
           Set(ByVal Value As Bitmap)
           End Set
       End Property
   
       Public Property WorldBounds() As Size
           Get
               Return New Size(gameWidth, gameHeight)
           End Get
           Set(ByVal Value As Size)
   
           End Set
       End Property
   
       '  Expose the Mouse position to Barbarian get self updated
       Public Property MouseLoc() As Point
           Get
               Return New Point(xMouse, yMouse)
           End Get
           Set(ByVal Value As Point)
   
           End Set
       End Property
   
       Protected Overrides Sub OnMouseDown(ByVal e As MouseEventArgs)
           Select Case currentStatus
               Case Display.Game
                   If (fRunning) Then
                       player.KeyboardOff()
   
                       'Pause the Game
                       If (e.Y < 15) Then
                           fRunning = False
                           fPaused = True
                       End If
   
                       'Release the Game
                   Else
                       Me.GameMainloop()
                   End If
   
               Case Display.Splash To Display.Won
                   currentStatus = Display.Game
                   Me.GameMainloop()
           End Select
       End Sub
   
       Protected Overrides Sub OnMouseUp(ByVal e As MouseEventArgs)
           Select Case currentStatus
               Case Display.Game
                   If (fRunning) Then
                       ' Launch the Arrow
                       If (currentPlayerArrow.Stuck = True) Then
                            currentPlayerArrow.Start(player.CellLeft + CInt(player.CellWidth / 2), WorldBounds.Height - 37, 0, -5)
                       End If
                   End If
           End Select
       End Sub
   
       Protected Overrides Sub OnMouseMove(ByVal e As MouseEventArgs)
           ' Update mouse position
           xMouse = e.X
           yMouse = e.Y
       End Sub
   
       Protected Overrides Sub OnPaint(ByVal e As PaintEventArgs)
           Dim gx As Graphics
           gx = e.Graphics
   
           Select Case currentStatus
               Case Display.Game
                   gx.DrawImage(statusBitmaps(3), 0, 0)
                   fRepaint = True
               Case Display.Won
                   gx.DrawImage(statusBitmaps(1), 0, 0)
               Case Display.Splash
                   gx.DrawImage(statusBitmaps(0), 0, 0)
           End Select
       End Sub
   
       Public Sub LoadBmp(ByVal bmpName As String, ByVal i As Integer)
   
           ' Try to Load the Bitmap
           Try
                statusBitmaps(i) = New Bitmap(baseDirectory + "Content\" + bmpName)
           Catch
               ' Any error ? Create a bitmap
               statusBitmaps(i) = New Bitmap(Me.Width, Height)
               Graphics.FromImage(statusBitmaps(i)).FillRectangle(New SolidBrush(Me.BackColor), New Rectangle(0, 0, Width - 1, Height - 1))
               Graphics.FromImage(statusBitmaps(i)).DrawLine(New Pen(Color.Red), 0, 0, Width - 1, Height - 1)
               Graphics.FromImage(statusBitmaps(i)).DrawLine(New Pen(Color.Red), Width - 1, 0, 0, Height - 1)
           End Try
       End Sub
   
       Private Sub RomanLegionOnClosing(ByVal sender As Object, ByVal e As System.ComponentModel.CancelEventArgs) Handles MyBase.Closing
           If (fRunning) Then
               fRunning = False
               e.Cancel = True
           End If
       End Sub
   
       Private Sub RomanLegionClosed(ByVal sender As Object, ByVal e As System.EventArgs) Handles MyBase.Closed
           Me.fRunning = False
       End Sub
   
       Protected Overrides Sub OnKeyDown(ByVal e As KeyEventArgs)
           ' Enable to use keyboard 
           Select Case e.KeyData
               Case Keys.Left
                   ' Start going left
                   player.GoLeft(True)
               Case Keys.Right
                   ' Start going right
                   player.GoRight(True)
           End Select
       End Sub
   
       Protected Overrides Sub OnKeyUp(ByVal e As KeyEventArgs)
           Select Case e.KeyData
               Case Keys.Left
                   ' Stop Going left
                   player.GoLeft(False)
               Case Keys.Right
                   ' Stop going right
                   player.GoRight(False)
               Case Keys.Space
                   ' Launch Arrow
                   If (currentPlayerArrow.IsAlive = False) Then
                        currentPlayerArrow.Start(player.CellLeft + CInt(player.CellWidth / 2), WorldBounds.Height - 37, 0, -5)
                   End If
               Case Keys.P
                   ' Pause game
                   fRunning = False
                   fPaused = True
   
               Case Keys.S
                   ' Start game
                   currentPlayerArrow.IsAlive = False
                   Me.GameMainloop()
   
           End Select
       End Sub
   
   End Class

End Namespace

