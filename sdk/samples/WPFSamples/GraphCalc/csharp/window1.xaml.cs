using System;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Input;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Media;
using System.Windows.Media.Animation;
using System.Windows.Media.Media3D;
using System.Windows.Shapes;
using System.Windows.Threading;

using Microsoft.Win32;
using Tests;

namespace GraphCalc
{
    public partial class Window1 : Page//IB: was Window
    {
        public Window1()
        {
            memory = null;
            lastAnswer = new ConstantExpression( 0.0 );
            trackball = null;
            viewport = null;

            VariableExpression.Define( answerKey, 0.0 );
            this.Focusable = true;
        }
        private void OnLoaded( object sender, EventArgs args )
        {
            immediate.Focus();
            viewport = new Viewport3D();
            trackball = new Trackball();
            trackball.Servants.Add( viewport );
            trackball.Attach( screen );
            trackball.Enabled = true;

            graphNone.Header = Settings.functionNone;
            graph.Header = Settings.function;
            graph2D.Header = Settings.function2D;
            graph3D.Header = Settings.function3D;
            graphOptions.Header = Settings.function;
            graphOptions2D.Header = Settings.function2D;
            graphOptions3D.Header = Settings.function3D;

            // Workaround for registry bug (key base must exist before getting values - else default won't work)
            Registry.SetValue( regSaveBase, "Version", "GraphCalc v.1.0", RegistryValueKind.String );

            xMin.Text = (string)Registry.GetValue( regSaveBase, Settings.xMinReg, Settings.xMinDefault );
            xMax.Text = (string)Registry.GetValue( regSaveBase, Settings.xMaxReg, Settings.xMaxDefault );
            yMin.Text = (string)Registry.GetValue( regSaveBase, Settings.yMinReg, Settings.yMinDefault );
            yMax.Text = (string)Registry.GetValue( regSaveBase, Settings.yMaxReg, Settings.yMaxDefault );

            xMinLabel.Text = Settings.xMin;
            xMaxLabel.Text = Settings.xMax;
            yMinLabel.Text = Settings.yMin;
            yMaxLabel.Text = Settings.yMax;

            xMin2D.Text = (string)Registry.GetValue( regSaveBase, Settings.xMin2DReg, Settings.xMin2DDefault );
            xMax2D.Text = (string)Registry.GetValue( regSaveBase, Settings.xMax2DReg, Settings.xMax2DDefault );
            yMin2D.Text = (string)Registry.GetValue( regSaveBase, Settings.yMin2DReg, Settings.yMin2DDefault );
            yMax2D.Text = (string)Registry.GetValue( regSaveBase, Settings.yMax2DReg, Settings.yMax2DDefault );
            tMin2D.Text = (string)Registry.GetValue( regSaveBase, Settings.tMin2DReg, Settings.tMin2DDefault );
            tMax2D.Text = (string)Registry.GetValue( regSaveBase, Settings.tMax2DReg, Settings.tMax2DDefault );
            tStep2D.Text = (string)Registry.GetValue( regSaveBase, Settings.tStep2DReg, Settings.tStep2DDefault );

            xMin2DLabel.Text = Settings.xMin2D;
            xMax2DLabel.Text = Settings.xMax2D;
            yMin2DLabel.Text = Settings.yMin2D;
            yMax2DLabel.Text = Settings.yMax2D;
            tMin2DLabel.Text = Settings.tMin2D;
            tMax2DLabel.Text = Settings.tMax2D;
            tStep2DLabel.Text = Settings.tStep2D;

            uMin.Text = (string)Registry.GetValue( regSaveBase, Settings.uMinReg, Settings.uMinDefault );
            uMax.Text = (string)Registry.GetValue( regSaveBase, Settings.uMaxReg, Settings.uMaxDefault );
            uGrid.Text = (string)Registry.GetValue( regSaveBase, Settings.uGridReg, Settings.uGridDefault );
            vMin.Text = (string)Registry.GetValue( regSaveBase, Settings.vMinReg, Settings.vMinDefault );
            vMax.Text = (string)Registry.GetValue( regSaveBase, Settings.vMaxReg, Settings.vMaxDefault );
            vGrid.Text = (string)Registry.GetValue( regSaveBase, Settings.vGridReg, Settings.vGridDefault );

            uMinLabel.Text = Settings.uMin;
            uMaxLabel.Text = Settings.uMax;
            uGridLabel.Text = Settings.uGrid;
            vMinLabel.Text = Settings.vMin;
            vMaxLabel.Text = Settings.vMax;
            vGridLabel.Text = Settings.vGrid;

            y.Text = (string)Registry.GetValue( regSaveBase, Settings.yReg, Settings.yDefault );
            xt.Text = (string)Registry.GetValue( regSaveBase, Settings.xtReg, Settings.xtDefault );
            yt.Text = (string)Registry.GetValue( regSaveBase, Settings.ytReg, Settings.ytDefault );
            fx.Text = (string)Registry.GetValue( regSaveBase, Settings.fxReg, Settings.fxDefault );
            fy.Text = (string)Registry.GetValue( regSaveBase, Settings.fyReg, Settings.fyDefault );
            fz.Text = (string)Registry.GetValue( regSaveBase, Settings.fzReg, Settings.fzDefault );

            yLabel.Text = Settings.y;
            xtLabel.Text = Settings.xt;
            ytLabel.Text = Settings.yt;
            fxLabel.Text = Settings.fx;
            fyLabel.Text = Settings.fy;
            fzLabel.Text = Settings.fz;
        }
        private void WindowSizeChanged( object sender, SizeChangedEventArgs args )
        {
            if ( screenCanvas.Visibility == Visibility.Visible )
            {
                switch ( lastRendered )
                {
                case Settings.function:
                        GraphScene( false );
                        break;

                case Settings.function2D:
                        GraphScene2D( false );
                        break;

                case Settings.function3D:
                        GraphScene3D( false );
                        break;
                }
            }
        }
        private void ShowFunctionNone( object sender, RoutedEventArgs args )
        {
            ShowScreenText();
        }
        private void ShowFunction( object sender, RoutedEventArgs args )
        {
            ShowFunction();
        }
        private void ShowFunction2D( object sender, RoutedEventArgs args )
        {
            ShowFunction2D();
        }
        private void ShowFunction3D( object sender, RoutedEventArgs args )
        {
            ShowFunction3D();
        }
        private void ShowOptions( object sender, RoutedEventArgs args )
        {
            ShowOptions();
        }
        private void ShowOptions2D( object sender, RoutedEventArgs args )
        {
            ShowOptions2D();
        }
        private void ShowOptions3D( object sender, RoutedEventArgs args )
        {
            ShowOptions3D();
        }

