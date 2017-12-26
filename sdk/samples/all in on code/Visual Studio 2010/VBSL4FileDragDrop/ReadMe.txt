========================================================================
    SILVERLIGHT APPLICATION : VBSL4FileDragDrop Project Overview
========================================================================

/////////////////////////////////////////////////////////////////////////////
Use:

Drag&Drop is the new supported feature in Silverlight4. In this sample, we demonstrate
how to implement visual element's drag&drop function and how to access local files
by using file drag&drop feature.


/////////////////////////////////////////////////////////////////////////////
Demo:

To test this sample:
1. Open VBSL4FileDragDrop solution and rebuild. (Due to an known issue, the drag&drop function are not allowed when running Silverlight app in Admin
mode IE browser. Therefore, user need avoid running this Sample in VisualStudio or IE with
Admin account)
2. Press Ctrl+F5 to open the test page.
3. In opened page, you may find a Silverlight application.
	a. Try drag any image files from your windows explorer to Silverlight application
	blue region. You may find that the image files will be loaded and displayed in
	Silverlight application. (Note that: Silverlight only support .jpg,.png image format)
	b. Try drag and drop the image icon.


/////////////////////////////////////////////////////////////////////////////
Prerequisites:

Silverlight 4 Tools for Visual Studio 2010
http://www.silverlight.net/getstarted/

Silverilght 4 runtime
http://www.silverlight.net/getstarted/


/////////////////////////////////////////////////////////////////////////////
Code Logic:

1. How to drag local files to Silverlight application?
	First, enable the file drop function on elements by setting UIElement.AllowDrop to true.
	Then, handle UIElement.Drop event. To access the draged files, we may use the code below:

	Private Sub LayoutRoot_Drop(ByVal sender As Object, ByVal e As DragEventArgs)
        If e.Data IsNot Nothing Then
            ' get fileinfos.
            Dim files = TryCast(e.Data.GetData(DataFormats.FileDrop), FileInfo())
			For Each file In files
				'access file here.
			Next
		End If
	End Sub

2. How to implement UI element drag and drop function?
	First, register UIElement.MouseLeftButtonDown event, in eventhandler, set
	"isdrag" flag to true, and store the mouse position relative to the drag 
	target element, and then capture mouse by UIElement.CaptureMouse() method.

        AddHandler element.MouseLeftButtonDown, Function(s, e)
                                                    isdrag = True
                                                    element.CaptureMouse()
                                                    relativepos = e.GetPosition(element)
                                                End Function

	Second, register UIElement.MouseMove event, in eventhandler, check "isdrag"
	flag value, if is true, set element's postion to 
	[mouse current position relative to element's container] - [stored mouse position relative to element]

        AddHandler element.MouseMove, Function(s, e)
                                          If isdrag Then
                                              Dim pos = e.GetPosition(LayoutRoot)
                                              Canvas.SetLeft(element, pos.X - relativepos.X)
                                              Canvas.SetTop(element, pos.Y - relativepos.Y)
                                          End If
                                      End Function

	Third, register UIElement.MouseLeftButtonUp event, in eventhandler, set
	"isdrag" to false, and release mouse capture.

	   AddHandler element.MouseLeftButtonUp, Function(s, e)
                                                  element.ReleaseMouseCapture()
                                                  isdrag = False
                                              End Function

/////////////////////////////////////////////////////////////////////////////
References:

UIElement.Drop Event
http://msdn.microsoft.com/en-us/library/system.windows.uielement.drop(VS.95).aspx

/////////////////////////////////////////////////////////////////////////////