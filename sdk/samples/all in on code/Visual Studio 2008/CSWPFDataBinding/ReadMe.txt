================================================================================
       WPF APPLICATION : CSWPFDataBinding Project Overview
       
                        WPF DataBinding Sample
                        
===============================================================================

/////////////////////////////////////////////////////////////////////////////
Use:

This projects demonstrates the basics of DataBinding in WPF. The sample provides
procedure code as well as XAML code, and they are achieving the same goal.
   

/////////////////////////////////////////////////////////////////////////////
Code Logic:

    1. Binding mode of TextBox "Name" to Twoway, and UpdateSourceTrigger
       is set to PropertyChanged, so the change will be
       reflected in the Label Name which is located in the second column.
       
    2. Binding mode of TextBox "Job" is set to OnewayToSource, so that we 
       can see the Job is empty when it is located. Once you enter in the 
       TextBox "Job", and switch focus, then the Label "Job" will appear.
       
    3. The custom validation rule is added to Binding's validation rule 
       collections, so if you enter a value does not fall certain range,
       there's UI change which indicates input contains error.
       
    4. Label Salary is given a Converter in its binding, so that we can see the 
       label actually displays "Total= No. $".
       
    5. Binding Mode of TextBox "Interest" is to to Oneway, so that when you 
       change the text of TextBox "Interest", lable "Interest" in the right 
       side will not be changed even the focus is lost. However, you can 
       set the value of Interest property in the code to change the target.
   
   
/////////////////////////////////////////////////////////////////////////////
References:

DataBinding Overview: http://msdn.microsoft.com/en-us/library/ms752347.aspx

/////////////////////////////////////////////////////////////////////////////