        #region Toggle what's displayed on the screen

        private void ShowScreenCanvas()
        {
            screenCanvas.Visibility = Visibility.Visible;
            screenText.Visibility = Visibility.Collapsed;
            function.Visibility = Visibility.Collapsed;
            options.Visibility = Visibility.Collapsed;
            functions2D.Visibility = Visibility.Collapsed;
            options2D.Visibility = Visibility.Collapsed;
            functions3D.Visibility = Visibility.Collapsed;
            options3D.Visibility = Visibility.Collapsed;
            immediate.Focus();
        }
        private void ShowScreenText()
        {
            screenCanvas.Visibility = Visibility.Collapsed;
            screenText.Visibility = Visibility.Visible;
            function.Visibility = Visibility.Collapsed;
            options.Visibility = Visibility.Collapsed;
            functions2D.Visibility = Visibility.Collapsed;
            options2D.Visibility = Visibility.Collapsed;
            functions3D.Visibility = Visibility.Collapsed;
            options3D.Visibility = Visibility.Collapsed;
        }
        private void ShowFunction()
        {
            screenCanvas.Visibility = Visibility.Collapsed;
            screenText.Visibility = Visibility.Collapsed;
            function.Visibility = Visibility.Visible;
            options.Visibility = Visibility.Collapsed;
            functions2D.Visibility = Visibility.Collapsed;
            options2D.Visibility = Visibility.Collapsed;
            functions3D.Visibility = Visibility.Collapsed;
            options3D.Visibility = Visibility.Collapsed;
        }
        private void ShowOptions()
        {
            screenCanvas.Visibility = Visibility.Collapsed;
            screenText.Visibility = Visibility.Collapsed;
            function.Visibility = Visibility.Collapsed;
            options.Visibility = Visibility.Visible;
            functions2D.Visibility = Visibility.Collapsed;
            options2D.Visibility = Visibility.Collapsed;
            functions3D.Visibility = Visibility.Collapsed;
            options3D.Visibility = Visibility.Collapsed;
        }
        private void ShowFunction2D()
        {
            screenCanvas.Visibility = Visibility.Collapsed;
            screenText.Visibility = Visibility.Collapsed;
            function.Visibility = Visibility.Collapsed;
            options.Visibility = Visibility.Collapsed;
            functions2D.Visibility = Visibility.Visible;
            options2D.Visibility = Visibility.Collapsed;
            functions3D.Visibility = Visibility.Collapsed;
            options3D.Visibility = Visibility.Collapsed;
        }
        private void ShowOptions2D()
        {
            screenCanvas.Visibility = Visibility.Collapsed;
            screenText.Visibility = Visibility.Collapsed;
            function.Visibility = Visibility.Collapsed;
            options.Visibility = Visibility.Collapsed;
            functions2D.Visibility = Visibility.Collapsed;
            options2D.Visibility = Visibility.Visible;
            functions3D.Visibility = Visibility.Collapsed;
            options3D.Visibility = Visibility.Collapsed;
        }
        private void ShowFunction3D()
        {
            screenCanvas.Visibility = Visibility.Collapsed;
            screenText.Visibility = Visibility.Collapsed;
            function.Visibility = Visibility.Collapsed;
            options.Visibility = Visibility.Collapsed;
            functions2D.Visibility = Visibility.Collapsed;
            options2D.Visibility = Visibility.Collapsed;
            functions3D.Visibility = Visibility.Visible;
            options3D.Visibility = Visibility.Collapsed;
        }
        private void ShowOptions3D()
        {
            screenCanvas.Visibility = Visibility.Collapsed;
            screenText.Visibility = Visibility.Collapsed;
            function.Visibility = Visibility.Collapsed;
            options.Visibility = Visibility.Collapsed;
            functions2D.Visibility = Visibility.Collapsed;
            options2D.Visibility = Visibility.Collapsed;
            functions3D.Visibility = Visibility.Collapsed;
            options3D.Visibility = Visibility.Visible;
        }

        #endregion

        #region Graphing options and validation

