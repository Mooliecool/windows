'****************************** Module Header ******************************\
' Module Name:  MainForm.vb
' Project:      VBWin7TaskbarThumbnail
' Copyright (c) Microsoft Corporation.
' 
' Thumbnail toolbars are another productivity feature that gives us the 
' ability to do more without actually switching to another application's 
' window.  A thumbnail toolbar is essentially a mini remote-control that is 
' displayed when you hover over the application's taskbar button, right 
' beneath the application’s thumbnail. 
' 
' VBWin7TaskbarThumbnail example demostrates how to set Taskbar Thumbnail
' previews, change Thumbnail previews order, set Thumbnail toolbars using 
' Taskbar related APIs in Windows API Code Pack.   
' 
' This MainForm can creates two Thumbnail previews based on previewPictureBox1
' and previewPicitureBox2, change the preivews order.  It also creates four
' Thumbnail toolbar buttons, First, Previous, Next, and Last to move the 
' selected image in the imageListView. 
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

#Region "Thumbnail Preview members"
    ' The max count of the Thumbnail preview of this application
    Const MAX_PREVIEW_COUNT As Integer = 2

    ' Index of the TabControl
    Const THUMBNAIL_PREVIEW As Integer = 0
    Const THUMBNAIL_TOOLBAR As Integer = 1

    ' The list holding the Thumbnail preview images
    Private tabbedThumbnailList As New List(Of TabbedThumbnail)( _
                                                MAX_PREVIEW_COUNT)


    ' Check the Windows version, then update the UI, otherwise
    ' exit the current process
    Private Sub MainForm_Load(ByVal sender As System.Object, ByVal e As  _
                              System.EventArgs) Handles MyBase.Load
        If Not TaskbarManager.IsPlatformSupported Then
            MessageBox.Show("Taskbar ProgressBar is not supported in your " & _
                            "operation system!" & vbNewLine & "Please launch " & _
                            "the application in Windows 7 or " & _
                            "Windows Server 2008 R2 systems.")

            '  Exit the current process
            Application.Exit()
        Else
            ' Update the buttons' enable status
            removeThumbnailButton.Enabled = False
            removeThumbnailButton2.Enabled = False
        End If
    End Sub


    ' Check whether the Thumbnail preivew has created
    Private Shared Function HasThumbnailPreview(ByVal control As Control)
        Return Not TaskbarManager.Instance.TabbedThumbnail. _
                    GetThumbnailPreview(control) Is Nothing
    End Function


    ' Make the image on preivewPictureBox1 as a Thumbnail preview
    Private Sub addThumbnailButton_Click(ByVal sender As System.Object, ByVal _
                                         e As System.EventArgs) Handles _
                                         addThumbnailButton.Click
        ' Check whether the Thumbnail preview has been created
        If Not HasThumbnailPreview(pictureBox1) Then

            ' Add the Thumbnail preview
            Me.AddThumbnailPreview(pictureBox1)

            ' Update the buttons' enable status
            addThumbnailButton.Enabled = False
            removeThumbnailButton.Enabled = True
        End If
    End Sub


    ' Remove the Thumbnail preview which is the image on preivewPictureBox1
    Private Sub removeThumbnailButton_Click(ByVal sender As System.Object, _
                                            ByVal e As System.EventArgs) _
                                            Handles removeThumbnailButton.Click
        ' Try to retrieve the Thumbnail preview
        Dim preview As TabbedThumbnail = TaskbarManager.Instance.TabbedThumbnail. _
                                                GetThumbnailPreview(pictureBox1)

        If Not preview Is Nothing Then

            ' Remove the Thumbnail preview
            Me.RemoveThumbnailPreview(preview)

            ' Update the buttons' enable status
            removeThumbnailButton.Enabled = False
            addThumbnailButton.Enabled = True
        End If
    End Sub


    ' Make the image on preivewPictureBox2 as a Thumbnail preview
    Private Sub addThumbnailButton2_Click(ByVal sender As System.Object, ByVal _
                                          e As System.EventArgs) Handles _
                                          addThumbnailButton2.Click
        ' Check whether the Thumbnail preview has been created
        If Not HasThumbnailPreview(pictureBox2) Then

            ' Add the Thumbnail preview
            Me.AddThumbnailPreview(pictureBox2)

            ' Update the buttons' enable status
            addThumbnailButton2.Enabled = False
            removeThumbnailButton2.Enabled = True
        End If
    End Sub


    ' Remove the Thumbnail preview which is the image on preivewPictureBox1
    Private Sub removeThumbnailButton2_Click(ByVal sender As System.Object, _
                                             ByVal e As System.EventArgs) _
                                             Handles removeThumbnailButton2.Click
        ' Try to retrieve the Thumbnail preview
        Dim preview As TabbedThumbnail = TaskbarManager.Instance.TabbedThumbnail. _
                                                GetThumbnailPreview(pictureBox2)
        If Not preview Is Nothing Then

            ' Remove the Thumbnail preview
            Me.RemoveThumbnailPreview(preview)

            ' Update the buttons' enable status
            removeThumbnailButton2.Enabled = False
            addThumbnailButton2.Enabled = True
        End If
    End Sub


    ' Change the Thumbnail previews order
    Private Sub changePreviewOrderButton_Click(ByVal sender As System.Object, _
                                               ByVal e As System.EventArgs) _
                                               Handles changePreviewOrderButton.Click
        ' Check if the Thumbnail preview 
        If tabbedThumbnailList.Count = MAX_PREVIEW_COUNT Then

            ' Change the Thumbnail preview order
            TaskbarManager.Instance.TabbedThumbnail.SetTabOrder(tabbedThumbnailList(1), _
                                                                tabbedThumbnailList(0))
            ' Reverse the Thumbnail preview image list
            tabbedThumbnailList.Reverse()
        End If
    End Sub


    ' Change the Thumbnail effect between Thumbnail preview and 
    ' Thumbnail toolbar based on TabControl's selected index.
    Private Sub thumbnailTabControl_SelectedIndexChanged(ByVal sender As System.Object, _
                                                         ByVal e As System.EventArgs) _
                                                         Handles thumbnailTabControl. _
                                                         SelectedIndexChanged
        ' Check if it is Thumbnail toolbar index
        If thumbnailTabControl.SelectedIndex = THUMBNAIL_TOOLBAR Then

            ' Clear the all the Thumbnail previews first

            ' Try to retrieve the Thumbnail preview of previewPictureBox1
            Dim preview As TabbedThumbnail = TaskbarManager.Instance. _
                                    TabbedThumbnail.GetThumbnailPreview(pictureBox1)

            If Not preview Is Nothing Then
                ' Remove the Thumbnail preview
                Me.RemoveThumbnailPreview(preview)
            End If

            ' Try to retrieve the Thumbnail preview of previewPictureBox2
            preview = TaskbarManager.Instance.TabbedThumbnail. _
                                    GetThumbnailPreview(pictureBox2)

            If Not preview Is Nothing Then
                ' Remove the Thumbnail preview
                Me.RemoveThumbnailPreview(preview)
            End If


            '  Then update the Thumbnail toolbar page effect

            ' Select and focus the first image in the image list
            imageListView.Items(0).Selected = True
            imageListView.Focus()

            ' Check if the Thumbnail buttons have been created
            If Not toolBarButtonCreated Then
                ' Create the Thumbnail toolbar buttons
                CreateToolbarButtons()

                ' Set the flag
                toolBarButtonCreated = True
            Else
                ' Make all the Thumbnail toolbar buttons visible
                ChangeVisibility(buttonPrevious, True)
                ChangeVisibility(buttonFirst, True)
                ChangeVisibility(buttonLast, True)
                ChangeVisibility(buttonNext, True)
            End If
            ' Check if it is Thumbnail preview index
        ElseIf thumbnailTabControl.SelectedIndex = THUMBNAIL_PREVIEW Then
            ' Make all the Thumbnail toolbar buttons invisible
            ChangeVisibility(buttonPrevious, False)
            ChangeVisibility(buttonFirst, False)
            ChangeVisibility(buttonLast, False)
            ChangeVisibility(buttonNext, False)

            ' Update the buttons' enable status
            addThumbnailButton.Enabled = True
            removeThumbnailButton.Enabled = False
            addThumbnailButton2.Enabled = True
            removeThumbnailButton2.Enabled = False
        End If
    End Sub


    ' Make the given Control as the Thumbnail preview
    Private Sub AddThumbnailPreview(ByVal control As Control)
        ' Make a new Thumbnail preview object
        Dim preview As New TabbedThumbnail(Me.Handle, control)

        ' Make the given Control as the Thumbnail preview
        TaskbarManager.Instance.TabbedThumbnail.AddThumbnailPreview(preview)

        ' Set the newly created Thumbnail preview as active
        TaskbarManager.Instance.TabbedThumbnail.SetActiveTab(preview)

        ' Add the Thumbnail preview into the list
        tabbedThumbnailList.Add(preview)
    End Sub


    ' Remove the passed-in TabbedThumbnail preview object
    Private Sub RemoveThumbnailPreview(ByVal preview As TabbedThumbnail)
        ' Remove the Thumbnail preview 
        TaskbarManager.Instance.TabbedThumbnail.RemoveThumbnailPreview(preview)

        ' Remove the Thumbnail preivew from the list
        tabbedThumbnailList.Remove(preview)
    End Sub
