'****************************** Module Header ******************************'
' Module Name:  MainPage.xaml.vb
' Project:      VBSL3OOB
' Copyright (c) Microsoft Corporation.
' 
' This example demonstrates how to work with OOB using VB.
' 
' This source is subject to the Microsoft Public License.
' See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
' All other rights reserved.
' 
' History:
' * 10/15/2009 16:21 Yilun Luo Created
'***************************************************************************'

Imports System.Net.NetworkInformation

Partial Public Class MainPage
	Inherits UserControl

	Public Sub New()
		InitializeComponent()
		AddHandler Application.Current.CheckAndDownloadUpdateCompleted, New CheckAndDownloadUpdateCompletedEventHandler(AddressOf Me.Current_CheckAndDownloadUpdateCompleted)
		AddHandler NetworkChange.NetworkAddressChanged, New NetworkAddressChangedEventHandler(AddressOf Me.NetworkChange_NetworkAddressChanged)
	End Sub

	''' <summary>
	''' Check if an update is available.
	''' </summary>
	''' <param name="sender"></param>
	''' <param name="e"></param>
	''' <remarks></remarks>
	Private Sub CheckUpdateButton_Click(ByVal sender As Object, ByVal e As RoutedEventArgs)
		Application.Current.CheckAndDownloadUpdateAsync()
	End Sub

	Private Sub Current_CheckAndDownloadUpdateCompleted(ByVal sender As Object, ByVal e As CheckAndDownloadUpdateCompletedEventArgs)
		If e.UpdateAvailable Then
			MessageBox.Show("You have to upgrade this application to the latest version in order to use it. Restart this application to upgrade automatically!")
		End If
	End Sub

	''' <summary>
	''' Install OOB with code.
	''' </summary>
	''' <param name="sender"></param>
	''' <param name="e"></param>
	''' <remarks></remarks>
	Private Sub InstallButton_Click(ByVal sender As Object, ByVal e As RoutedEventArgs)
		If (Application.Current.InstallState = InstallState.NotInstalled) Then
			Application.Current.Install()
		ElseIf (Application.Current.InstallState = InstallState.Installed) Then
			MessageBox.Show("Application already installed. You cannot remove the OOB via code!")
		End If
	End Sub

	''' <summary>
	''' Detect the network connection changing.
	''' </summary>
	''' <param name="sender"></param>
	''' <param name="e"></param>
	''' <remarks></remarks>
	Private Sub NetworkChange_NetworkAddressChanged(ByVal sender As Object, ByVal e As EventArgs)
		If Not NetworkInterface.GetIsNetworkAvailable Then
			Me.informationTextBlock.Text = "Your network connection has been lost!"
		Else
			Me.informationTextBlock.Text = "Your network connection has been restored!"
		End If
	End Sub
End Class