        private void ShowOptions( object sender, EventArgs args )
        {
            ShowOptions();
        }
        private void SaveOptions( object sender, EventArgs args )
        {
            try
            {
                ValidateOptions();
                ShowFunction();
            }
            catch ( FunctionParserException ex )
            {
                string message = ex.Message;
                if ( ex.InnerException != null )
                {
                    message += "\n" + ex.InnerException.Message;
                }
                MessageBox.Show( message );
            }
        }
        private void ValidateOptions()
        {
            ValidateOption( xMin.Text, Settings.xMinReg );
            ValidateOption( xMax.Text, Settings.xMaxReg );
            ValidateOption( yMin.Text, Settings.yMinReg );
            ValidateOption( yMax.Text, Settings.yMaxReg );

            if ( XMax == XMin )
            {
                throw new InvalidExpressionException( "\"" + Settings.xMin + "\" and \"" + Settings.xMax + "\" cannot have the same value" );
            }
            if ( XMax < XMin )
            {
                throw new InvalidExpressionException( "\"" + Settings.xMin + "\" must be less than \"" + Settings.xMax + "\"" );
            }
            if ( YMax == YMin )
            {
                throw new InvalidExpressionException( "\"" + Settings.yMin + "\" and \"" + Settings.yMax + "\" cannot have the same value" );
            }
            if ( YMax < YMin )
            {
                throw new InvalidExpressionException( "\"" + Settings.yMin + "\" must be less than \"" + Settings.yMax + "\"" );
            }
        }
        private void ResetOptions( object sender, EventArgs args )
        {
            xMin.Text = Settings.xMinDefault;
            xMax.Text = Settings.xMaxDefault;
            yMin.Text = Settings.yMinDefault;
            yMax.Text = Settings.yMaxDefault;
            ValidateOptions();
        }
        private void ShowOptions2D( object sender, EventArgs args )
        {
            ShowOptions2D();
        }
        private void SaveOptions2D( object sender, EventArgs args )
        {
            try
            {
                ValidateOptions2D();
                ShowFunction2D();
            }
            catch ( FunctionParserException ex )
            {
                string message = ex.Message;
                if ( ex.InnerException != null )
                {
                    message += "\n" + ex.InnerException.Message;
                }
                MessageBox.Show( message );
            }
        }
        private void ValidateOptions2D()
        {
            ValidateOption( xMin2D.Text, Settings.xMin2DReg );
            ValidateOption( xMax2D.Text, Settings.xMax2DReg );
            ValidateOption( yMin2D.Text, Settings.yMin2DReg );
            ValidateOption( yMax2D.Text, Settings.yMax2DReg );
            ValidateOption( tMin2D.Text, Settings.tMin2DReg );
            ValidateOption( tMax2D.Text, Settings.tMax2DReg );
            ValidateOption( tStep2D.Text, Settings.tStep2DReg );

            if ( XMax2D == XMin2D )
            {
                throw new InvalidExpressionException( "\"" + Settings.xMin2D + "\" and \"" + Settings.xMax2D + "\" cannot have the same value" );
            }
            if ( XMax2D < XMin2D )
            {
                throw new InvalidExpressionException( "\"" + Settings.xMin2D + "\" must be less than \"" + Settings.xMax2D + "\"" );
            }
            if ( YMax2D == YMin2D )
            {
                throw new InvalidExpressionException( "\"" + Settings.yMin2D + "\" and \"" + Settings.yMax2D + "\" cannot have the same value" );
            }
            if ( YMax2D < YMin2D )
            {
                throw new InvalidExpressionException( "\"" + Settings.yMin2D + "\" must be less than \"" + Settings.yMax2D + "\"" );
            }
        }
        private void ResetOptions2D( object sender, EventArgs args )
        {
            xMin2D.Text = Settings.xMin2DDefault;
            xMax2D.Text = Settings.xMax2DDefault;
            yMin2D.Text = Settings.yMin2DDefault;
            yMax2D.Text = Settings.yMax2DDefault;
            tMin2D.Text = Settings.tMin2DDefault;
            tMax2D.Text = Settings.tMax2DDefault;
            tStep2D.Text = Settings.tStep2DDefault;
            ValidateOptions2D();
        }
        private void ShowOptions3D( object sender, EventArgs args )
        {
            ShowOptions3D();
        }
        private void SaveOptions3D( object sender, EventArgs args )
        {
            try
            {
                ValidateOptions3D();
                ShowFunction3D();
            }
            catch ( FunctionParserException ex )
            {
                string message = ex.Message;
                if ( ex.InnerException != null )
                {
                    message += "\n" + ex.InnerException.Message;
                }
                MessageBox.Show( message );
            }
        }
        private void ValidateOptions3D()
        {
            ValidateOption( uMin.Text, Settings.uMinReg );
            ValidateOption( uMax.Text, Settings.uMaxReg );
            ValidateOption( uGrid.Text, Settings.uGridReg );
            ValidateOption( vMin.Text, Settings.vMinReg );
            ValidateOption( vMax.Text, Settings.vMaxReg );
            ValidateOption( vGrid.Text, Settings.vGridReg );

            if ( UGrid <= 0 )
            {
                throw new InvalidExpressionException( "\"" + Settings.uGrid + "\" must be greater than 0" );
            }
            if ( VGrid <= 0 )
            {
                throw new InvalidExpressionException( "\"" + Settings.vGrid + "\" must be greater than 0" );
            }
        }
        private void ResetOptions3D( object sender, EventArgs args )
        {
            uMin.Text = Settings.uMinDefault;
            uMax.Text = Settings.uMaxDefault;
            uGrid.Text = Settings.uGridDefault;
            vMin.Text = Settings.vMinDefault;
            vMax.Text = Settings.vMaxDefault;
            vGrid.Text = Settings.vGridDefault;
            ValidateOptions3D();
        }
        private void ValidateOption( string value, string registryName )
        {
            try
            {
                IExpression exp = FunctionParser.Parse( value ).Simplify();
                if ( !( exp is ConstantExpression ) )
                {
                    throw new InvalidExpressionException( "The input expression must be constant" );
                }
                Registry.SetValue( regSaveBase, registryName, value, RegistryValueKind.String );
            }
            catch ( FunctionParserException ex )
            {
                throw new InvalidExpressionException( "Cannot save value for \"" + registryName + "\"", ex );
            }
        }

