'****************************** Module Header ******************************\
' Module Name:  MainForm.vb
' Project:      VBWin7TaskbarAppID
' Copyright (c) Microsoft Corporation.
' 
' Application User Model IDs (AppUserModelIDs) are used extensively by the 
' taskbar in Windows 7 and later systems to associate processes, files, and 
' windows with a particular application. In some cases, it is sufficient to 
' rely on the internal AppUserModelID assigned to a process by the system. 
' However, an application that owns multiple processes or an application that 
' is running in a host process might need to explicitly identify itself so 
' that it can group its otherwise disparate windows under a single taskbar 
' button and control the contents of that application's Jump List.
' 
' VBWin7TaskbarAppID example demostrates how to set process level Application
' User Model IDs (AppUserModelIDs or AppIDs) and modify the AppIDs for a 
' specific window using Taskbar related APIs in Windows API Code Pack.
' 
' This MainForm sets a process level AppID for the current process.  It can 
' create some SubForms and set specific AppIDs for these SubForms, so the
' SubForms will be in different group of the MainForm's Taskbar button.  
' It also can modify the SubForms AppIDs back to the MainForm's AppID, so 
' the SubForms Taskbar buttons will be in the same group of theMainForm.
' 
' This source is subject to the Microsoft Public License.
' See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
' All other rights reserved.
' 
' THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND, 
' EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED 
' WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
'***************************************************************************/

#Region "Imports directive"
Imports Microsoft.WindowsAPICodePack.Taskbar
#End Region

Public Class MainForm

    ' The AppID for the current process
    Const MainFormAppID As String = "All-In-One Code Framework.VBWin7AppID"

    ' The AppID for the SubForms
    Const SubFormAppID As String = MainFormAppID & ".SubForm"

    ' A generic List holding the SubForm references
    Private _subFormList As List(Of SubForm)
    Public Property SubFormList() As List(Of SubForm)
        Get
            Return _subFormList
        End Get
        Set(ByVal value As List(Of SubForm))
            _subFormList = value
        End Set
    End Property


    ' Check the Windows version, then set the current process's AppID or
    ' exit the current process
    Private Sub MainForm_Load(ByVal sender As System.Object, ByVal e As  _
                              System.EventArgs) Handles MyBase.Load

        ' Check whether the current system is Windows 7 or 
        ' Windows Server 2008 R2
        If TaskbarManager.IsPlatformSupported Then

            ' Set the AppID for the current process, it calls the 
            ' Windows API SetCurrentProcessExplicitAppUserModelID
            ' TaskbarManager.Instance represents an instance of the
            ' Windows Taskbar
            TaskbarManager.Instance.ApplicationId = MainFormAppID

            ' Set the Title of the MainForm to the AppID
            Me.Text = TaskbarManager.Instance.ApplicationId

            '  Initialize the list holding the SubForms references
            SubFormList = New List(Of SubForm)()
        Else
            MessageBox.Show("Taskbar Application ID is not supported in" & _
                            " your operation system!" & vbNewLine _
                            & "Please launch the application in Windows 7" & _
                            " or Windows Server 2008 R2 systems.")

            ' Exit the current process
            Application.Exit()
        End If
    End Sub


    ' Create a SubForm and set a new AppID for it
    Private Sub openSubFormButton_Click(ByVal sender As System.Object, ByVal e _
                                        As System.EventArgs) Handles _
                                        openSubFormButton.Click
        ' Create a new SubForm
        Dim subForm As New SubForm()

        ' Set the SubForm's AppID, it calls Windows API
        ' SHGetPropertyStoreForWindow
        TaskbarManager.Instance.SetApplicationIdForSpecificWindow( _
                                                subForm.Handle, SubFormAppID)

        ' Set the SubForm Title to the new AppID
        subForm.Text = SubFormAppID

        ' Display the SubForm
        subForm.Show()

        ' Add this SubForm's reference into list
        SubFormList.Add(subForm)

        ' Update the buttons' enable status
        resetSubFormAppIDButton.Enabled = True
        setSubFormAppIDButton.Enabled = False
    End Sub


    '  Set all the SubForms's AppIDs to the SubFormAppID
    Private Sub setSubFormAppIDButton_Click(ByVal sender As System.Object, ByVal e _
                                            As System.EventArgs) Handles _
                                            setSubFormAppIDButton.Click
        ' Set all the SubForms's AppIDs and update the button enable status
        If SetAllSubFormAppIDs(SubFormAppID) Then
            resetSubFormAppIDButton.Enabled = True
        End If
        setSubFormAppIDButton.Enabled = False
    End Sub


    ' Reset all the SubForm's AppIDs to the MainFormAppID
    Private Sub resetSubFormAppIDButton_Click(ByVal sender As System.Object, ByVal e _
                                              As System.EventArgs) Handles _
                                              resetSubFormAppIDButton.Click
        ' Set all the SubForms's AppIDs and update the button enable status
        If SetAllSubFormAppIDs(MainFormAppID) Then
            setSubFormAppIDButton.Enabled = True
        End If
        resetSubFormAppIDButton.Enabled = False
    End Sub


    ''' <summary>
    ''' Set all the SubForms' AppID
    ''' </summary>
    ''' <param name="AppID">The AppID to be set</param>
    ''' <returns>Whether the operation successes</returns>
    Private Function SetAllSubFormAppIDs(ByVal AppID As String) As Boolean

        ' Check whether there are any SubForms exist
        If SubFormList.Count > 0 Then
            For Each subForm In SubFormList

                ' Set each SubForm's AppID, it calls Windows API
                ' SHGetPropertyStoreForWindow
                TaskbarManager.Instance.SetApplicationIdForSpecificWindow( _
                                                    subForm.Handle, AppID)

                ' Set the SubForm Title to the new AppID 
                subForm.Text = AppID
            Next
            Return True
        Else
            MessageBox.Show("No SubForms now!")
            Return False
        End If
    End Function
End Class
