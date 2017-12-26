using System;
using System.Globalization;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Animation;

namespace InkCanvasEditingModes
{
    /// <summary>
    /// An Adorner which displays animated text message. 
    /// </summary>
    public class MessageAdorner : Adorner
    {
        #region Construtor

        /// <summary>
        /// Constructor
        /// </summary>
        /// <param name="adornedElement"></param>
        public MessageAdorner(UIElement adornedElement) : base(adornedElement) {}

        #endregion Construtor


        #region Public Methods

        /// <summary>
        /// Show a text message at the specified location
        /// </summary>
        /// <param name="message"></param>
        /// <param name="location"></param>
        public void ShowMessage(string message, Point location)
        {
            // Format the message text
            _message = new FormattedText(message, CultureInfo.InvariantCulture,
               FlowDirection.LeftToRight, MessageTypeface, 32, Brushes.DarkGray);

            _location = location;

            // Setup an opacity animation
            Storyboard storyboard = new Storyboard();
            PropertyPath opacityPropertyPath = new PropertyPath(UIElement.OpacityProperty);

            DoubleAnimation animationOpacity = new DoubleAnimation(1d, 0d, AnimationTimeSpan);
            animationOpacity.SetValue(Storyboard.TargetPropertyProperty, opacityPropertyPath);
            storyboard.Children.Add(animationOpacity);
            BeginStoryboard(storyboard);

            // Invalidate rendering
            InvalidateVisual();
        }

        #endregion Public Methods


        #region Protected Methods

        /// <summary>
        /// The overriden rendering Method
        /// </summary>
        /// <param name="drawingContext"></param>
        protected override void OnRender(DrawingContext drawingContext)
        {
            if (_message == null)
            {
                // Nothing to render
                return;
            }

            // Draw the text message
            drawingContext.DrawText(_message, _location);
        }

        #endregion Protected Methods

        #region Private Fields

        private FormattedText   _message;
        private Point           _location;
        private readonly TimeSpan   AnimationTimeSpan = new TimeSpan(0, 0, 1);
        private readonly Typeface   MessageTypeface = new Typeface(SystemFonts.MessageFontFamily, SystemFonts.MessageFontStyle,
                                                        SystemFonts.MessageFontWeight, FontStretches.Normal);

        #endregion Private Fields
    }
}