        #endregion

        #region Immediate Input

        private void AppendSin( object sender, RoutedEventArgs args )
        {
            AppendText( "sin" );
        }
        private void AppendCos( object sender, RoutedEventArgs args )
        {
            AppendText( "cos" );
        }
        private void AppendTan( object sender, RoutedEventArgs args )
        {
            AppendText( "tan" );
        }
        private void AppendPow2( object sender, RoutedEventArgs args )
        {
            AppendText( "^2" );
        }
        private void AppendPow( object sender, RoutedEventArgs args )
        {
            AppendText( "^" );
        }
        private void AppendPi( object sender, RoutedEventArgs args )
        {
            AppendText( "pi" );
        }
        private void AppendE( object sender, RoutedEventArgs args )
        {
            AppendText( "e" );
        }
        private void AppendFoo( object sender, RoutedEventArgs args )
        {
            AppendText( "foo" );
        }
        private void AppendX( object sender, RoutedEventArgs args )
        {
            AppendText( "x" );
        }
        private void AppendT( object sender, RoutedEventArgs args )
        {
            AppendText( "t" );
        }
        private void AppendU( object sender, RoutedEventArgs args )
        {
            AppendText( "u" );
        }
        private void AppendV( object sender, RoutedEventArgs args )
        {
            AppendText( "v" );
        }
        private void Append0( object sender, RoutedEventArgs args )
        {
            AppendText( "0" );
        }
        private void Append1( object sender, RoutedEventArgs args )
        {
            AppendText( "1" );
        }
        private void Append2( object sender, RoutedEventArgs args )
        {
            AppendText( "2" );
        }
        private void Append3( object sender, RoutedEventArgs args )
        {
            AppendText( "3" );
        }
        private void Append4( object sender, RoutedEventArgs args )
        {
            AppendText( "4" );
        }
        private void Append5( object sender, RoutedEventArgs args )
        {
            AppendText( "5" );
        }
        private void Append6( object sender, RoutedEventArgs args )
        {
            AppendText( "6" );
        }
        private void Append7( object sender, RoutedEventArgs args )
        {
            AppendText( "7" );
        }
        private void Append8( object sender, RoutedEventArgs args )
        {
            AppendText( "8" );
        }
        private void Append9( object sender, RoutedEventArgs args )
        {
            AppendText( "9" );
        }
        private void AppendLParen( object sender, RoutedEventArgs args )
        {
            AppendText( "(" );
        }
        private void AppendRParen( object sender, RoutedEventArgs args )
        {
            AppendText( ")" );
        }
        private void AppendMult( object sender, RoutedEventArgs args )
        {
            AppendText( "*" );
        }
        private void AppendDiv( object sender, RoutedEventArgs args )
        {
            AppendText( "/" );
        }
        private void AppendAdd( object sender, RoutedEventArgs args )
        {
            AppendText( "+" );
        }
        private void AppendMinus( object sender, RoutedEventArgs args )
        {
            AppendText( "-" );
        }
        private void AppendDecimal( object sender, RoutedEventArgs args )
        {
            AppendText( "." );
        }
        private void AppendNegate( object sender, RoutedEventArgs args )
        {
            if ( Buffer[ Buffer.Length-1 ] != '-' )
            {
                AppendText( "-" );
            }
            else
            {
                Buffer = Buffer.Substring( 0, Buffer.Length-1 );
            }
        }
        private void AppendAns( object sender, RoutedEventArgs args )
        {
            AppendText( answerKey );
        }
        private void Off( object sender, RoutedEventArgs args )
        {
            Application.Current.Shutdown();
        }

        #endregion

        private void Clear( object sender, RoutedEventArgs args )
        {
            if ( Buffer == string.Empty )
            {
                screenText.Text = string.Empty;
            }
            Buffer = string.Empty;
        }

        #region Memory Operations

        private void MemoryAppend( object sender, RoutedEventArgs args )
        {
            if ( ComputeAnswer() )
            {
                if ( lastAnswer is ConstantExpression )
                {
                    if ( memory == null )
                    {
                        memory = lastAnswer;
                    }
                    else
                    {
                        memory = new AddExpression( memory, lastAnswer );
                        memory = memory.Simplify();
                    }
                    ScreenBuffer += memory.ToString() + "\n";
                }
                else
                {
                    ScreenBuffer += "Cannot store this value in memory\n";
                }
            }
        }
        private void MemoryRecall( object sender, RoutedEventArgs args )
        {
            if ( memory != null )
            {
                AppendText( memory.ToString() );
            }
        }
        private void MemoryClear( object sender, RoutedEventArgs args )
        {
            memory = null;
        }

        #endregion

        #region Fill in equation fields

        private void SpiralClicked( object sender, RoutedEventArgs args )
        {
            xt.Text = Settings.xtDefault;
            yt.Text = Settings.ytDefault;
            xMin2D.Text = Settings.xMin2DDefault;
            xMax2D.Text = Settings.xMax2DDefault;
            yMin2D.Text = Settings.yMin2DDefault;
            yMax2D.Text = Settings.yMax2DDefault;
            tMin2D.Text = Settings.tMin2DDefault;
            tMax2D.Text = Settings.tMax2DDefault;
            tStep2D.Text = Settings.tStep2DDefault;
            ValidateOptions2D();
        }