#End Region


#Region "Thumbnail Toolbar Buttom members"
    ' Thumbnail toolbar button enum
    Public Enum ButtonTask
        FirstButton
        LastButton
        NextButton
        PreviousButton
        All
    End Enum

    ' The flag to record whether the Thumbnail toolbar buttons are created
    Private toolBarButtonCreated As Boolean = False

    ' The Thumbnail toolbar buttons instances
    Private buttonPrevious As ThumbnailToolbarButton
    Private buttonNext As ThumbnailToolbarButton
    Private buttonFirst As ThumbnailToolbarButton
    Private buttonLast As ThumbnailToolbarButton


    ' Create the Thumbnail toolbar buttons instance
    Private Sub CreateToolbarButtons()

        ' Initialize the First Thumbnail toolbar button
        buttonFirst = New ThumbnailToolbarButton(My.Resources.first, _
                                                 "First Image")
        ' Update the button's enabled status and register Click event handler
        buttonFirst.Enabled = False
        AddHandler buttonFirst.Click, AddressOf buttonFirst_Click

        ' Initialize the Previous Thumbnail toolbar button
        buttonPrevious = New ThumbnailToolbarButton(My.Resources.prevArrow, _
                                                    "Previous Image")

        ' Update the button's enabled status and register Click event handler
        buttonPrevious.Enabled = False
        AddHandler buttonPrevious.Click, AddressOf buttonPrevious_Click

        ' Initialize the Next Thumbnail toolbar button
        buttonNext = New ThumbnailToolbarButton(My.Resources.nextArrow, _
                                                "Next Image")

        ' Register the Click event handler
        AddHandler buttonNext.Click, AddressOf buttonNext_Click

        ' Initialize the Last Thumbnail toolbar button
        buttonLast = New ThumbnailToolbarButton(My.Resources.last, _
                                                "Last Image")

        ' Register the Click event handler
        AddHandler buttonLast.Click, AddressOf buttonLast_Click

        ' Creates the four Thumbnail toolbar buttons
        TaskbarManager.Instance.ThumbnailToolbars.AddButtons(Me.Handle, _
                                                             buttonFirst, _
                                                             buttonPrevious, _
                                                             buttonNext, _
                                                             buttonLast)
    End Sub


    ' Update the visible and enable status of the Thumbnail buttons
    Private Sub UpdateButtons()
        ChangeVisibility(buttonPrevious, True, ButtonTask.PreviousButton)
        ChangeVisibility(buttonFirst, True, ButtonTask.FirstButton)
        ChangeVisibility(buttonLast, True, ButtonTask.LastButton)
        ChangeVisibility(buttonNext, True, ButtonTask.NextButton)
    End Sub


    ' Change the certain Thumbnail button's visible and enable status
    ' based on the current selected index in the imageListView
    Private Sub ChangeVisibility(ByVal btn As ThumbnailToolbarButton, _
                                 ByVal show As Boolean, _
                                 ByVal task As ButtonTask)
        If Not btn Is Nothing Then

            ' Update the button's UI status
            btn.Visible = show
            btn.IsInteractive = show

            ' Change the certain button enable status based on the 
            ' selected index of the imageListView
            Select Case task

                ' The First and Previous Thumbnail toolbar button 
                ' is enabled if the first image in the 
                ' imageListView is not selected
                Case ButtonTask.FirstButton
                    btn.Enabled = Not imageListView.Items(0).Selected
                Case ButtonTask.PreviousButton
                    btn.Enabled = Not imageListView.Items(0).Selected
                    ' The Last and Next Thumbnail toolbar button 
                    ' is enabled if the last image in the 
                    ' imageListView is not selected
                Case ButtonTask.LastButton
                    btn.Enabled = Not imageListView.Items(imageListView. _
                                                          Items.Count - 1). _
                                                          Selected
                Case ButtonTask.NextButton
                    btn.Enabled = Not imageListView.Items(imageListView. _
                                                          Items.Count - 1). _
                                                          Selected
            End Select
        End If
    End Sub


    ' Only update the Thumbnal toolbar button visible status
    Private Sub ChangeVisibility(ByVal btn As ThumbnailToolbarButton, _
                                 ByVal show As Boolean)
        ChangeVisibility(btn, show, ButtonTask.All)
    End Sub


    ' Update the selected index when the certain Thumbnail toolbar 
    ' button is clicked
    Private Sub ShowList(ByVal newIndex As Integer)

        ' Check whether the selected index is valid
        If newIndex < 0 Or newIndex > imageListView.Items.Count - 1 Then
            Return
        End If

        ' Update the selected index and focus the imageListView
        imageListView.Items(newIndex).Selected = True
        imageListView.Focus()
    End Sub


    ' Select the previous item in the imageListView
    Private Sub buttonPrevious_Click(ByVal sender As System.Object, _
                                     ByVal e As System.EventArgs)
        ShowList(imageListView.SelectedIndices(0) - 1)
    End Sub


    ' Select the next item in the imageListView
    Private Sub buttonNext_Click(ByVal sender As System.Object, _
                                 ByVal e As System.EventArgs)
        ShowList(imageListView.SelectedIndices(0) + 1)
    End Sub


    ' Select the first item in the imageListView
    Private Sub buttonFirst_Click(ByVal sender As System.Object, _
                                  ByVal e As System.EventArgs)
        ShowList(0)
    End Sub


    ' Select the last item in the imageListView
    Private Sub buttonLast_Click(ByVal sender As System.Object, _
                                 ByVal e As System.EventArgs)
        ShowList(imageListView.Items.Count - 1)
    End Sub


    ' Show different image in the imagePictureBox based on the 
    ' selected index of the imageListView
    Private Sub imageListView_SelectedIndexChanged(ByVal sender As  _
                                                   System.Object, _
                                                   ByVal e As System.EventArgs) _
                                                   Handles imageListView. _
                                                   SelectedIndexChanged
        If imageListView.SelectedItems.Count > 0 Then
            Dim image As Image

            ' Retrieve the image from the Resource according to the 
            ' selected index of the imageListView
            Select Case imageListView.SelectedItems(0).ImageIndex
                Case 0
                    image = My.Resources.C
                Case 1
                    image = My.Resources.O
                Case 2
                    image = My.Resources.D
                Case Else
                    image = My.Resources.E
            End Select

            ' Update the image in the imagePictureBox
            imagePictureBox.Image = image

            ' Update the visible and enable status of the Thumbnail
            ' toolbar buttons
            UpdateButtons()
        End If
    End Sub
#End Region
End Class
