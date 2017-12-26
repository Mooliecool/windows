'****************************** Module Header ******************************\
' Module Name:  TicTacToeViewModel.vb
' Project:	    VBWPFMVVMPractice
' Copyright (c) Microsoft Corporation.
' 
' The TicTacToeViewModel class is a ViewModel in the MVVM pattern, which 
' contains the game's logic and data.
'  
' This source is subject to the Microsoft Public License.
' See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
' All other rights reserved.
' 
' THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND, 
' EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED 
' WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
'***************************************************************************/

Imports System.ComponentModel
Imports System.Threading
Imports System.Collections.ObjectModel

Public Class TicTacToeViewModel
    Implements INotifyPropertyChanged

    Private _cells As ObservableCollection(Of Cell)
    Private _changeDimensionCommand As RelayCommand
    Private _currentMoveNumber As Integer
    Private _currentPlayerName As String
    Private _dimension As Integer
    Private _moveCommand As RelayCommand

    ''' <summary>
    ''' The readonly Cells property returns a collection of cells of the current game.
    ''' </summary>
    Public ReadOnly Property Cells As ObservableCollection(Of Cell)
        Get
            Return Me._cells
        End Get
    End Property

    ''' <summary>
    ''' The ChangeDimensionCommand represents a request to change the dimension of the game.
    ''' </summary>
    Public ReadOnly Property ChangeDimensionCommand As ICommand
        Get
            If (Me._changeDimensionCommand Is Nothing) Then

                Me._changeDimensionCommand =
                    New RelayCommand(
                        Function(dimension As Object)
                            Me.Dimension = Convert.ToInt32(dimension)
                            Me.NewGame()
                        End Function)
            End If
            Return Me._changeDimensionCommand
        End Get
    End Property

    ''' <summary>
    ''' The CurrentPlayerName property gets and sets the name of the current player.
    ''' </summary>
    Public Property CurrentPlayerName As String
        Get
            Return Me._currentPlayerName
        End Get
        Private Set(ByVal value As String)
            If (Me._currentPlayerName <> value) Then
                Me._currentPlayerName = value
                Me.Notify("CurrentPlayerName")
            End If
        End Set
    End Property


    ''' <summary>
    ''' The Dimension property gets and sets the dimension of the game.
    ''' </summary>
    Public Property Dimension As Integer
        Get
            Return Me._dimension
        End Get
        Set(ByVal value As Integer)
            If (Me._dimension <> value) Then
                Me._dimension = value
                Me.Notify("Dimension")
                If (Me._cells Is Nothing) Then
                    Me._cells = New ObservableCollection(Of Cell)
                Else
                    Me._cells.Clear()
                End If
                Dim i As Integer
                For i = 0 To (Me._dimension * Me._dimension) - 1
                    Me._cells.Add(New Cell(i))
                Next i
            End If
        End Set
    End Property

    ''' <summary>
    ''' The MoveCommand represents a request to move on a given cell.
    ''' </summary>
    Public ReadOnly Property MoveCommand As ICommand
        Get
            If (Me._moveCommand Is Nothing) Then
                Me._moveCommand =
                    New RelayCommand(
                        Function(cellnumber As Object)
                            Me.Move(Convert.ToInt32(cellnumber))
                        End Function,
                        Function(cellnumber As Object)
                            Return Me.CanMove(Convert.ToInt32(cellnumber))
                        End Function)
            End If
            Return Me._moveCommand
        End Get
    End Property

    Public Sub New(ByVal dimension As Integer)
        Me.Dimension = dimension
    End Sub

    ''' <summary>
    ''' The NewGame method starts a new game.
    ''' </summary>
    Public Sub NewGame()

        ' Reset the Move property of all the Cell objects.    
        Dim i As Integer
        For i = 0 To Me.Cells.Count - 1
            Me._cells.Item(i).Move = Nothing
        Next i

        ' Set the current player name and current move number.
        Me.CurrentPlayerName = "x"
        Me._currentMoveNumber = 1
    End Sub


    ''' <summary>
    ''' The CanMove method determines whether a player can move on the specified cell.
    ''' </summary>
    ''' <param name="cellNumber">The number of the cell</param>
    ''' <returns>If true, can move; otherwise, can't move</returns>
    Private Function CanMove(ByVal cellNumber As Integer) As Boolean
        If (cellNumber >= (Me._dimension * Me._dimension)) Then
            Return False
        End If
        Return (Me._cells.Item(cellNumber).Move Is Nothing)
    End Function

    ''' <summary>
    ''' The Move method sets a PlayerMove object for the current player on the specified Cell object. 
    ''' </summary>
    ''' <param name="cellNumber">The number of the cell</param>
    Private Sub Move(ByVal cellNumber As Integer)
        Me._cells.Item(cellNumber).Move = New PlayerMove(Me._currentPlayerName, Me._currentMoveNumber)
        Me._currentMoveNumber += 1

        ' Check if the game is over, i.e. the current player wins the game or the game is tie. If 
        ' so, raise the GameOver event and starts a new game.            
        If Me.HasWon(Me.CurrentPlayerName) Then
            Me.RaiseGameOverEvent(New GameOverEventArgs With {
                                  .IsTie = False,
                                  .WinnerName = Me._currentPlayerName
                              })
            Me.NewGame()
        ElseIf Me.TieGame Then
            Me.RaiseGameOverEvent(New GameOverEventArgs With {
                .IsTie = True
            })
            Me.NewGame()

            ' If the game isn't over, switch the current player.    
        ElseIf Me.CurrentPlayerName.Equals("x") Then
            Me.CurrentPlayerName = "o"
        Else
            Me.CurrentPlayerName = "x"
        End If
    End Sub

    ''' <summary>
    ''' The HasWon method checks whether the given player has won.
    ''' </summary>
    ''' <param name="player">The name of the player</param>
    ''' <returns></returns>
    Private Function HasWon(ByVal player As String) As Boolean

        ' Check the rows in the game grid.
        Dim j As Integer
        Dim i As Integer = 0
        Do While (i <= ((Me._dimension - 1) * Me._dimension))
            j = 0
            Do While (j <= (Me._dimension - 1))
                If Not ((Not Me._cells.Item((i + j)).Move Is Nothing) AndAlso
                        Me._cells.Item((i + j)).Move.PlayerName.Equals(player)) Then
                    Exit Do
                End If
                j += 1
            Loop
            If (j = Me._dimension) Then
                j = 0
                Do While (j <= (Me._dimension - 1))
                    Me._cells.Item((i + j)).Move.IsPartOfWin = True
                    j += 1
                Loop
                Return True
            End If
            i = (i + Me._dimension)
        Loop

        ' Check the columns in the game grid.  
        j = 0
        Do While (j <= (Me._dimension - 1))
            i = 0
            Do While (i <= ((Me._dimension - 1) * Me._dimension))
                If Not ((Not Me._cells.Item((i + j)).Move Is Nothing) AndAlso
                        Me._cells.Item((i + j)).Move.PlayerName.Equals(player)) Then
                    Exit Do
                End If
                i = (i + Me._dimension)
            Loop
            If (i = (Me._dimension * Me._dimension)) Then
                i = 0
                Do While (i <= ((Me._dimension - 1) * Me._dimension))
                    Me._cells.Item((i + j)).Move.IsPartOfWin = True
                    i = (i + Me._dimension)
                Loop
                Return True
            End If
            j += 1
        Loop

        ' Check the diagonal line ( "\" ) in the game grid.
        Dim x As Integer = 0
        Do While (x < (Me._dimension * Me._dimension))
            If Not ((Not Me._cells.Item(x).Move Is Nothing) AndAlso Me._cells.Item(x).Move.PlayerName.Equals(player)) Then
                Exit Do
            End If
            x = (x + (Me._dimension + 1))
        Loop
        If (x = ((Me._dimension * Me._dimension) + Me._dimension)) Then
            x = 0
            Do While (x < (Me._dimension * Me._dimension))
                Me._cells.Item(x).Move.IsPartOfWin = True
                x = (x + (Me._dimension + 1))
            Loop
            Return True
        End If

        ' Check the diagonal line ( "/" ) in the game grid.  
        Dim y As Integer = (Me._dimension - 1)
        Do While (y <= (Me._dimension * (Me._dimension - 1)))
            If Not ((Not Me._cells.Item(y).Move Is Nothing) AndAlso Me._cells.Item(y).Move.PlayerName.Equals(player)) Then
                Exit Do
            End If
            y = (y + (Me._dimension - 1))
        Loop
        If (y = ((Me._dimension * Me._dimension) - 1)) Then
            y = (Me._dimension - 1)
            Do While (y <= (Me._dimension * (Me._dimension - 1)))
                Me._cells.Item(y).Move.IsPartOfWin = True
                y = (y + (Me._dimension - 1))
            Loop
            Return True
        End If

        ' If all the checks above fail, return false.
        Return False
    End Function

    ''' <summary>
    ''' The TieGame method checks if the game is tie.
    ''' </summary>
    Private Function TieGame() As Boolean
        Dim nomove As Boolean = True
        Dim i As Integer
        For i = 0 To (Me.Dimension * Me.Dimension) - 1
            If (Me.Cells.Item(i).Move Is Nothing) Then
                nomove = False
                Exit For
            End If
        Next i
        Return nomove
    End Function

    ''' <summary>
    ''' The Notify method is used to raise a PropertyChanged event when a property value is 
    ''' changed
    ''' </summary>
    ''' <param name="propName">The name of the property whose value is changed.</param>
    Private Sub Notify(ByVal propName As String)
        RaiseEvent PropertyChanged(Me, New PropertyChangedEventArgs(propName))
    End Sub

    ''' <summary>
    ''' The RaiseGameOverEvent method raise the GameOverEvent
    ''' </summary>
    ''' <param name="e">A GameOverEventArgs object</param>
    Private Sub RaiseGameOverEvent(ByVal e As GameOverEventArgs)
        RaiseEvent GameOverEvent(Me, e)
    End Sub

    Public Event PropertyChanged As PropertyChangedEventHandler Implements INotifyPropertyChanged.PropertyChanged

    Public Class GameOverEventArgs
        Inherits EventArgs

        Public Property IsTie As Boolean
        Public Property WinnerName As String

    End Class

    Public Delegate Sub GameOverEventHandler(ByVal sender As Object, ByVal e As GameOverEventArgs)

    Public Event GameOverEvent As GameOverEventHandler

End Class