        private void EllipseClicked( object sender, RoutedEventArgs args )
        {
            xt.Text = "4cos(t)";
            yt.Text = "3sin(t)";
            xMin2D.Text = Settings.xMin2DDefault;
            xMax2D.Text = Settings.xMax2DDefault;
            yMin2D.Text = Settings.yMin2DDefault;
            yMax2D.Text = Settings.yMax2DDefault;
            tMin2D.Text = "0";
            tMax2D.Text = "2pi";
            tStep2D.Text = "pi/16";
            ValidateOptions2D();
        }

        private void SphereClicked( object sender, RoutedEventArgs args )
        {
            fx.Text = Settings.fxDefault;
            fy.Text = Settings.fyDefault;
            fz.Text = Settings.fzDefault;
            uMin.Text = Settings.uMinDefault;
            uMax.Text = Settings.uMaxDefault;
            uGrid.Text = Settings.uGridDefault;
            vMin.Text = Settings.vMinDefault;
            vMax.Text = Settings.vMaxDefault;
            vGrid.Text = Settings.vGridDefault;
            ValidateOptions3D();
        }

        private void ConeClicked( object sender, RoutedEventArgs args )
        {
            fx.Text = ".6(1.5-v)cos(u)";
            fy.Text = "v";
            fz.Text = ".6(1.5-v)sin(-u)";
            uMin.Text = "-pi";
            uMax.Text = "pi";
            uGrid.Text = "24";
            vMin.Text = "0";
            vMax.Text = "1.5";
            vGrid.Text = "12";
            ValidateOptions3D();
        }

        private void TorusClicked( object sender, RoutedEventArgs args )
        {
            fx.Text = "-(1+.25cos(v))cos(u)";
            fy.Text = "(1+.25cos(v))sin(u)";
            fz.Text = "-.25sin(v)";
            uMin.Text = "-pi";
            uMax.Text = "pi";
            uGrid.Text = "48";
            vMin.Text = "0";
            vMax.Text = "2pi";
            vGrid.Text = "24";
            ValidateOptions3D();
        }

        #endregion

        #region Graphing logic

        private double CanvasWidth
        {
            get
            {
                return ((FrameworkElement)screenCanvas.Parent).ActualWidth;
            }
        }
        private double CanvasHeight
        {
            get
            {
                return ((FrameworkElement)screenCanvas.Parent).ActualHeight;
            }
        }
        private System.Windows.Size CanvasSize
        {
            get
            {
                return ((UIElement)screenCanvas.Parent).RenderSize;
            }
        }
        private void Graph( object sender, RoutedEventArgs args )
        {
            try
            {
                if ( function.Visibility == Visibility.Visible )
                {
                    GraphScene( false );
                    lastRendered = Settings.function;
                }
                else if ( functions2D.Visibility == Visibility.Visible )
                {
                    GraphScene2D( false );
                    lastRendered = Settings.function2D;
                }
                else if ( functions3D.Visibility == Visibility.Visible )
                {
                    GraphScene3D( false );
                    lastRendered = Settings.function3D;
                }
                else
                {
                    ScreenBuffer += "No active functions to graph\n";
                }
            }
            catch ( FunctionParserException ex )
            {
                string message = ex.Message;
                if ( ex.InnerException != null )
                {
                    message += "\n" + ex.InnerException.Message;
                }
                MessageBox.Show( message, "Failed to graph equation" );
            }
        }

