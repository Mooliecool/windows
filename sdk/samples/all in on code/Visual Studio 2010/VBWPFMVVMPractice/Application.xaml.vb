'****************************** Module Header ******************************\
' Module Name:  Application.xaml.vb
' Project:      VBWPFMVVMPractice
' Copyright (c) Microsoft Corporation.
' 
' The VBWPFMVVMPractice demo demonstrates how to implement the MVVM patten in a 
' WPF application.
'  
' This source is subject to the Microsoft Public License.
' See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
' All other rights reserved.
' 
' THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND, 
' EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED 
' WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
'***************************************************************************/

Class Application

    Protected Overrides Sub OnStartup(ByVal e As StartupEventArgs)
        MyBase.OnStartup(e)

        'Create a TicTacToeViewModel object with dimension of 3 and start a new game.            
        Dim game As New TicTacToeViewModel(3)
        game.NewGame()

        'Create an instance of the MainWindow and set the TicTacToeViewModel instance 
        'as the data context of the MainWindow.     
        Dim win As New MainWindow
        win.DataContext = game

        'Set the attached GameoverBehavior.ReportResult property to true on the MainWindow.            
        win.SetValue(GameOverBehavior.ReportResultProperty, True)

        'Show the MainWindow. 
        win.Show()
    End Sub

End Class
