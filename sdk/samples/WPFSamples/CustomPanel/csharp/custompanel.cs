using System;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Media;
using System.Windows.Shapes;
using System.Threading;

namespace Custom_Panel
{
	public class app : Application
	{
        Window mainWindow;
        CustomPanel customPanel1;
        Rectangle rect1;
        Rectangle rect2;
        Rectangle rect3;
        TextBlock txt1;

        protected override void OnStartup(StartupEventArgs e)
        {
            base.OnStartup(e);
            CreateAndShowMainWindow();
        }

        private void CreateAndShowMainWindow()
        {
            // Create the application's main window and instantiate a CustomPanel

            customPanel1 = new CustomPanel();
            customPanel1.Width = 450;
            customPanel1.Height = 450;

			// Add elements to populate the CustomPanel
			
            rect1 = new Rectangle();
            rect2 = new Rectangle();
            rect3 = new Rectangle();
            rect1.Width = 200;
            rect1.Height = 200;
            rect1.Fill = Brushes.Blue;
            rect2.Width = 200;
            rect2.Height = 200;
            rect2.Fill = Brushes.Purple;
            rect3.Width = 200;
            rect3.Height = 200;
            rect3.Fill = Brushes.Red;
            
			// Add a TextBlock to show flowing and clip behavior
			
            txt1 = new TextBlock();
            txt1.Text = "Text is clipped when it reaches the edge of the container";
            txt1.FontSize = 25;

			// Add the elements defined above as children of the CustomPanel
			
            customPanel1.Children.Add(rect1);
            customPanel1.Children.Add(rect2);
            customPanel1.Children.Add(rect3);
            customPanel1.Children.Add(txt1);

			// Add the CustomPanel as a Child of the MainWindow and show the Window
			
            mainWindow = new Window();
            mainWindow.Content = customPanel1;
            mainWindow.Title = "Custom Panel Sample";
            mainWindow.Show();
        }

		// Define the CustomPanel class, derived from Panel
		
        public class CustomPanel : Panel
        {
		
		// Define a default Constructor

            public CustomPanel() : base()
            {
            }

			// Override the Measure method of Panel
			
            protected override Size MeasureOverride(Size constraint)
            {
                Size curLineSize = new Size();
                Size panelSize = new Size();

                UIElementCollection children = InternalChildren;

                for (int i = 0; i < children.Count; i++)
                {
                    UIElement child = children[i] as UIElement;

                    // Flow passes its own constraint to children
					
                    child.Measure(constraint);
                    Size sz = child.DesiredSize;

                    if (curLineSize.Width + sz.Width > constraint.Width) //need to switch to another line
                    {
                        panelSize.Width = Math.Max(curLineSize.Width, panelSize.Width);
                        panelSize.Height += curLineSize.Height;
                        curLineSize = sz;

                        if (sz.Width > constraint.Width) // if the element is wider then the constraint - give it a separate line                    
                        {
                            panelSize.Width = Math.Max(sz.Width, panelSize.Width);
                            panelSize.Height += sz.Height;
                            curLineSize = new Size();
                        }
                    }
                    else //continue to accumulate a line
                    {
                        curLineSize.Width += sz.Width;
                        curLineSize.Height = Math.Max(sz.Height, curLineSize.Height);
                    }
                }

                // the last line size, if any need to be added

                panelSize.Width = Math.Max(curLineSize.Width, panelSize.Width);
                panelSize.Height += curLineSize.Height;

                return panelSize;
            }


            protected override Size ArrangeOverride(Size arrangeBounds)
            {
                int firstInLine = 0;

                Size curLineSize = new Size();

                double accumulatedHeight = 0;

                UIElementCollection children = InternalChildren;

                for (int i = 0; i < children.Count; i++)
                {
                    Size sz = children[i].DesiredSize;

                    if (curLineSize.Width + sz.Width > arrangeBounds.Width) //need to switch to another line
                    {
                        arrangeLine(accumulatedHeight, curLineSize.Height, firstInLine, i);

                        accumulatedHeight += curLineSize.Height;
                        curLineSize = sz;

                        if (sz.Width > arrangeBounds.Width) //the element is wider then the constraint - give it a separate line                    
                        {
                            arrangeLine(accumulatedHeight, sz.Height, i, ++i);
                            accumulatedHeight += sz.Height;
                            curLineSize = new Size();
                        }
                        firstInLine = i;
                    }
                    else //continue to accumulate a line
                    {
                        curLineSize.Width += sz.Width;
                        curLineSize.Height = Math.Max(sz.Height, curLineSize.Height);
                    }
                }

                if (firstInLine < children.Count)
                    arrangeLine(accumulatedHeight, curLineSize.Height, firstInLine, children.Count);

                return arrangeBounds;
            }

            private void arrangeLine(double y, double lineHeight, int start, int end)
            {
                double x = 0;
                UIElementCollection children = InternalChildren;
                for (int i = start; i < end; i++)
                {
                    UIElement child = children[i];
                    child.Arrange(new Rect(x, y, child.DesiredSize.Width, lineHeight));
                    x += child.DesiredSize.Width;
                }
            }

        }


    }

	// Run the application
	
    internal static class EntryClass
    {
        [System.STAThread()]
        private static void Main()
        {
            app app = new app();
            app.Run();
        }
    }


}
