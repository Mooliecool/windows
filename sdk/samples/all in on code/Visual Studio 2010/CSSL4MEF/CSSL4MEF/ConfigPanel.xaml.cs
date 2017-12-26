/****************************** Module Header ******************************\
* Module Name:                ConfigPanel.xaml.cs
* Project:                    CSSL4MEF
* Copyright (c) Microsoft Corporation.
* 
* ConfigPanel's code behind file.
* 
* This source is subject to the Microsoft Public License.
* See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
* All other rights reserved.
* 
* THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND, 
* EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED 
* WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
\***************************************************************************/

using System;
using System.Linq;
using System.Net;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Media;
using System.ComponentModel.Composition;
using System.Reflection;
using System.ComponentModel.DataAnnotations;
using System.ComponentModel;
using ConfigControl.Contract;

namespace CSSL4MEF
{
    /// <summary>
    /// ConfigPanel use reflection to traverse the properties of binded datamodel,
    /// and generate config controls automaticly for each property.
    /// </summary>
    public partial class ConfigPanel : UserControl,IPartImportsSatisfiedNotification
    {
        public ConfigPanel()
        {
            InitializeComponent();
            CompositionInitializer.SatisfyImports(this);
        }

        /// <summary>
        /// Create DependencyProperty, ConfigPanel with generate config controls for 
        /// this property value.
        /// </summary>
        public object ConfigData
        {
            get { return (object)GetValue(ConfigDataProperty); }
            set { SetValue(ConfigDataProperty, value); }
        }

        public static readonly DependencyProperty ConfigDataProperty =
            DependencyProperty.Register("ConfigData", typeof(object),
            typeof(ConfigPanel),
            new PropertyMetadata(
                new PropertyChangedCallback((s, e) =>
                {
                    // Regenerate controls when datamodel changed.
                    ((ConfigPanel)s).BindConfig(e.NewValue);
                })));     

        /// <summary>
        /// Import ConfigControl list
        /// </summary>
        [ImportMany(AllowRecomposition = true)]
        public Lazy<IConfigControl, IConfigAttributes>[] ConfigControls { set; get; }

        // 
        /// <summary>
        /// Traverse dataModel, find appropriate controls for each property
        /// </summary>
        /// <param name="datamodel">data model</param>
        void BindConfig(object datamodel)
        {
            if (datamodel != null)
            {
                ConfigList.Items.Clear();
                foreach (var property in datamodel.GetType().
                    GetProperties().OrderBy(p=>p.Name))
                {
                    var item = new ListBoxItem();

                    var border = new Border();
                    border.Margin = new Thickness(0, 0, 0, 5);
                    border.Padding = new Thickness(5);
                    border.BorderThickness = new Thickness(0,1,0,0);
                    border.BorderBrush = new SolidColorBrush(Colors.Black);

                    var stackpanel = new StackPanel();
                    stackpanel.Children.Add(
                        new TextBlock
                        {
                            Text = property.Name,
                            FontStyle=FontStyles.Italic
                        });

                    var propertycontrol = GetBestMatch(property);
                    var propertyview = propertycontrol.Value.CreateView(property);
                    stackpanel.Children.Add(propertyview);
                    stackpanel.DataContext = datamodel;

                    item.Content = border;
                    border.Child = stackpanel;

                    item.HorizontalContentAlignment = HorizontalAlignment.Stretch;
                    ConfigList.Items.Add(item);
                }
            }
        }

        // Find most suitable config control in Imported configControl list
        // for property
        Lazy<IConfigControl, IConfigAttributes> GetBestMatch(PropertyInfo property)
        {
            Lazy<IConfigControl, IConfigAttributes> bestMatch = null;

            // If UIHint match control's name, use that specified control
            // for this property.
            var uihintAttr = property.GetCustomAttributes(typeof(UIHintAttribute),
                false).FirstOrDefault() as UIHintAttribute;
            if (uihintAttr != null)
            {
                bestMatch = ConfigControls.FirstOrDefault(lz => lz.Metadata.Name.
                    Equals(uihintAttr.UIHint, StringComparison.OrdinalIgnoreCase));
                if (bestMatch != null)
                {
                    return bestMatch;
                }
            }

            // Find most matches control for property.
            var maxmatch = MatchResult.NotMatch;
            foreach (var configcontrol in ConfigControls)
            {
                var matchresult = configcontrol.Value.MatchTest(property);

                if (matchresult == MatchResult.Recommended)
                    return configcontrol;

                if (matchresult > maxmatch)
                {
                    maxmatch = matchresult;
                    bestMatch = configcontrol;
                }
            }
            
            return bestMatch;
        }

        // Implement IPartImportsSatisfiedNotification interface, when Import
        // parts get changed, this method would be invoked
        public void OnImportsSatisfied()
        {
            BindConfig(ConfigData);
        }
    }
}
