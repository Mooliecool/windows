================================================================================
       WPF Drag&Drop Project Overview                        
===============================================================================

/////////////////////////////////////////////////////////////////////////////
Use:

The sample demonstrates how to implement drag&drop operation in a TabControl and a Grid control.
   

/////////////////////////////////////////////////////////////////////////////
Demo:

Step1. Build the sample project in Visual Studio 2008.

Step2. Press Mouse left button down on a TabItem in the TabControl and drag to another TabItem in the TabControl. 

Step3. Release Mouse left button and you'll see the dragged TabItem is switched to the index of the target TabItem 
that contains the dropped point.

Step4. Press Mouse left button down on the Button in the Grid control and drag to another cell in the Grid control.

Step5. Release Mouse left button and you'll see the Button is moved to the new cell in the Grid control.

/////////////////////////////////////////////////////////////////////////////
Code Logic:


Handle the MouseMove event on the TabItem. In the MouseMove event, if the Mouse left button is pressed and the 
drag&drop operation hasn't started, begin a new drag&drop operation by calling DragDrop.DoDragDrop method.


Handle the DragEnter and Drop events on the TabControl. In the Drop event handler, determine the index of the target 
TabItem that contains the dropped point and move the dragged TabItem to this new index.

Handle the PreviewMouseLeftButtonDown event on the Grid. In the PreviewMouseLeftButtonDown event, determine if 
a child element is clicked. If so, write down the clicked element and set a variable IsMouseDown to true to indicate
that mouse button is pressed. 

Handle the PreviewMouseMove event on the Grid. In this event, if the IsMouseDown variable is true and the drag&drop 
operation hasn't started, begin a new drag&drop operation.

Handle the Drop event on the Grid. In this event, check which cell contains the dropped point. If the cell is found, set the 
Grid.Row and Grid.Column attached property on the dragged element to the new cell's address.

/////////////////////////////////////////////////////////////////////////////


