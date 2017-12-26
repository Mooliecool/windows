================================================================================
       WINDOWS FORMS APPLICATION : VBWinFormDragAndDrop Project Overview
       
                        Drag-and-Drop Sample
                        
===============================================================================

/////////////////////////////////////////////////////////////////////////////
Use:

This example demonstrates how to perform drag-and-drop operations in a 
Windows Forms Application.
   

/////////////////////////////////////////////////////////////////////////////
Code Logic:

1. Enable dropping on the destination control(it's TreeView in this example)
   by setting AllowDrop property to true.
   
2. Handle the MouseDown event on the source control(here is ListBox) to start 
   the drag operation. And call the DoDragDrop method to enable data to be 
   collected when dragging begins.
   
3. Handle the DragEnter event on the destination control to set the effect 
   that will happen when the drop occurs.
   
4. Handle the DragDrop event on the destination control to retrieve the data 
   dragged from the source control. 


/////////////////////////////////////////////////////////////////////////////
References:

1. Windows Forms General FAQ.
   http://social.msdn.microsoft.com/Forums/en-US/winforms/thread/77a66f05-804e-4d58-8214-0c32d8f43191
   
2. Performing Drag-and-Drop Operations in Windows Forms
   http://msdn.microsoft.com/en-us/library/aa984430(VS.71).aspx
   

/////////////////////////////////////////////////////////////////////////////
