//This is a list of commonly used namespaces for a window.
using System;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Documents;
using System.Windows.Navigation;
using System.Windows.Shapes;
using System.Windows.Data;

namespace SystemColorsAndBrushes_csharp
{
    /// <summary>
    /// Interaction logic for Window1.xaml
    /// </summary>

    public partial class Window1 : Window
    {
        
        
        public Window1() {
            InitializeComponent();

            listSystemBrushes();
            listGradientExamples();
        }

        // Demonstrates using system colors to fill rectangles and buttons.
        private void listSystemBrushes()
        {
            // The window style (defined in Window1.xaml) is used to
            // specify the height and width of each of the System.Windows.Shapes.Rectangles.
            System.Windows.Controls.TextBlock t = new System.Windows.Controls.TextBlock();
            t.Text = "ActiveBorder";
            System.Windows.Shapes.Rectangle r = new System.Windows.Shapes.Rectangle();
            r.Fill = SystemColors.ActiveBorderBrush;
            systemBrushesPanel.Children.Add(t);
            systemBrushesPanel.Children.Add(r);

            t = new System.Windows.Controls.TextBlock();
            t.Text = "ActiveCaption";
            r = new System.Windows.Shapes.Rectangle();
            r.Fill = SystemColors.ActiveCaptionBrush;
            systemBrushesPanel.Children.Add(t);
            systemBrushesPanel.Children.Add(r);

            t = new System.Windows.Controls.TextBlock();
            t.Text = "ActiveCaptionText";
            r = new System.Windows.Shapes.Rectangle();
            r.Fill = SystemColors.ActiveCaptionTextBrush;
            systemBrushesPanel.Children.Add(t);
            systemBrushesPanel.Children.Add(r);

            t = new System.Windows.Controls.TextBlock();
            t.Text = "AppWorkspace";
            r = new System.Windows.Shapes.Rectangle();
            r.Fill = SystemColors.AppWorkspaceBrush;
            systemBrushesPanel.Children.Add(t);
            systemBrushesPanel.Children.Add(r);

            t = new System.Windows.Controls.TextBlock();
            t.Text = "Control";
            r = new System.Windows.Shapes.Rectangle();
            r.Fill = SystemColors.ControlBrush;
            systemBrushesPanel.Children.Add(t);
            systemBrushesPanel.Children.Add(r);

            t = new System.Windows.Controls.TextBlock();
            t.Text = "ControlDark";
            r = new System.Windows.Shapes.Rectangle();
            r.Fill = SystemColors.ControlDarkBrush;
            systemBrushesPanel.Children.Add(t);
            systemBrushesPanel.Children.Add(r);

            t = new System.Windows.Controls.TextBlock();
            t.Text = "ControlDarkDark";
            r = new System.Windows.Shapes.Rectangle();
            r.Fill = SystemColors.ControlDarkDarkBrush;
            systemBrushesPanel.Children.Add(t);
            systemBrushesPanel.Children.Add(r);

            t = new System.Windows.Controls.TextBlock();
            t.Text = "ControlLight";
            r = new System.Windows.Shapes.Rectangle();
            r.Fill = SystemColors.ControlLightBrush;
            systemBrushesPanel.Children.Add(t);
            systemBrushesPanel.Children.Add(r);

            t = new System.Windows.Controls.TextBlock();
            t.Text = "ControlLightLight";
            r = new System.Windows.Shapes.Rectangle();
            r.Fill = SystemColors.ControlLightLightBrush;
            systemBrushesPanel.Children.Add(t);
            systemBrushesPanel.Children.Add(r);

            t = new System.Windows.Controls.TextBlock();
            t.Text = "ControlText";
            r = new System.Windows.Shapes.Rectangle();
            r.Fill = SystemColors.ControlTextBrush;
            systemBrushesPanel.Children.Add(t);
            systemBrushesPanel.Children.Add(r);

            t = new System.Windows.Controls.TextBlock();
            t.Text = "Desktop";
            r = new System.Windows.Shapes.Rectangle();
            //r.Fill = SystemColors.DesktopBrush;
            r.SetResourceReference(System.Windows.Shapes.Shape.FillProperty, System.Windows.SystemColors.DesktopBrushKey);

            systemBrushesPanel.Children.Add(t);
            systemBrushesPanel.Children.Add(r);

            t = new System.Windows.Controls.TextBlock();
            t.Text = "GradientActiveCaption";
            r = new System.Windows.Shapes.Rectangle();
            r.Fill = SystemColors.GradientActiveCaptionBrush;
            systemBrushesPanel.Children.Add(t);
            systemBrushesPanel.Children.Add(r);

            t = new System.Windows.Controls.TextBlock();
            t.Text = "GradientInactiveCaption";
            r = new System.Windows.Shapes.Rectangle();
            r.Fill = SystemColors.GradientInactiveCaptionBrush;
            systemBrushesPanel.Children.Add(t);
            systemBrushesPanel.Children.Add(r);

            t = new System.Windows.Controls.TextBlock();
            t.Text = "GrayText";
            r = new System.Windows.Shapes.Rectangle();
            r.Fill = SystemColors.GrayTextBrush;
            systemBrushesPanel.Children.Add(t);
            systemBrushesPanel.Children.Add(r);

            t = new System.Windows.Controls.TextBlock();
            t.Text = "Highlight";
            r = new System.Windows.Shapes.Rectangle();
            r.Fill = SystemColors.HighlightBrush;
            systemBrushesPanel.Children.Add(t);
            systemBrushesPanel.Children.Add(r);

            t = new System.Windows.Controls.TextBlock();
            t.Text = "HighlightText";
            r = new System.Windows.Shapes.Rectangle();
            r.Fill = SystemColors.HighlightTextBrush;
            systemBrushesPanel.Children.Add(t);
            systemBrushesPanel.Children.Add(r);

            t = new System.Windows.Controls.TextBlock();
            t.Text = "HotTrack";
            r = new System.Windows.Shapes.Rectangle();
            r.Fill = SystemColors.HotTrackBrush;
            systemBrushesPanel.Children.Add(t);
            systemBrushesPanel.Children.Add(r);

            t = new System.Windows.Controls.TextBlock();
            t.Text = "InactiveBorder";
            r = new System.Windows.Shapes.Rectangle();
            r.Fill = SystemColors.InactiveBorderBrush;
            systemBrushesPanel.Children.Add(t);
            systemBrushesPanel.Children.Add(r);

            t = new System.Windows.Controls.TextBlock();
            t.Text = "InactiveCaption";
            r = new System.Windows.Shapes.Rectangle();
            r.Fill = SystemColors.InactiveCaptionBrush;
            systemBrushesPanel.Children.Add(t);
            systemBrushesPanel.Children.Add(r);

            t = new System.Windows.Controls.TextBlock();
            t.Text = "InactiveCaptionText";
            r = new System.Windows.Shapes.Rectangle();
            r.Fill = SystemColors.InactiveCaptionTextBrush;
            systemBrushesPanel.Children.Add(t);
            systemBrushesPanel.Children.Add(r);

            t = new System.Windows.Controls.TextBlock();
            t.Text = "Info";
            r = new System.Windows.Shapes.Rectangle();
            r.Fill = SystemColors.InfoBrush;
            systemBrushesPanel.Children.Add(t);
            systemBrushesPanel.Children.Add(r);

            t = new System.Windows.Controls.TextBlock();
            t.Text = "InfoText";
            r = new System.Windows.Shapes.Rectangle();
            r.Fill = SystemColors.InfoTextBrush;
            systemBrushesPanel.Children.Add(t);
            systemBrushesPanel.Children.Add(r);

            t = new System.Windows.Controls.TextBlock();
            t.Text = "Menu";
            r = new System.Windows.Shapes.Rectangle();
            r.Fill = SystemColors.MenuBrush;
            systemBrushesPanel.Children.Add(t);
            systemBrushesPanel.Children.Add(r);

            t = new System.Windows.Controls.TextBlock();
            t.Text = "MenuBar";
            r = new System.Windows.Shapes.Rectangle();
            r.Fill = SystemColors.MenuBarBrush;
            systemBrushesPanel.Children.Add(t);
            systemBrushesPanel.Children.Add(r);

            t = new System.Windows.Controls.TextBlock();
            t.Text = "MenuHighlight";
            r = new System.Windows.Shapes.Rectangle();
            r.Fill = SystemColors.MenuHighlightBrush;
            systemBrushesPanel.Children.Add(t);
            systemBrushesPanel.Children.Add(r);

            t = new System.Windows.Controls.TextBlock();
            t.Text = "MenuText";
            r = new System.Windows.Shapes.Rectangle();
            r.Fill = SystemColors.MenuTextBrush;
            systemBrushesPanel.Children.Add(t);
            systemBrushesPanel.Children.Add(r);

            t = new System.Windows.Controls.TextBlock();
            t.Text = "ScrollBar";
            r = new System.Windows.Shapes.Rectangle();
            r.Fill = SystemColors.ScrollBarBrush;
            systemBrushesPanel.Children.Add(t);
            systemBrushesPanel.Children.Add(r);

            t = new System.Windows.Controls.TextBlock();
            t.Text = "Window";
            r = new System.Windows.Shapes.Rectangle();
            r.Fill = SystemColors.WindowBrush;
            systemBrushesPanel.Children.Add(t);
            systemBrushesPanel.Children.Add(r);

            // Try it out on a button.
            t = new System.Windows.Controls.TextBlock();
            t.Text = "WindowFrame";
            System.Windows.Controls.Button b = new System.Windows.Controls.Button();
            b.Width = 120;
            b.Height = 20;
            b.Background = SystemColors.WindowFrameBrush;
            systemBrushesPanel.Children.Add(t);
            systemBrushesPanel.Children.Add(b);

            t = new System.Windows.Controls.TextBlock();
            t.Text = "WindowText";
            b = new System.Windows.Controls.Button();
            b.Width = 120;
            b.Height = 20;
            b.Background = SystemColors.WindowTextBrush;
            systemBrushesPanel.Children.Add(t);
            systemBrushesPanel.Children.Add(b);
        }

