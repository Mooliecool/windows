=============================================================================
       WPF APPLICATION: CSWPFListBoxValidation Overview
=============================================================================

/////////////////////////////////////////////////////////////////////////////
Summary:  

The sample demonstrates how to add validation to a ListBox, overriding the control
to contain a ValidationListener property, which can be bound to provide validation
using built in validation UI features in WPF.


/////////////////////////////////////////////////////////////////////////////
Demo:

To run the sample, simply open in Visual Studio 2010 and run it. 
It contains a ListBox, which was overridden to include a property called 
ValidationListener, which is used to bind the ListBox to a property used for 
validation, and a validation rule.  The validation property is simply a text 
field added to the Window, in which error text is written if the ListBox is 
found having no selected items.  Of course the rule could be more complex 
as necessary, but this demonstrates the approach.

When the form first displays, no items are selected, so it is not valid.  
Validation UI displays a red outline around it, and another label control 
is also validated using the same criteria, and is outlined in red as well, 
and displays the validation error message.

Selecting any items validates the control.  Removing all selections will 
again invalidate the control.


/////////////////////////////////////////////////////////////////////////////
Implementation:

The built in validation in WPF does not have a default means of performing validation 
on collection-valued sources, and was designed for scalar-valued properties.  
To put another way, validation listens to PropertyChanged events, but not to 
CollectionChanged events.  In order to achieve this, a scalar-valued property is 
required for validation to listen to.  This implementation simply creates that 
property (the Validation property defined in the Window), and then detects that 
in the ValidationRule for the ValidationListener property of the ValidatingListBox, 
to which it is bound.  Whenever that property changes, WPF detects it because the 
rule is marked as ValidatesOnTargetUpdated, and runs the defined validation logic.


/////////////////////////////////////////////////////////////////////////////
References:  

How to: Implement Binding Validation
http://msdn.microsoft.com/en-us/library/ms753962.aspx

Hook Up and Display Validation in WPF 
http://msdn.microsoft.com/en-us/vbasic/cc788743.aspx

/////////////////////////////////////////////////////////////////////////////