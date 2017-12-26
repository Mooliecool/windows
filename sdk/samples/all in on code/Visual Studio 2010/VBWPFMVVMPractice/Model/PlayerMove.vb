'****************************** Module Header ******************************\
' Module Name:  PlayerMove.vb
' Project:	    VBWPFMVVMPractice
' Copyright (c) Microsoft Corporation.
' 
' The PlayerMove class is a Model in the MVVM pattern, which represents a player's move.
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

Public Class PlayerMove
    Implements INotifyPropertyChanged

    Private _isPartOfWin As Boolean = False
    Private _moveNumber As Integer
    Private _playerName As String

    Public Event PropertyChanged As PropertyChangedEventHandler Implements INotifyPropertyChanged.PropertyChanged

    ''' <summary>
    ''' Get and set whether the move is part of all the moves of the winner.
    ''' </summary>
    Public Property IsPartOfWin As Boolean
        Get
            Return Me._isPartOfWin
        End Get
        Set(ByVal value As Boolean)
            If (Me._isPartOfWin <> value) Then
                Me._isPartOfWin = value
                Me.Notify("IsPartOfWin")
            End If
        End Set
    End Property

    ''' <summary>
    ''' Get and set the number of this move of the player.
    ''' </summary>
    Public Property MoveNumber As Integer
        Get
            Return Me._moveNumber
        End Get
        Set(ByVal value As Integer)
            If (Me._moveNumber <> value) Then
                Me._moveNumber = value
                Me.Notify("MoveNumber")
            End If
        End Set
    End Property

    ''' <summary>
    ''' Get and set the player's name.
    ''' </summary>
    Public Property PlayerName As String
        Get
            Return Me._playerName
        End Get
        Set(ByVal value As String)
            If (String.Compare(Me._playerName, value) <> 0) Then
                Me._playerName = value
                Me.Notify("PlayerName")
            End If
        End Set
    End Property

    Public Sub New(ByVal playerName As String, ByVal moveNumber As Integer)
        Me.PlayerName = playerName
        Me.MoveNumber = moveNumber
    End Sub

    Private Sub Notify(ByVal propName As String)
        RaiseEvent PropertyChanged(Me, New PropertyChangedEventArgs(propName))
    End Sub

End Class


