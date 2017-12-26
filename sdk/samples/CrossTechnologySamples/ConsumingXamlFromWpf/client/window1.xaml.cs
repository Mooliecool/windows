//  Copyright (c) Microsoft Corporation.  All Rights Reserved.

using System;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Media;
using System.Windows.Shapes;
using System.IO;
using Microsoft.ServiceModel.Samples;
using System.Windows.Markup;
using System.Xml;
using System.Windows.Threading;

namespace ConsumingXamlFromWpf
{
    public partial class Window1 : Window
    {
        public Window1()
        {
            InitializeComponent();
        }

        protected override void OnInitialized(EventArgs e)
        {
            base.OnInitialized(e);

            // Handle button click
            _getFruitButton.Click += _getFruitButton_Click;
        }

        // Keep proxy for lifetime of Window
        ProvideFruitClient client = new ProvideFruitClient();

        protected override void OnClosed(EventArgs e)
        {
            base.OnClosed(e);

            // Clean up proxy when Window closes
            client.Close();
        }

        void _getFruitButton_Click(object sender, RoutedEventArgs e)
        {
            // Get the four fruit asynchronously, calling GotFruit when
            // each piece is available and passing the type of the fruit
            // via IAsyncResult.AsyncState so we can label each piece of fruit
            client.BeginGetFruit(FruitType.Banana, GotFruit, FruitType.Banana);
            client.BeginGetFruit(FruitType.Cherry, GotFruit, FruitType.Cherry);
            client.BeginGetFruit(FruitType.Lemon, GotFruit, FruitType.Lemon);
            client.BeginGetFruit(FruitType.Lime, GotFruit, FruitType.Lime);
        }

        // Show the result when call to web service completes
        void GotFruit(IAsyncResult ar)
        {
            //This call back is not coming on the ui thread, so we must use the
            //Dispatcher to invoke on the ui thread
            Dispatcher.Invoke(DispatcherPriority.Normal, new AsyncCallback(AddFruit), ar);
        }

        void AddFruit(IAsyncResult ar)
        {
            // Harvest the results
            FruitType fruitType = (FruitType)ar.AsyncState;
            string fruitXaml = client.EndGetFruit(ar);

            // Parse the XAML data
            FrameworkElement graphic = null;
            using (StringReader sreader = new StringReader(fruitXaml))
            using (XmlTextReader xreader = new XmlTextReader(sreader))
            {
                graphic = (FrameworkElement)XamlReader.Load(xreader);
                graphic.LayoutTransform = new ScaleTransform(0.25, 0.25);
            }

            // Compose some text and the graphic
            TextBlock text = new TextBlock();
            text.Text = fruitType.ToString() + ": ";

            StackPanel fruitPanel = new StackPanel();
            fruitPanel.Orientation = Orientation.Horizontal;
            fruitPanel.Children.Add(text);
            fruitPanel.Children.Add(graphic);

            // Show the text and the graphic

            _stackPanel.Children.Add(fruitPanel);
        }
    }
}