        // Demonstrates using system colors to create gradients.
        private void listGradientExamples()
        {
            // The window style (defined in Window1.xaml) is used to
            // specify the height and width of each of the System.Windows.Shapes.Rectangles.

            System.Windows.Controls.TextBlock t = new System.Windows.Controls.TextBlock();
            t.Text = "System Color Gradient Examples";
            t.HorizontalAlignment = System.Windows.HorizontalAlignment.Left;
            t.FontWeight = System.Windows.FontWeights.Bold;
            gradientExamplePanel.Children.Add(t);

            t = new System.Windows.Controls.TextBlock();
            t.Text = "ControlDark to ControlLight";
            System.Windows.Shapes.Rectangle r = new System.Windows.Shapes.Rectangle();
            r.Fill = new System.Windows.Media.RadialGradientBrush(
                System.Windows.SystemColors.ControlDarkColor, System.Windows.SystemColors.ControlLightColor);
            gradientExamplePanel.Children.Add(t);
            gradientExamplePanel.Children.Add(r);

            t = new System.Windows.Controls.TextBlock();
            t.Text = "ControlDarkDark to ControlLightLight";
            r = new System.Windows.Shapes.Rectangle();
            r.Fill = new System.Windows.Media.LinearGradientBrush(System.Windows.SystemColors.ControlDarkDarkColor, System.Windows.SystemColors.ControlLightLightColor, 45);
            gradientExamplePanel.Children.Add(t);
            gradientExamplePanel.Children.Add(r);
            
            // Try it out on a button.
            t = new System.Windows.Controls.TextBlock();
            t.Text = "Desktop to AppWorkspace";
            System.Windows.Controls.Button b = new System.Windows.Controls.Button();
            b.Width = 120;
            b.Height = 20;
            b.Background = new System.Windows.Media.RadialGradientBrush(System.Windows.SystemColors.DesktopColor, System.Windows.SystemColors.AppWorkspaceColor);
            gradientExamplePanel.Children.Add(t);
            gradientExamplePanel.Children.Add(b);

            t = new System.Windows.Controls.TextBlock();
            t.Text = "Desktop to Control";
            b = new System.Windows.Controls.Button();
            b.Width = 120;
            b.Height = 20;
            b.Background = new System.Windows.Media.RadialGradientBrush(System.Windows.SystemColors.DesktopColor, System.Windows.SystemColors.ControlColor);
            gradientExamplePanel.Children.Add(t);
            gradientExamplePanel.Children.Add(b);
            
        }

    }
}