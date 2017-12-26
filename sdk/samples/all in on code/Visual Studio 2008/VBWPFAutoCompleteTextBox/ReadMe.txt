================================================================================
       WPF APPLICATION : VBWPFAutoCompleteTextBox Project Overview
       
                        AutoCompleteTextBox Sample
                        
===============================================================================

/////////////////////////////////////////////////////////////////////////////
Use:

Provide an easy implementation of AutoCompleteTextBox in WPF
   

/////////////////////////////////////////////////////////////////////////////
Code Logic:

   1. Retemplate ComboBox to make it looks like TextBox.
   2. Extend ComboBoxItem so that we can hightlight the already entered part
      in the dropdown list.
   3. Get reference to the TextBox part of the ComboBox, and hook up 
      TextBox.TextChanged event.
   4. In the TextBox.TextChanged event handler, we filter the underlying 
      datasource and create new list source with our customized ComboBox
      Items.
   
 
 
   
/////////////////////////////////////////////////////////////////////////////
References:


/////////////////////////////////////////////////////////////////////////////
Demo

Step1. Build the sample project in Visual Studio 2008.

Step2. Start Without Debugging, and the mian window of the project will show.

Step3. Input a string (e.g. "app") in the text box in the top of the window, and it
will list all the items which starts with the input string.
/////////////////////////////////////////////////////////////////////////////