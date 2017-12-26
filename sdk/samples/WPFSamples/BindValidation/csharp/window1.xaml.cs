
using System;
using System.ComponentModel;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Documents;
using System.Windows.Navigation;
using System.Windows.Shapes;
using System.Windows.Data;
using System.Globalization;
using System.Collections.ObjectModel;

namespace SDKSample
{
  public partial class Window1 : Window
  {
      public Window1()
      {
          InitializeComponent();
      }

      void UseCustomHandler(object sender, RoutedEventArgs e)
      {

          BindingExpression myBindingExpression = textBox3.GetBindingExpression(TextBox.TextProperty);
          Binding myBinding = myBindingExpression.ParentBinding;
          myBinding.UpdateSourceExceptionFilter = new UpdateSourceExceptionFilterCallback(ReturnExceptionHandler);
          myBindingExpression.UpdateSource();
      }

      void DisableCustomHandler(object sender, RoutedEventArgs e)
      {
          // textBox3 is an instance of a TextBox
          // the TextProperty is the data-bound dependency property
          Binding myBinding = BindingOperations.GetBinding(textBox3, TextBox.TextProperty);
          myBinding.UpdateSourceExceptionFilter -= new UpdateSourceExceptionFilterCallback(ReturnExceptionHandler);
          BindingOperations.GetBindingExpression(textBox3, TextBox.TextProperty).UpdateSource();
      }

      object ReturnExceptionHandler(object bindingExpression, Exception exception)
      {
          return "This is from the UpdateSourceExceptionFilterCallBack.";
      }
  }
}
