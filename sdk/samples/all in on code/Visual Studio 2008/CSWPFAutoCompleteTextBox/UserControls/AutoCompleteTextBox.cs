/************************************* Module Header **************************************\
* Module Name:  AutoCompleteTextBox.cs
* Project:      CSWPFAutoCompleteTextBox
* Copyright (c) Microsoft Corporation.
* 
* This example demonstrates how to achieve AutoComplete TextBox in WPF Application.
* 
* 
* This source is subject to the Microsoft Public License.
* See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
* All other rights reserved.
* 
* History:
* * 10/20/2009 3:00 PM Bruce Zhou Created
 * 
\******************************************************************************************/



using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Navigation;
using System.Windows.Shapes;
using System.Collections.ObjectModel;

namespace CSWPFAutoCompleteTextBox.UserControls
{

    // Achieve AutoComplete TextBox or ComboBox
    public class AutoCompleteTextBox : ComboBox
    {
        /// <summary>
        /// Initializes a new instance of the <see cref="AutoCompleteTextBox"/> class.
        /// </summary>
        public AutoCompleteTextBox()
        {
            //load and apply style to the ComboBox.
            ResourceDictionary rd = new ResourceDictionary();
            rd.Source = new Uri("/" + this.GetType().Assembly.GetName().Name + 
                ";component/UserControls/AutoCompleteComboBoxStyle.xaml",
                 UriKind.Relative);
            this.Resources = rd;
            //disable default Text Search Function
            this.IsTextSearchEnabled = false;
        }
 
        /// <summary>
        ///  override OnApplyTemplate method 
        /// get TextBox control out of Combobox control, and hook up TextChanged event.
        /// </summary>
        public override void OnApplyTemplate()
        {
            base.OnApplyTemplate();
            //get the textbox control in the ComboBox control
            TextBox textBox = this.Template.FindName("PART_EditableTextBox", this) as TextBox;
            if (textBox != null)
            {
                //disable Autoword selection of the TextBox
                textBox.AutoWordSelection = false;
                //handle TextChanged event to dynamically add Combobox items.
                textBox.TextChanged += new TextChangedEventHandler(textBox_TextChanged);
            }
        }

        //The autosuggestionlist source.
        private ObservableCollection<string> _autoSuggestionList = new ObservableCollection<string>();

        /// <summary>
        /// Gets or sets the auto suggestion list.
        /// </summary>
        /// <value>The auto suggestion list.</value>
        public ObservableCollection<string> AutoSuggestionList
        {
            get { return _autoSuggestionList; }
            set { _autoSuggestionList = value; }
        }

      
        /// <summary>
        /// main logic to generate auto suggestion list.
        /// </summary>
        /// <param name="sender">The source of the event.</param>
        /// <param name="e">The <see cref="System.Windows.Controls.TextChangedEventArgs"/> 
        /// instance containing the event data.</param>
        void textBox_TextChanged(object sender, TextChangedEventArgs e)
        {
            TextBox textBox = sender as TextBox;
            textBox.AutoWordSelection = false;
            // if the word in the textbox is selected, then don't change item collection
            if ((textBox.SelectionStart != 0 || textBox.Text.Length==0))
            {
                this.Items.Clear();
                //add new filtered items according the current TextBox input
                if (!string.IsNullOrEmpty(textBox.Text))
                {
                    foreach (string s in this._autoSuggestionList)
                    {
                        if (s.StartsWith(textBox.Text, StringComparison.InvariantCultureIgnoreCase))
                        {

                            string unboldpart = s.Substring(textBox.Text.Length);
                            string boldpart = s.Substring(0, textBox.Text.Length);
                            //construct AutoCompleteEntry and add to the ComboBox
                            AutoCompleteEntry entry = new AutoCompleteEntry(s, boldpart, unboldpart);
                            this.Items.Add(entry);
                        }
                    }
                }
            }
            // open or close dropdown of the ComboBox according to whether there are items in the 
            // fitlered result.
            this.IsDropDownOpen = this.HasItems;

            //avoid auto selection
            textBox.Focus();
            textBox.SelectionStart = textBox.Text.Length;

        }
    }

    //class derived from ComboBoxItem.
    /// <summary>
    /// Extended ComboBox Item
    /// </summary>
    public class AutoCompleteEntry : ComboBoxItem
    {
        private TextBlock tb;

        //text of the item
        private string _text;

        /// <summary>
        /// Contrutor of AutoCompleteEntry class
        /// </summary>
        /// <param name="text">All the Text of the item </param>
        /// <param name="bold">The already entered part of the Text</param>
        /// <param name="unbold">The remained part of the Text</param>
        public AutoCompleteEntry(string text, string bold, string unbold)
        {
            _text = text;
            tb = new TextBlock();
            //highlight the current input Text
            tb.Inlines.Add(new Run { Text = bold, FontWeight = FontWeights.Bold,
                Foreground = new SolidColorBrush(Colors.RoyalBlue) });
            tb.Inlines.Add(new Run { Text = unbold });
            this.Content = tb;
        }

        /// <summary>
        /// Gets the text.
        /// </summary>
        public string Text
        {
            get { return _text; }
        }
    }
}
