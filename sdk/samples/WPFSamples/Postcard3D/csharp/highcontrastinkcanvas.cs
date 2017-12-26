using System;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Ink;
using System.Windows.Input;
using System.Windows.Input.StylusPlugIns;
using System.Windows.Media;

namespace Postcard3D
{
    class HighContrastInkCanvas : InkCanvas
    {
        /// <summary>
        /// Custom InkCanvas element to collect
        /// and render high contrast ink
        /// </summary>
        public HighContrastInkCanvas()
            : base()
        {
            // set Color, Width and Height for the inner stroke
            this.DynamicRenderer.DrawingAttributes.Color = Colors.Chocolate;
            this.DynamicRenderer.DrawingAttributes.Width = 3d;
            this.DynamicRenderer.DrawingAttributes.Height = 3d;

            // create a second DynamicRenderer
            outerDynamicRenderer = new DynamicRenderer();
            outerDynamicRenderer.DrawingAttributes = this.DefaultDrawingAttributes.Clone();
            outerDynamicRenderer.DrawingAttributes.Color = Colors.DarkGreen;
            outerDynamicRenderer.DrawingAttributes.Width *= 1.4d;
            outerDynamicRenderer.DrawingAttributes.Height *= 1.4d;

            // plug the new DynamicRenderer in, so that it can receive
            // real-time stylus notifications
            this.StylusPlugIns.Add(outerDynamicRenderer);

            // re-order the RootVisuals of our DynamicRendererd, so that
            // the inner stroke gets rendered on top of the outer stroke
            this.InkPresenter.DetachVisuals(this.DynamicRenderer.RootVisual);
            this.InkPresenter.AttachVisuals(outerDynamicRenderer.RootVisual, outerDynamicRenderer.DrawingAttributes);
            this.InkPresenter.AttachVisuals(this.DynamicRenderer.RootVisual, this.DynamicRenderer.DrawingAttributes);
        }

        /// <summary>
        /// Occurs when a stroke drawn by the user is added to the Strokes property 
        /// </summary>
        protected override void OnStrokeCollected(InkCanvasStrokeCollectedEventArgs e)
        {
            base.OnStrokeCollected(e);

            // create a clone of the collected strokes with new
            // DrawingAttributes to represent the outline
            DrawingAttributes daOutline = GetOutlineDrawingAttributes(e.Stroke.DrawingAttributes);
            Stroke newStroke = new Stroke(e.Stroke.StylusPoints.Clone(), daOutline);

            // insert the new strokes at the proper location in
            // the stroke collection so that it will be renderer
            // behind the original stroke
            int index = this.Strokes.IndexOf(e.Stroke);
            this.Strokes.Insert(index, newStroke);
        }

        /// <summary>
        /// Occurs when the current editing mode changes 
        /// </summary>
        protected override void OnActiveEditingModeChanged(RoutedEventArgs e)
        {
            base.OnActiveEditingModeChanged(e);
            if (outerDynamicRenderer == null) return;

            if (this.ActiveEditingMode == InkCanvasEditingMode.Ink ||
                this.ActiveEditingMode == InkCanvasEditingMode.InkAndGesture ||
                this.ActiveEditingMode == InkCanvasEditingMode.GestureOnly)
            {
                outerDynamicRenderer.Enabled = true;
            }
            else
            {
                // prevent dynamic ink rendering when ActiveEditingMode
                // is not one of the inking modes
                outerDynamicRenderer.Enabled = false;
            }
        }
        /// <summary>
        /// Creates a new DrawingAttributes instance to represent
        /// the outline for given DrawingAttributes
        /// </summary>
        private DrawingAttributes GetOutlineDrawingAttributes(DrawingAttributes oirginalDrawingAttributes)
        {
            DrawingAttributes newDrawingAttributes = oirginalDrawingAttributes.Clone();

            // set Color, Width and Height for the outer stroke
            newDrawingAttributes.Color = Colors.DarkGreen;
            newDrawingAttributes.Width *= 1.4d;
            newDrawingAttributes.Height *= 1.4d;

            return newDrawingAttributes;
        }

        /// <summary>
        /// DynamicRender to render the stroke outline
        /// </summary>
        DynamicRenderer outerDynamicRenderer = null;
    }
}
