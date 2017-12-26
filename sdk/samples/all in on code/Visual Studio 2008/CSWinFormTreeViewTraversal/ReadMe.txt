================================================================================
       WINDOWS FORMS APPLICATION : CSWinFormTreeViewTraversal Project Overview
       
                        TreeView Sample
                        
===============================================================================

/////////////////////////////////////////////////////////////////////////////
Use:

This example demonstrates how to perform TreeView nodes travel and find
a special node
   

/////////////////////////////////////////////////////////////////////////////
Code Logic:

1. Create a method called FindNode which will travel through all nodes 
   and find a special one
   
2. When call the FindNode method, pass TreeView that is going to be looped through.Pass
   the text of the TreeNode which you are going to find. Pass the MathWholdWord option indicate
   whether you are going to match the whold word.
   
3. When you pass empty string as second parameter and false as the third, it list all nodes
   of the TreeView control
   
4. After you get the result TreeNode List, you can update the UI with these data


/////////////////////////////////////////////////////////////////////////////
References:

1. Windows Forms General FAQ.
   http://social.msdn.microsoft.com/Forums/en-US/winforms/thread/77a66f05-804e-4d58-8214-0c32d8f43191
   
2. Windows Forms TreeView control
   http://msdn.microsoft.com/en-us/library/ch6etkw4.aspx
   

/////////////////////////////////////////////////////////////////////////////
