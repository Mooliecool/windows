================================================================================
       WINDOWS FORMS APPLICATION : CSWinFormPassValueBetweenForms Project Overview
       
                        Pass Value Between Forms Sample
                        
===============================================================================

/////////////////////////////////////////////////////////////////////////////
Use:

The Pass Value Between Forms sample demonstrates how to pass value between forms.
 
There're two common ways to pass value between forms:

1. Use a property.

   Create a public property on the target form class, then we can pass value 
   to the target form by setting value for the property.

2. Use a method.

   Create a public method on the target form class, then we can pass value to 
   the target form by passing the value as parameter to the method.
   

/////////////////////////////////////////////////////////////////////////////
Code Logic:

   1. Create two forms named FrmPassValueBetweenForms and 
      FrmPassValueBetweenForms2 respectively;
      
   2. Create a public property named ValueToPassBetweenForms in the 
      FrmPassValueBetweenForms2 class;
      
      private string _valueToPassBetweenForms;

      public string ValueToPassBetweenForms
      {
          get { return this._valueToPassBetweenForms; }
          set { this._valueToPassBetweenForms = value; }
      }

      
   3. Create a public method named SetValueFromAnotherForm in the 
      FrmPassValueBetweenForms2 class;
      
      public void SetValueFromAnotherForm(string val)
      {
          this._valueToPassBetweenForms = val;
      }
      
   4. On the FrmPassValueBetweenForms form, handle the Click event of the buttons.
      In the Click event handler of button1, set the SetValueFromAnotherForm 
      property for the FrmPassValueBetweenForms2 to pass the text value from 
      FrmPassValueBetweenForms to FrmPassValueBetweenForms2.
      In the Click event handler of button2, call the SetValueFromAnotherForm 
      method and pass the text value as parameter to the FrmPassValueBetweenForms2.


/////////////////////////////////////////////////////////////////////////////
References:
   
1. Windows Forms General FAQ.
   http://social.msdn.microsoft.com/Forums/en-US/winforms/thread/77a66f05-804e-4d58-8214-0c32d8f43191
   

/////////////////////////////////////////////////////////////////////////////