        private void GraphScene( bool legacy )
        {
            IExpression exp = Parse( y.Text, Settings.y, Settings.yReg );

            double width = CanvasWidth;
            double height = CanvasHeight;
            double offsetX = -XMin;
            double offsetY = YMax;
            double graphToCanvasX = width / ( XMax-XMin );
            double graphToCanvasY = height / ( YMax-YMin );

            PointCollection points = new PointCollection();
            for ( double x = XMin; x < XMax; x += 1 / graphToCanvasX )
            {
                VariableExpression.Define( "x", x );

                // Translate the origin based on the max/min parameters (y axis is flipped), then scale to canvas.
                double xCanvas = ( x + offsetX ) * graphToCanvasX;
                double yCanvas = ( offsetY - exp.Evaluate() ) * graphToCanvasY;

                points.Add( ClampedPoint( xCanvas, yCanvas ) );
            }
            VariableExpression.Undefine( "x" );

            screenCanvas.Children.Clear();
            axisHelper = new DrawAxisHelper( screenCanvas, CanvasSize );
            axisHelper.DrawAxes( XMin, XMax, YMin, YMax );


                Polyline graphLine = new Polyline();
                graphLine.Stroke = System.Windows.Media.Brushes.Black;
                graphLine.StrokeThickness = 1;
                graphLine.Points = points;

                screenCanvas.Children.Add( graphLine );
            
            ShowScreenCanvas();
        }
        private void GraphScene2D( bool legacy )
        {
            IExpression xExp = Parse( xt.Text, Settings.xt, Settings.xtReg );
            IExpression yExp = Parse( yt.Text, Settings.yt, Settings.ytReg );

            double width = CanvasWidth;
            double height = CanvasHeight;
            double graphToCanvasX = width / ( XMax2D-XMin2D );
            double graphToCanvasY = height / ( YMax2D-YMin2D );

            // distance from origin of graph to origin of canvas
            double offsetX = -XMin2D;
            double offsetY = YMax2D;

            PointCollection points = new PointCollection();
            for ( double t = TMin2D; t <= TMax2D + 0.000001; t += TStep2D )
            {
                VariableExpression.Define( "t", t );
                double xGraph = xExp.Evaluate();
                double yGraph = yExp.Evaluate();

                // Translate the origin based on the max/min parameters (y axis is flipped), then scale to canvas.
                double xCanvas = ( xGraph + offsetX ) * graphToCanvasX;
                double yCanvas = ( offsetY - yGraph ) * graphToCanvasY;

                points.Add( ClampedPoint( xCanvas, yCanvas ) );
            }
            VariableExpression.Undefine( "t" );

            screenCanvas.Children.Clear();
            axisHelper = new DrawAxisHelper( screenCanvas, CanvasSize );
            axisHelper.DrawAxes( XMin2D, XMax2D, YMin2D, YMax2D );


                Polyline polyLine = new Polyline();
                polyLine.Stroke = System.Windows.Media.Brushes.Black;
                polyLine.StrokeThickness = 1;
                polyLine.Points = points;
                screenCanvas.Children.Add( polyLine );

            ShowScreenCanvas();
        }
        private System.Windows.Point ClampedPoint( double x, double y )
        {
            if ( double.IsPositiveInfinity( x ) || x > CanvasWidth*2 )
            {
                x = CanvasWidth * 2;
            }
            else if ( double.IsNegativeInfinity( x ) || x < -CanvasWidth )
            {
                x = -CanvasWidth;
            }
            else if ( double.IsNaN( x ) )
            {
                x = -CanvasWidth;
            }
            if ( double.IsPositiveInfinity( y ) || y > CanvasHeight*2 )
            {
                y = CanvasHeight * 2;
            }
            else if ( double.IsNegativeInfinity( y ) || y < -CanvasHeight )
            {
                y = -CanvasHeight;
            }
            else if ( double.IsNaN( x ) )
            {
                y = -CanvasHeight;
            }
            return new System.Windows.Point( x,y );
        }
        private void GraphScene3D( bool legacy )
        {
            // We do this so we can get good error information.
            // The values return by these calls are ignored.
            Parse( fx.Text, Settings.fx, Settings.fxReg );
            Parse( fy.Text, Settings.fy, Settings.fyReg );
            Parse( fz.Text, Settings.fz, Settings.fzReg );

            PerspectiveCamera camera = new PerspectiveCamera();
            camera.Position = new Point3D( 0,0,5 );
            camera.LookDirection = new Vector3D( 0,0,-2 );
            camera.UpDirection = new Vector3D( 0,1,0 );
            camera.NearPlaneDistance = 1;
            camera.FarPlaneDistance = 100;
            camera.FieldOfView = 45;

            Model3DGroup group = null;

            if ( legacy )
            {
                FunctionWireframeModel model = new FunctionWireframeModel( fx.Text, fy.Text, fz.Text, UMin, UMax, VMin, VMax );
                group = model.CreateWireframeModel( UGrid+1, VGrid+1 );
            }
            else
            {
                group = new Model3DGroup();
                FunctionMesh mesh = new FunctionMesh( fx.Text, fy.Text, fz.Text, UMin, UMax, VMin, VMax );
                group.Children.Add(new GeometryModel3D(mesh.CreateMesh(UGrid + 1, VGrid + 1), new DiffuseMaterial(System.Windows.Media.Brushes.Blue)));
                group.Children.Add( new DirectionalLight( Colors.White, new Vector3D( -1,-1,-1 ) ) );
            }


            // Should draw axis here.

            // Save the transform added by the trackball;            
            if (viewport.Children.Count > 0){
            
                group.Transform = ((Model3DGroup)((ModelVisual3D)viewport.Children[0]).Content).Transform;
            
            }

            //<newcode>
            ModelVisual3D sceneVisual = new ModelVisual3D();
            sceneVisual.Content = group;
            viewport.Children.Clear();
            viewport.Children.Add(sceneVisual);
            //</newcode>
            
            
            //viewport.Models = group;
            viewport.Camera = camera;
            viewport.Width = CanvasWidth;
            viewport.Height = CanvasHeight;
            viewport.ClipToBounds = true;
            
            screenCanvas.Children.Clear();
            screenCanvas.Children.Add( viewport );
            ShowScreenCanvas();
        }
        private IExpression Parse( string equation, string labelName, string registryName )
        {
            try
            {
                IExpression exp = FunctionParser.Parse( equation );
                Registry.SetValue( regSaveBase, registryName, equation, RegistryValueKind.String );
                return exp;
            }
            catch ( FunctionParserException ex )
            {
                throw new InvalidExpressionException( "Error in equation: \"" + labelName + "\"", ex );
            }
        }

        #endregion

        #region Options

        // These will be verified by the options tab before the property is accessed in real code
        // An exception should never be thrown

        private double XMin
        {
            get
            {
                IExpression exp = FunctionParser.Parse( xMin.Text ).Simplify();
                return ((ConstantExpression)exp).Value;
            }
        }
        private double XMax
        {
            get
            {
                IExpression exp = FunctionParser.Parse( xMax.Text ).Simplify();
                return ((ConstantExpression)exp).Value;
            }
        }
        private double YMin
        {
            get
            {
                IExpression exp = FunctionParser.Parse( yMin.Text ).Simplify();
                return ((ConstantExpression)exp).Value;
            }
        }
        private double YMax
        {
            get
            {
                IExpression exp = FunctionParser.Parse( yMax.Text ).Simplify();
                return ((ConstantExpression)exp).Value;
            }
        }

        #endregion

        #region Options 2D

        // These will be verified by the options tab before the property is accessed in real code
        // An exception should never be thrown

