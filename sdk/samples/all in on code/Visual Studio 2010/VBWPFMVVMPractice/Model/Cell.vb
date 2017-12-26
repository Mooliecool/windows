'****************************** Module Header ******************************\
' Module Name:  Cell.vb
' Project:	    VBWPFMVVMPractice
' Copyright (c) Microsoft Corporation.
' 
' The Cell class is a Model in the MVVM pattern, which represents a cell in the TicTacToe 
' game grid.
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

Public Class Cell
    Implements INotifyPropertyChanged

    Private _cellNumber As Integer
    Private _move As PlayerMove

    Public Event PropertyChanged As PropertyChangedEventHandler Implements INotifyPropertyChanged.PropertyChanged

    ''' <summary>
    ''' Return the number of the cell.
    ''' </summary>
    Public ReadOnly Property CellNumber As Integer
        Get
            Return Me._cellNumber
        End Get
    End Property

    ''' <summary>
    ''' Represents a player's move on the cell.
    ''' </summary>
    Public Property Move As PlayerMove
        Get
            Return Me._move
        End Get
        Set(ByVal value As PlayerMove)
            If (Not Me._move Is value) Then
                Me._move = value
                Me.Notify("Move")
            End If
        End Set
    End Property

    Public Sub New(ByVal cellNumber As Integer)
        Me._cellNumber = cellNumber
    End Sub

    Private Sub Notify(ByVal propName As String)
        RaiseEvent PropertyChanged(Me, New PropertyChangedEventArgs(propName))
    End Sub

End Class


