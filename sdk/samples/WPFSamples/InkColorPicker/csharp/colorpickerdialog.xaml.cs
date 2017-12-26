using System;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Media;
using System.Windows.Ink;
using System.Windows.Shapes;

namespace InkColorPicker
{
    public partial class ColorPicker : Canvas
    {
        /// <summary>
        /// Class members
        /// </summary>
        DrawingAttributes theDrawingAttributes;
        bool _notUserInitiated;

        /// <summary>
        /// Initialization
        /// </summary>
        public ColorPicker()
        {
            InitializeComponent();
        }

        /// <summary>
        /// Completes initialization after all XAML member vars have been initialized.
        /// </summary>
        /// <param name="e"></param>
        protected override void OnInitialized(EventArgs e)
        {
            base.OnInitialized(e);

            theDrawingAttributes = new DrawingAttributes();
            UpdateControlValues();
            UpdateControlVisuals();

            colorComb.ColorSelected += new EventHandler<ColorEventArgs>(colorComb_ColorSelected);
            brightnessSlider.ValueChanged += new RoutedPropertyChangedEventHandler<double>(brightnessSlider_ValueChanged);
            opacitySlider.ValueChanged += new RoutedPropertyChangedEventHandler<double>(opacitySlider_ValueChanged);
            ellipticalRadio.Checked += new RoutedEventHandler(radio_Click);
            rectangularRadio.Checked += new RoutedEventHandler(radio_Click);
            ignorepsiCheckbox.Click += new RoutedEventHandler(checkbox_Click);
            fitcurveCheckbox.Click += new RoutedEventHandler(checkbox_Click);
            decrementThickness.Click += new RoutedEventHandler(decrementThickness_Click);
            incrementThickness.Click += new RoutedEventHandler(incrementThickness_Click);
        }

        /// <summary>
        /// Interface
        /// </summary>
        public DrawingAttributes SelectedDrawingAttributes
        {
            get 
            { 
                return theDrawingAttributes;
            }
            set
            {
                theDrawingAttributes = value;
                UpdateControlValues();
                UpdateControlVisuals();
            }
        }

        /// <summary>
        /// Updates values of controls when new DA is set (or upon initialization).
        /// </summary>
        public void UpdateControlValues()
        {
            _notUserInitiated = true;
            try
            {
                // Set nominal color on comb.
                Color nc = theDrawingAttributes.Color;
                float f = Math.Max(Math.Max(nc.ScR, nc.ScG), nc.ScB);
                if (f < 0.001f) // black
                    nc = Color.FromScRgb(1f, 1f, 1f, 1f);
                else
                    nc = Color.FromScRgb(1f, nc.ScR / f, nc.ScG / f, nc.ScB / f);
                colorComb.SelectedColor = nc;

                // Set brightness and opacity.
                brightnessSlider.Value = f;
                opacitySlider.Value = theDrawingAttributes.Color.ScA;

                // Set stylus characteristics.
                ellipticalRadio.IsChecked = (theDrawingAttributes.StylusTip == StylusTip.Ellipse);
                rectangularRadio.IsChecked = (theDrawingAttributes.StylusTip == StylusTip.Rectangle);
                ignorepsiCheckbox.IsChecked = !(theDrawingAttributes.IgnorePressure);
                fitcurveCheckbox.IsChecked = (theDrawingAttributes.FitToCurve);
            }
            finally
            {
                _notUserInitiated = false;
            }
        }

        /// <summary>
        /// Updates visual properties of all controls, in response to any change.
        /// </summary>
        public void UpdateControlVisuals()
        {
            Color c = theDrawingAttributes.Color;

            // Update LGB for brightnessSlider
            Border sb1 = brightnessSlider.Parent as Border;
            LinearGradientBrush lgb1 = sb1.Background as LinearGradientBrush;
            lgb1.GradientStops[1].Color = colorComb.SelectedColor;

            // Update LGB for opacitySlider
            Color c2a = Color.FromScRgb(0f, c.ScR, c.ScG, c.ScB);
            Color c2b = Color.FromScRgb(1f, c.ScR, c.ScG, c.ScB);
            Border sb2 = opacitySlider.Parent as Border;
            LinearGradientBrush lgb2 = sb2.Background as LinearGradientBrush;
            lgb2.GradientStops[0].Color = c2a;
            lgb2.GradientStops[1].Color = c2b;

            // Update controls
            theDrawingAttributes.Width = Math.Round(theDrawingAttributes.Width, 2);
            thicknessTextbox.Text = theDrawingAttributes.Width.ToString();
            fitcurveCheckbox.IsChecked = theDrawingAttributes.FitToCurve;
            ignorepsiCheckbox.IsChecked = !theDrawingAttributes.IgnorePressure;
            ellipticalRadio.IsChecked = (theDrawingAttributes.StylusTip == StylusTip.Ellipse);
            rectangularRadio.IsChecked = (theDrawingAttributes.StylusTip == StylusTip.Rectangle);
        }

