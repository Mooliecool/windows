================================================================================
       WINDOWS FORMS APPLICATION : CSWinFormTreeViewLoad Project Overview
       
                        TreeView Sample
                        
===============================================================================

/////////////////////////////////////////////////////////////////////////////
Use:

This example demonstrates how to load TreeView nodes based on DataTable's data.
   

/////////////////////////////////////////////////////////////////////////////
Code Logic:

1. Create a method called BuildTree which will create a TreeView
   from a specify DataTable.
   
2. When call the BuildTree method, pass a DataTable, a TreeView, 
   a Boolean value which will control TreeView if expand all or not,a column name 
   which value in this column will display on the TreeNode, a column name 
   which value in this column will identifiy the TreeNode , and a column name 
   which value in this column will identifiy the TreeNode's parent.
   
3. That will call the BuildTree method and create a TreeView 
   When the application loads the MainForm.


/////////////////////////////////////////////////////////////////////////////
References:

1. Windows Forms General FAQ.
   http://social.msdn.microsoft.com/Forums/en-US/winforms/thread/77a66f05-804e-4d58-8214-0c32d8f43191
   
2. Windows Forms TreeView control
   http://msdn.microsoft.com/en-us/library/ch6etkw4.aspx
   

/////////////////////////////////////////////////////////////////////////////
