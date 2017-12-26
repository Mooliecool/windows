using System;
using System.Windows;
using System.IO;
using System.Windows.Markup;
using System.Windows.Controls;
using System.Windows.Documents;
using System.Windows.Navigation;
using System.ComponentModel;
using System.Windows.Media;
using System.Xml;

namespace Guide
{
	public partial class Page1 : Page
	{
        Application app;
        public bool RealTimeUpdate = true;

        public void menuExit(object sender, RoutedEventArgs args)
            {
                app = (Application)System.Windows.Application.Current;
                app.Shutdown();
            }

            public void setXAML(object sender, EventArgs e)
            {
                myCombo.SelectedValue = cbItem5;
            }

        //Begin XAMLPAD integration

            protected void HandleTextChanged(object sender, TextChangedEventArgs me)
            {
                if (RealTimeUpdate) ParseCurrentBuffer();
            }

            private void ParseCurrentBuffer()
            {
                try
                {
                    MemoryStream ms = new MemoryStream();
                    StreamWriter sw = new StreamWriter(ms);
                    string str = TextBox1.Text;
                    sw.Write(str);
                    sw.Flush();
                    ms.Flush();
                    ms.Position = 0;
                    try
                    {
                        object content = XamlReader.Load(ms);
                        if (content != null)
                        {

                            cc.Children.Clear();
                            cc.Children.Add((UIElement)content);
                        }
                        TextBox1.Foreground = System.Windows.Media.Brushes.Black;
                        ErrorText.Text = "";
                    }

                    catch (XamlParseException xpe)
                    {
                        TextBox1.Foreground = System.Windows.Media.Brushes.Red;
                        ErrorText.Text = xpe.Message.ToString();
                    }
                }
                catch (Exception)
                {
                    return;
                }
            }

        // End integration of XAMLPAD

        public void selectLang(Object sender, RoutedEventArgs e)
        {
            if (cbItem1.IsSelected == true)
            {
                Welcome.Page1.myDouble = 1;
                frame2.Refresh();
            }
            else if (cbItem2.IsSelected == true)
            {
                Welcome.Page1.myDouble = 2;
                frame2.Refresh();
            }
            else if (cbItem3.IsSelected == true)
            {
                Welcome.Page1.myDouble = 3;
                frame2.Refresh();
            }
            else if (cbItem4.IsSelected == true)
            {
                Welcome.Page1.myDouble = 4;
                frame2.Refresh();
            }
            else if (cbItem5.IsSelected == true)
            {
                Welcome.Page1.myDouble = 5;
                frame2.Refresh();
            }
            else if (cbItem6.IsSelected == true)
            {
                Welcome.Page1.myDouble = 6;
                frame2.Refresh();
            }
            else if (cbItem7.IsSelected == true)
            {
                Welcome.Page1.myDouble = 7;
                frame2.Refresh();
            }
        }
    }

}