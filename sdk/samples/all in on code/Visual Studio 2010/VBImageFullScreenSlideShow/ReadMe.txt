=============================================================================
       Windows APPLICATION: VBImageFullScreenSlideShow Overview
=============================================================================

/////////////////////////////////////////////////////////////////////////////
Summary:

The sample demonstrates how to display image slideshow in a Windows Forms 
application.  It also shows how to enter the full screen mode to slide-show 
images. 


/////////////////////////////////////////////////////////////////////////////
Demo:

Step1. Build and run the sample project in Visual Studio 2010. 

Step2. Prepare some image files.  Click the "Open Folder..." button and 
       select the path which includes image files. 

Step3. Click "Previous" button and "Next" button to make image files 
       displayed in order.

Step4. Left-click the "Settings" button and select the internal between the 
       displayed image files for Timer control in order to display them 
       with a fixed interval time. Finally, left-click the "Start Slideshow"
	   button to make the image files displayed one by one.

Step5. Left-click the "Full Screen" button to display images in the full 
       screen mode.  Press the "ESC" key to leave the full screen mode.


/////////////////////////////////////////////////////////////////////////////
Implementation:

1. When user selects the root folder of image files, the sample enumerates 
   the image files in the folder using the stack-based iteration method 
   demonstrated in this MSDN article: 
   http://msdn.microsoft.com/en-us/library/bb513869.aspx
   The sample does not use 
        Directory.GetFiles(path, "*.*", SearchOption.AllDirectories)
   to enumerate the files because it will abort when the user does not have 
   access permissions for certain directories or files in the root folder.

       Public Shared Function GetFiles(ByVal path As String, ByVal searchPattern As String) As String()
        Dim patterns() As String = searchPattern.Split(";"c)
        Dim files As New List(Of String)()
        For Each filter As String In patterns
            ' Iterate through the directory tree and ignore the 
            ' DirectoryNotFoundException or UnauthorizedAccessException 
            ' exceptions. 
            ' http://msdn.microsoft.com/en-us/library/bb513869.aspx

            ' Data structure to hold names of subfolders to be
            ' examined for files.
            Dim dirs As New Stack(Of String)(20)

            If Not Directory.Exists(path) Then
                Throw New ArgumentException()
            End If
            dirs.Push(path)

            Do While dirs.Count > 0
                Dim currentDir As String = dirs.Pop()
                Dim subDirs() As String
                Try
                    subDirs = Directory.GetDirectories(currentDir)
                    ' An UnauthorizedAccessException exception will be thrown 
                    ' if we do not have discovery permission on a folder or 
                    ' file. It may or may not be acceptable to ignore the 
                    ' exception and continue enumerating the remaining files 
                    ' and folders. It is also possible (but unlikely) that a 
                    ' DirectoryNotFound exception will be raised. This will 
                    ' happen if currentDir has been deleted by another 
                    ' application or thread after our call to Directory.Exists. 
                    ' The choice of which exceptions to catch depends entirely 
                    ' on the specific task you are intending to perform and 
                    ' also on how much you know with certainty about the 
                    ' systems on which this code will run.
                Catch e As UnauthorizedAccessException
                    Continue Do
                Catch e As DirectoryNotFoundException
                    Continue Do
                End Try

                Try
                    files.AddRange(Directory.GetFiles(currentDir, filter))
                Catch e As UnauthorizedAccessException
                    Continue Do
                Catch e As DirectoryNotFoundException
                    Continue Do
                End Try

                ' Push the subdirectories onto the stack for traversal.
                ' This could also be done before handing the files.
                For Each str As String In subDirs
                    dirs.Push(str)
                Next str
            Loop
        Next filter

        Return files.ToArray()
    End Function

2. The sample displays the images in a PictureBox. 

    ''' <summary>
    ''' Show the image in the PictureBox.
    ''' </summary>
    Public Shared Sub ShowImage(ByVal path As String, ByVal pct As PictureBox)
        pct.ImageLocation = path
    End Sub

    ''' <summary>
    ''' Show the previous image.
    ''' </summary>
    Private Sub ShowPrevImage()
        Me.selected -= 1
        ShowImage(Me.imageFiles((Me.selected) Mod Me.imageFiles.Length), Me.pictureBox)
    End Sub

    ''' <summary>
    ''' Show the next image.
    ''' </summary>
    Private Sub ShowNextImage()
        Me.selected += 1
        ShowImage(Me.imageFiles((Me.selected) Mod Me.imageFiles.Length), Me.pictureBox)
    End Sub

   A timer is used to automatically slideshow the images.

    ''' <summary>
    ''' Show the next image at every regular intervals.
    ''' </summary>
    Private Sub timer_Tick(ByVal sender As Object, ByVal e As EventArgs) Handles timer.Tick
        ShowNextImage()
    End Sub

2. To slide-show images in the full-screen mode, the sample provides a helper 
   class 'FullScreen'.  FullScreen.cs contains two public methods: 
   
        EnterFullScreen - used to make a Windows Form display in the full screen.
        LeaveFullScreen - used to restore a Windows Form to its original state.

    ''' <summary>
    ''' Maximize the window to the full screen.
    ''' </summary>
    Public Sub EnterFullScreen(ByVal targetForm As Form)
        If Not IsFullScreen Then
            Save(targetForm) ' Save the original form state.

            targetForm.WindowState = FormWindowState.Maximized
            targetForm.FormBorderStyle = FormBorderStyle.None
            targetForm.TopMost = True
            targetForm.Bounds = Screen.GetBounds(targetForm)

            IsFullScreen = True
        End If
    End Sub
	
    ''' <summary>
    ''' Leave the full screen mode and restore the original window state.
    ''' </summary>
    Public Sub LeaveFullScreen(ByVal targetForm As Form)
        If IsFullScreen Then
            ' Restore the original Window state.
            targetForm.WindowState = winState
            targetForm.FormBorderStyle = brdStyle
            targetForm.TopMost = topMost
            targetForm.Bounds = bounds

            IsFullScreen = False
        End If
    End Sub


/////////////////////////////////////////////////////////////////////////////
References:

How to: Iterate Through a Directory Tree (C# Programming Guide)
http://msdn.microsoft.com/en-us/library/bb513869.aspx

Screen.GetBounds Method 
http://msdn.microsoft.com/en-us/library/system.windows.forms.screen.getbounds.aspx


/////////////////////////////////////////////////////////////////////////////