        #region Event Handlers to update color picker UI
        void colorComb_ColorSelected(object sender, ColorEventArgs e)
        {
            if (_notUserInitiated) return;

            float a, f, r, g, b;
            a = (float)opacitySlider.Value;
            f = (float)brightnessSlider.Value;

            Color nc = e.Color;
            r = f * nc.ScR;
            g = f * nc.ScG;
            b = f * nc.ScB;

            theDrawingAttributes.Color = Color.FromScRgb(a, r, g, b);
            UpdateControlVisuals();
        }

        void brightnessSlider_ValueChanged(object sender, RoutedPropertyChangedEventArgs<double> e)
        {
            if (_notUserInitiated) return;

            Color nc = colorComb.SelectedColor;
            float f = (float)e.NewValue;

            float a, r, g, b;
            a = (float)opacitySlider.Value;
            r = f * nc.ScR;
            g = f * nc.ScG;
            b = f * nc.ScB;

            theDrawingAttributes.Color = Color.FromScRgb(a, r, g, b);
            UpdateControlVisuals();
        }

        void opacitySlider_ValueChanged(object sender, RoutedPropertyChangedEventArgs<double> e)
        {
            if (_notUserInitiated) return;

            Color c = theDrawingAttributes.Color;
            float a = (float)e.NewValue;

            theDrawingAttributes.Color = Color.FromScRgb(a, c.ScR, c.ScG, c.ScB);
            UpdateControlVisuals();
        }

        void radio_Click(object sender, RoutedEventArgs e)
        {
            if (_notUserInitiated) return;

            if (sender == ellipticalRadio)
                theDrawingAttributes.StylusTip = StylusTip.Ellipse;
            if (sender == rectangularRadio)
                theDrawingAttributes.StylusTip = StylusTip.Rectangle;

            UpdateControlVisuals();
        }

        void checkbox_Click(object sender, RoutedEventArgs e)
        {
            if (_notUserInitiated) return;

            if (sender == ignorepsiCheckbox)
                theDrawingAttributes.IgnorePressure = (ignorepsiCheckbox.IsChecked!=true);
            if (sender == fitcurveCheckbox)
                theDrawingAttributes.FitToCurve = (fitcurveCheckbox.IsChecked==true);

            UpdateControlVisuals();
        }

        void incrementThickness_Click(object sender, RoutedEventArgs e)
        {
            if (_notUserInitiated) return;

            if (theDrawingAttributes.Width < 1.0)
            {
                theDrawingAttributes.Width += 0.1;
                theDrawingAttributes.Height += 0.1;
            }
            else if (theDrawingAttributes.Width < 10.0)
            {
                theDrawingAttributes.Width += 0.5;
                theDrawingAttributes.Height += 0.5;
            }
            else
            {
                theDrawingAttributes.Width += 1d;
                theDrawingAttributes.Height += 1d;
            }

            UpdateControlVisuals();
        }

        void decrementThickness_Click(object sender, RoutedEventArgs e)
        {
            if (_notUserInitiated) return;

            if (theDrawingAttributes.Width < 0.1001)
                return;

            if (theDrawingAttributes.Width < 1.001)
            {
                theDrawingAttributes.Width -= 0.1;
                theDrawingAttributes.Height -= 0.1;
            }
            else if (theDrawingAttributes.Width < 10.001)
            {
                theDrawingAttributes.Width -= 0.5;
                theDrawingAttributes.Height -= 0.5;
            }
            else
            {
                theDrawingAttributes.Width -= 1d;
                theDrawingAttributes.Height -= 1d;
            }

            UpdateControlVisuals();
        }
        #endregion 
    }
}