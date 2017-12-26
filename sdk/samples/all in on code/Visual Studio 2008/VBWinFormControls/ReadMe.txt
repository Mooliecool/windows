================================================================================
       WINDOWS FORMS APPLICATION : VBWinFormControls Project Overview
       
                        Control Customization Sample
                        
===============================================================================

/////////////////////////////////////////////////////////////////////////////
Use:

The Control Customization sample demonstrates how to customize the Windows Forms 
controls.

In this sample, there're 4 examples:
 
1. Multiple Column ComboBox.
   Demonstrates how to display multiple columns of data in the dropdown of a ComboBox.
2. ListBox Items With Different ToolTips.
   Demonstrates how to display different tooltips on each items of the ListBox.
3. Numeric-only TextBox.
   Demonstrates how to make a TextBox only accepts numbers.
4. A Round Button.
   Demonstrates how to create a Button with irregular shape.
   

/////////////////////////////////////////////////////////////////////////////
Code Logic:

1. Example 1: "Multiple Column ComboBox".

   1). Create a DataTable with several columns and rows of data;
   2). Data bind the ComboBox control to the DataTable;
   3). Enable the owner draw on ComboBox by setting the DrawMode property to 
       DrawMode.OwnerDrawFixed;
   4). Handle the DrawItem event on the ComboBox;
   5). In the DrawItem event handler, compute the bounds for each column and 
       draw corresponding value for each column on its bounds.

2. Example 2: "ListBox Items With Different ToolTips".

   1). Add some items to the ListBox control;
   2). Handle the MouseMove event on the ListBox control;
   3). Call the ListBox.IndexFromPoint method to retrieve the item index at 
       where the mouse hovers;
   4). If the mouse is over the items, call ToolTip.SetToolTip method to display
       a tooltip for the individual item.
   
3. Example 3: "Numeric-only TextBox".

   The make a TextBox accepts only numbers, we can handle the TextBox.KeyPress 
   event, in the event handler use char.IsNumber method to filter the input keys.
   
4. Example 4: "A Round Button".

   The key point of creating a round button is changing its Region property.
   

/////////////////////////////////////////////////////////////////////////////
References:

1. Windows Forms General FAQ.
   http://social.msdn.microsoft.com/Forums/en-US/winforms/thread/77a66f05-804e-4d58-8214-0c32d8f43191

2. Shaped Windows Forms and Controls in Visual Studio .NET
   http://msdn.microsoft.com/en-us/library/aa289517.aspx

3. Round Button in C#
   http://www.codeproject.com/KB/buttons/RoundButton_csharp.aspx


/////////////////////////////////////////////////////////////////////////////
