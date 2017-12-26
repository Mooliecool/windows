using System;
using System.Collections.Generic;
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

namespace PopupPlacement
{
    /// <summary>
    /// Interaction logic for NumericUpDownControl.xaml
    /// </summary>

    public partial class NumericUpDownControl : System.Windows.Controls.UserControl
    {
        /// <summary>
        /// Initializes a new instance of the NumericUpDownControl.
        /// </summary>
        public NumericUpDownControl()
        {
            InitializeComponent();

            UpdateTextBlock();
        }

        /// <summary>
        /// Identifies the DecreaseButtonContent property.
        /// </summary>
        public static readonly DependencyProperty DecreaseButtonContentProperty =
            DependencyProperty.Register("DecreaseButtonContent", typeof(object),
            typeof(NumericUpDownControl),
            new PropertyMetadata(new PropertyChangedCallback(OnDecreaseTextChanged)));


        private static void OnDecreaseTextChanged(DependencyObject obj, DependencyPropertyChangedEventArgs args)
        {
            NumericUpDownControl control = (NumericUpDownControl)obj;
            control.downButton.Content = args.NewValue;


        }

        /// <summary>
        /// Gets or sets the content in the Button that
        /// Decreases Value.
        /// </summary>
        public object DecreaseButtonContent
        {
            get { return GetValue(DecreaseButtonContentProperty); }
            set { SetValue(DecreaseButtonContentProperty, value); }
        }



        ////////////////////////////////////////////////////////////////
        /// <summary>
        /// Identifies the IncreaseButtonContent property.
        /// </summary>
        public static readonly DependencyProperty IncreaseButtonContentProperty =
            DependencyProperty.Register("IncreaseButtonContent", typeof(object),
            typeof(NumericUpDownControl),
            new PropertyMetadata(new PropertyChangedCallback(OnIncreaseTextChanged)));


        private static void OnIncreaseTextChanged(DependencyObject obj, DependencyPropertyChangedEventArgs args)
        {
            NumericUpDownControl control = (NumericUpDownControl)obj;
            control.upButton.Content = args.NewValue;


        }

        /// <summary>
        /// Gets or sets the content in the Button that
        /// increases Value.
        /// </summary>
        public object IncreaseButtonContent
        {
            get { return GetValue(IncreaseButtonContentProperty); }
            set {SetValue(IncreaseButtonContentProperty, value);}
        }


        /// <summary>
        /// Gets or sets the value assigned to the control.
        /// </summary>
        public decimal Value
        {
            get { return (decimal)GetValue(ValueProperty); }
            set { SetValue(ValueProperty, value); }
        }

        /// <summary>
        /// Identifies the Value dependency property.
        /// </summary>
        public static readonly DependencyProperty ValueProperty =
            DependencyProperty.Register(
                "Value", typeof(decimal), typeof(NumericUpDownControl),
                new FrameworkPropertyMetadata(new PropertyChangedCallback(OnValueChanged)));

        private static void OnValueChanged(DependencyObject obj, DependencyPropertyChangedEventArgs args)
        {
            NumericUpDownControl control = (NumericUpDownControl)obj;
            control.UpdateTextBlock();

            RoutedPropertyChangedEventArgs<decimal> e = new RoutedPropertyChangedEventArgs<decimal>(
                (decimal)args.OldValue, (decimal)args.NewValue, ValueChangedEvent);
            control.OnValueChanged(e);
        }

        /// <summary>
        /// Identifies the ValueChanged routed event.
        /// </summary>
        public static readonly RoutedEvent ValueChangedEvent = EventManager.RegisterRoutedEvent(
            "ValueChanged", RoutingStrategy.Bubble,
            typeof(RoutedPropertyChangedEventHandler<decimal>), typeof(NumericUpDownControl));

        /// <summary>
        /// Occurs when the Value property changes.
        /// </summary>
        public event RoutedPropertyChangedEventHandler<decimal> ValueChanged
        {
            add { AddHandler(ValueChangedEvent, value); }
            remove { RemoveHandler(ValueChangedEvent, value); }
        }

        /// <summary>
        /// Raises the ValueChanged event.
        /// </summary>
        /// <param name="args">Arguments associated with the ValueChanged event.</param>
        protected virtual void OnValueChanged(RoutedPropertyChangedEventArgs<decimal> args)
        {
            RaiseEvent(args);
        }

        private void upButton_Click(object sender, EventArgs e)
        {
            Value++;
        }

        private void downButton_Click(object sender, EventArgs e)
        {
            Value--;
        }

        private void UpdateTextBlock()
        {
            valueText.Text = Value.ToString();
        }

    }
}