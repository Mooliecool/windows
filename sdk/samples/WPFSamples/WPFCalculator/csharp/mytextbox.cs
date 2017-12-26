#region Using directives

using System;
using System.Collections.Generic;
using System.Text;
using System.Windows.Input;

#endregion

namespace WPFCalculator
{
    class MyTextBox : System.Windows.Controls.TextBox
    {
        protected override void OnPreviewGotKeyboardFocus(System.Windows.Input.KeyboardFocusChangedEventArgs e)
        {
            e.Handled = true;
            base.OnPreviewGotKeyboardFocus(e);
        }
 

    }
}
