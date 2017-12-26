using System;
using System.Net;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Documents;
using System.Windows.Ink;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Animation;
using System.Windows.Shapes;
using System.Windows.Browser;

namespace CSSL3DeepZoom
{
        // Courtesy of Pete Blois
        public class MouseWheelEventArgs : EventArgs
        {
            private double delta;
            private bool handled = false;

            public MouseWheelEventArgs(double delta)
            {
                this.delta = delta;
            }

            public double Delta
            {
                get { return this.delta; }
            }

            // Use handled to prevent the default browser behavior!
            public bool Handled
            {
                get { return this.handled; }
                set { this.handled = value; }
            }
        }

        public class MouseWheelHelper
        {

            public event EventHandler<MouseWheelEventArgs> Moved;
            private static Worker worker;
            private bool isMouseOver = false;

            public MouseWheelHelper(FrameworkElement element)
            {

                if (MouseWheelHelper.worker == null)
                    MouseWheelHelper.worker = new Worker();

                MouseWheelHelper.worker.Moved += this.HandleMouseWheel;

                element.MouseEnter += this.HandleMouseEnter;
                element.MouseLeave += this.HandleMouseLeave;
                element.MouseMove += this.HandleMouseMove;
            }

            private void HandleMouseWheel(object sender, MouseWheelEventArgs args)
            {
                if (this.isMouseOver)
                    this.Moved(this, args);
            }

            private void HandleMouseEnter(object sender, EventArgs e)
            {
                this.isMouseOver = true;
            }

            private void HandleMouseLeave(object sender, EventArgs e)
            {
                this.isMouseOver = false;
            }

            private void HandleMouseMove(object sender, EventArgs e)
            {
                this.isMouseOver = true;
            }

            private class Worker
            {

                public event EventHandler<MouseWheelEventArgs> Moved;

                public Worker()
                {

                    if (HtmlPage.IsEnabled)
                    {
                        HtmlPage.Window.AttachEvent("DOMMouseScroll", this.HandleMouseWheel);
                        HtmlPage.Window.AttachEvent("onmousewheel", this.HandleMouseWheel);
                        HtmlPage.Document.AttachEvent("onmousewheel", this.HandleMouseWheel);
                    }

                }

                private void HandleMouseWheel(object sender, HtmlEventArgs args)
                {
                    double delta = 0;

                    ScriptObject eventObj = args.EventObject;

                    if (eventObj.GetProperty("wheelDelta") != null)
                    {
                        delta = ((double)eventObj.GetProperty("wheelDelta")) / 120;


                        if (HtmlPage.Window.GetProperty("opera") != null)
                            delta = -delta;
                    }
                    else if (eventObj.GetProperty("detail") != null)
                    {
                        delta = -((double)eventObj.GetProperty("detail")) / 3;

                        if (HtmlPage.BrowserInformation.UserAgent.IndexOf("Macintosh") != -1)
                            delta = delta * 3;
                    }

                    if (delta != 0 && this.Moved != null)
                    {
                        MouseWheelEventArgs wheelArgs = new MouseWheelEventArgs(delta);
                        this.Moved(this, wheelArgs);

                        if (wheelArgs.Handled)
                            args.PreventDefault();
                    }
                }
            }
        }
}