        private double XMin2D
        {
            get
            {
                IExpression exp = FunctionParser.Parse( xMin2D.Text ).Simplify();
                return ((ConstantExpression)exp).Value;
            }
        }
        private double XMax2D
        {
            get
            {
                IExpression exp = FunctionParser.Parse( xMax2D.Text ).Simplify();
                return ((ConstantExpression)exp).Value;
            }
        }
        private double YMin2D
        {
            get
            {
                IExpression exp = FunctionParser.Parse( yMin2D.Text ).Simplify();
                return ((ConstantExpression)exp).Value;
            }
        }
        private double YMax2D
        {
            get
            {
                IExpression exp = FunctionParser.Parse( yMax2D.Text ).Simplify();
                return ((ConstantExpression)exp).Value;
            }
        }
        private double TMin2D
        {
            get
            {
                IExpression exp = FunctionParser.Parse( tMin2D.Text ).Simplify();
                return ((ConstantExpression)exp).Value;
            }
        }
        private double TMax2D
        {
            get
            {
                IExpression exp = FunctionParser.Parse( tMax2D.Text ).Simplify();
                return ((ConstantExpression)exp).Value;
            }
        }
        private double TStep2D
        {
            get
            {
                IExpression exp = FunctionParser.Parse( tStep2D.Text ).Simplify();
                return ((ConstantExpression)exp).Value;
            }
        }

        #endregion

        #region Options3D
        // These will be verified by the options tab before the property is accessed in real code
        // An exception should never be thrown

        private double UMin
        {
            get
            {
                IExpression exp = FunctionParser.Parse( uMin.Text ).Simplify();
                return ((ConstantExpression)exp).Value;
            }
        }
        private double UMax
        {
            get
            {
                IExpression exp = FunctionParser.Parse( uMax.Text ).Simplify();
                return ((ConstantExpression)exp).Value;
            }
        }
        private int UGrid
        {
            get
            {
                IExpression exp = FunctionParser.Parse( uGrid.Text ).Simplify();
                return (int) ((ConstantExpression)exp).Value;
            }
        }
        private double VMin
        {
            get
            {
                IExpression exp = FunctionParser.Parse( vMin.Text ).Simplify();
                return ((ConstantExpression)exp).Value;
            }
        }
        private double VMax
        {
            get
            {
                IExpression exp = FunctionParser.Parse( vMax.Text ).Simplify();
                return ((ConstantExpression)exp).Value;
            }
        }
        private int VGrid
        {
            get
            {
                IExpression exp = FunctionParser.Parse( vGrid.Text ).Simplify();
                return (int) ((ConstantExpression)exp).Value;
            }
        }

        #endregion

        private void Differentiate( object sender, RoutedEventArgs args )
        {
            ScreenBuffer += "> d/dx(" + Buffer.Trim() + ")\n";
            ShowScreenText();

            try
            {
                IExpression exp = FunctionParser.Parse( Buffer );
                exp = exp.Differentiate( "x" ).Simplify();
                ScreenBuffer += exp.ToString() + "\n";
                Buffer = string.Empty;
            }
            catch ( FunctionParserException )
            {
                ScreenBuffer += "Error!\n";
                Buffer = string.Empty;
            }
        }
        private void ComputeAnswer( object sender, RoutedEventArgs args )
        {
            if ( ComputeAnswer() )
            {
                ScreenBuffer += lastAnswer.ToString() + "\n";
                Buffer = string.Empty;
            }
        }
        private bool ComputeAnswer()
        {
            immediate.Focus();
            Buffer = Buffer.Trim();

            if ( Buffer.Length == 0 )
            {
                Buffer = answerKey;
            }

            // Print what we're about to evaluate
            ScreenBuffer += "> " + Buffer.Trim() + "\n";
            ShowScreenText();

            try
            {
                IExpression exp = FunctionParser.Parse( Buffer );
                lastAnswer = exp.Simplify();
                if ( lastAnswer is ConstantExpression )
                {
                    VariableExpression.Define( answerKey, ((ConstantExpression)lastAnswer).Value );
                }
                return true;
            }
            catch ( FunctionParserException )
            {
                ScreenBuffer += "Error!\n";
                lastAnswer = new ConstantExpression( 0 );
                VariableExpression.Define( answerKey, 0.0 );
                Buffer = string.Empty;
                return false;
            }
        }
        private void OnPreviewKeyDown( object sender, KeyEventArgs args )
        {
            switch ( args.Key )
            {
            case Key.Enter:
                    if ( immediate.IsFocused )
                    {
                        ComputeAnswer( sender, null );
                        args.Handled = true;
                    }
                    else if ( function.Visibility == Visibility.Visible )
                    {
                        GraphScene( false );
                        args.Handled = true;
                    }
                    else if ( functions2D.Visibility == Visibility.Visible )
                    {
                        GraphScene2D( false );
                        args.Handled = true;
                    }
                    else if ( functions3D.Visibility == Visibility.Visible )
                    {
                        GraphScene3D( false );
                        args.Handled = true;
                    }
                    break;
            case Key.Escape:
                    Clear( sender, null );
                    args.Handled = true;
                    break;

            case Key.D2:
                    if ( ( args.KeyboardDevice.Modifiers & ModifierKeys.Control ) != ModifierKeys.None )
                    {
                        AppendPow2( sender, null );
                    }
                    break;
            }
        }
        private void OnTextBoxGotFocus( object sender, RoutedEventArgs args )
        {
            if ( sender.Equals( screenText ) )
            {
                immediate.Focus();
                args.Handled = true;
            }
            else
            {
                focusedBox = (TextBox)sender;
            }
        }

        #region Selection/Zoom logic

