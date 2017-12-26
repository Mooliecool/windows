========================================================================
    Web APPLICATION : CSASPNETPageValidation Project Overview
========================================================================

/////////////////////////////////////////////////////////////////////////////
Use:

The sample demonstrates how to use ASP.NET validation control. 
There are two ways to use ASP.NET validation controls: Server-side or 
Client-side. The client side validation has greater performance, because it 
does not do some postback. If clients do not support client-side validation, 
we can use the server side validation instead.


/////////////////////////////////////////////////////////////////////////////
Validator Control Basics

RequiredFieldValidator
Ensures that the user does not skip a form entry field. 
The property setting:
	ControlToValidate - to which control the validator is applied.
	ErrorMessage - the error message that will be displayed in the 
	validation summary.

CompareValidator
Allows for comparisons between the user's input and another item using a 
comparison operator.
The property setting:
	ControlToValidate - to which control the validator is applied.
	ErrorMessage - the error message that will be displayed in the 
	validation summary.
	ControlToCompare - gets or sets the input control to compare with the 
	input control being validated. 

RegularExpressionValidator
Checks that the user¡¯s entry matches a pattern defined by a regular 
expression.
The property setting:
	ControlToValidate - to which control the validator is applied.
	ErrorMessage - the error message that will be displayed in the 
	validation summary.
	ValidationExpression - determines the pattern used to validate a field.

CustomValidator
Checks the user's entry using custom-coded validation logic. 
The property setting:
	ControlToValidate - to which control the validator is applied.
	ErrorMessage - the error message that will be displayed in the 
	validation summary.
	ClientValidationFunction - determines client-side script function used 
	for validation.

ValidationSummary
Displays all the error messages from the validators in one specific spot on 
the page.


/////////////////////////////////////////////////////////////////////////////
References:

Validating ASP.NET Server Controls
http://msdn.microsoft.com/en-us/library/aa479013.aspx


/////////////////////////////////////////////////////////////////////////////