        private void OnCanvasClickStart( object sender, MouseButtonEventArgs args )
        {
            switch ( lastRendered )
            {
            case Settings.function:
            case Settings.function2D:
                    selectionStarted = true;
                    selectionStart = args.GetPosition( screenCanvas );
                    selection.Width = 0;
                    selection.Height = 0;
                    selection.Visibility = Visibility.Visible;
                    break;

            case Settings.function3D:
                    break;
            }
        }
        private void OnCanvasClickFinish( object sender, MouseButtonEventArgs args )
        {
            if ( selectionStarted )
            {
                Rect zoomIn = new Rect( selectionStart, args.GetPosition( screenCanvas ) );
                selection.Visibility = Visibility.Collapsed;
                if ( zoomIn.Width <= 1 || zoomIn.Height <= 1 )
                {
                    return;
                }
                if ( lastRendered == Settings.function )
                {
                    ZoomViewportTo( zoomIn );
                }
                else if ( lastRendered == Settings.function2D )
                {
                    ZoomViewport2DTo( zoomIn );
                }
                selectionStarted = false;
            }
        }
        private void OnCanvasMouseMove( object sender, MouseEventArgs args )
        {
            if ( selectionStarted )
            {
                Rect rect = new Rect( selectionStart, args.GetPosition( screenCanvas ) );
                selection.RenderTransform = new TranslateTransform( rect.X, rect.Y );
                selection.Width = rect.Width;
                selection.Height = rect.Height;
            }
        }
        private void OnCanvasRightClick( object sender, MouseEventArgs args )
        {
            if ( selectionStarted )
            {
                selectionStarted = false;
                selection.Visibility = Visibility.Collapsed;
            }
            else
            {
                System.Windows.Size canvasSize = CanvasSize;
                double xBorder = canvasSize.Width / 2;
                double yBorder = canvasSize.Height / 2;

                Rect zoomOut = new Rect( -xBorder,-yBorder,xBorder*4,yBorder*4 );
                if ( lastRendered == Settings.function )
                {
                    ZoomViewportTo( zoomOut );
                }
                else if ( lastRendered == Settings.function2D )
                {
                    ZoomViewport2DTo( zoomOut );
                }
            }
        }
        private void ZoomViewportTo( Rect canvasSelection )
        {
            System.Windows.Size canvasSize = CanvasSize;
            Vector selectionOffset = new Vector( canvasSelection.X, canvasSelection.Y );
            Vector selectionScale = new Vector( canvasSelection.Width / canvasSize.Width, canvasSelection.Height / canvasSize.Height );

            System.Windows.Size graphSize = new System.Windows.Size(XMax - XMin, YMax - YMin);
            Vector canvasToGraphScale = new Vector( graphSize.Width / canvasSize.Width, graphSize.Height / canvasSize.Height );
            graphSize.Width *= selectionScale.X;
            graphSize.Height *= selectionScale.Y;
            Vector graphOffset = new Vector( selectionOffset.X * canvasToGraphScale.X, selectionOffset.Y * canvasToGraphScale.Y );
            Rect newViewport = new Rect( XMin + graphOffset.X, YMax - graphOffset.Y, graphSize.Width, graphSize.Height );
            xMin.Text = newViewport.Left.ToString();
            xMax.Text = newViewport.Right.ToString();
            yMax.Text = newViewport.Top.ToString();
            yMin.Text = ( newViewport.Top - graphSize.Height ).ToString();
            ValidateOptions();
            GraphScene( false );
        }
        private void ZoomViewport2DTo( Rect canvasSelection )
        {
            System.Windows.Size canvasSize = CanvasSize;
            Vector selectionOffset = new Vector( canvasSelection.X, canvasSelection.Y );
            Vector selectionScale = new Vector( canvasSelection.Width / canvasSize.Width, canvasSelection.Height / canvasSize.Height );

            System.Windows.Size graphSize = new System.Windows.Size(XMax2D - XMin2D, YMax2D - YMin2D);
            Vector canvasToGraphScale = new Vector( graphSize.Width / canvasSize.Width, graphSize.Height / canvasSize.Height );
            graphSize.Width *= selectionScale.X;
            graphSize.Height *= selectionScale.Y;
            Vector graphOffset = new Vector( selectionOffset.X * canvasToGraphScale.X, selectionOffset.Y * canvasToGraphScale.Y );
            Rect newViewport = new Rect( XMin2D + graphOffset.X, YMax2D - graphOffset.Y, graphSize.Width, graphSize.Height );
            xMin2D.Text = newViewport.Left.ToString();
            xMax2D.Text = newViewport.Right.ToString();
            yMax2D.Text = newViewport.Top.ToString();
            yMin2D.Text = ( newViewport.Top - graphSize.Height ).ToString();
            ValidateOptions2D();
            GraphScene2D( false );
        }

        #endregion

        private void AppendText( string text )
        {
            if ( text.Length == 0 )
            {
                return;
            }
            if ( Buffer.Length == 0 )
            {
                if ( text == "+" || text == "-" || text == "*" || text == "/" )
                {
                    text = answerKey + text;
                }
            }
            Buffer += text;
        }

        private string Buffer
        {
            get
            {
                return focusedBox.Text;
            }
            set
            {
                focusedBox.Text = value;
                focusedBox.Focus();
                focusedBox.Select( value.Length, 0 );
            }
        }
        private string ScreenBuffer
        {
            get
            {
                return screenText.Text;
            }
            set
            {
                screenText.Text = value;
                screenText.ScrollToEnd();
            }
        }

        private IExpression lastAnswer;
        private IExpression memory;
        private Trackball trackball;
        private Viewport3D viewport;
        private TextBox focusedBox;
        private string lastRendered;
        private bool selectionStarted;
        private System.Windows.Point selectionStart;
        private DrawAxisHelper axisHelper;

        private string regSaveBase = Registry.CurrentUser.Name + @"\Software\GraphCalc\Settings";
        private const string answerKey = "ans";
    }
}