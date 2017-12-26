using System;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.ComponentModel;
using System.Globalization;
using System.Text;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Ink;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Shapes;


namespace InkCanvasEditingModes
{
    /// <summary>
    /// EditingMode Ids
    /// </summary>
    public enum EditingModeId
    {
        Pen1,
        Pen2,
        Pen3,
        Pen4,
        Pen5,
        Highlighter1,
        Highlighter2,
        Highlighter3,
        Highlighter4,
        Highlighter5,
        PointEraser1,
        PointEraser2,
        PointEraser3,
        CurrentPen,
        CurrentHighlighter,
        CurrentEraser,
        SelectionTool,
        StrokeEraser,
        None,
    }

    /// <summary>
    /// Option Ids
    /// </summary>
    public enum OptionId
    {
        InkAndGesture,
        Ink,
        GestureOnly,
    }

    /// <summary>
    /// A converter used by Option menu items.
    /// If the current ink mode is equal to the value expected by an item, Convert method will return true.
    /// Then the item's check mark can be shown or hidden accordingly
    /// </summary>
    public class InkEditingModeOptionIsCheckedConverter : IValueConverter
    {
        #region IValueConverter Members

        public object Convert(object value, Type targetType, object parameter, System.Globalization.CultureInfo culture)
        {
            if (value == null) return null;

            // Get the current ink mode which is one of the values - OptionId.Ink, OptionId.GestureOnly and OptionId.InkAndGesture
            OptionId currentOptionId = (OptionId)value;

            // Get the expected Id which the menu item expects.
            OptionId expectedOptionId = (OptionId)parameter;

            // return true if the active option id is same as the item's
            return expectedOptionId == currentOptionId;
        }

        public object ConvertBack(object value, Type targetType, object parameter, System.Globalization.CultureInfo culture)
        {
            // No value needs to be converted back.
            return null;
        }

        #endregion
    }
    
    /// <summary>
    /// A converter used by the various controls (Tools items, Selection button, Pen/Hight/Eraser DropDownButton and items
    /// If the bound SampleWindow property equals to the value expected by the control, Convert method will return true.
    /// Then the item's check mark can be shown or hidden accordingly
    /// </summary>
    public class EditingModeIsCheckedConverter : IValueConverter
    {
        #region IValueConverter Members

        public object Convert(object value, Type targetType, object parameter, System.Globalization.CultureInfo culture)
        {
            if (value == null) return null;

            // Get the bound property value.
            EditingModeId currentEditingModeId = (EditingModeId)value;
            // Get the id which the control expects
            EditingModeId expectedEditingModeId = (EditingModeId)parameter;

            if (expectedEditingModeId == currentEditingModeId)
            {
                // Returns true if the two values are same.
                return true;
            }
            else if (expectedEditingModeId == EditingModeId.CurrentPen
                && (currentEditingModeId >= EditingModeId.Pen1 && currentEditingModeId <= EditingModeId.Pen5))
            {
                // The pen toolbar button should be checked whenever any pen mode is active.
                return true;
            }
            else if (expectedEditingModeId == EditingModeId.CurrentHighlighter
               && (currentEditingModeId >= EditingModeId.Highlighter1 && currentEditingModeId <= EditingModeId.Highlighter5))
            {
                // The highlighter toolbar button should be checked whenever any highlighter mode is active.
                return true;
            }
            else if (expectedEditingModeId == EditingModeId.CurrentEraser
                && ((currentEditingModeId >= EditingModeId.PointEraser1 && currentEditingModeId <= EditingModeId.PointEraser3)
                    || currentEditingModeId == EditingModeId.StrokeEraser))
            {
                // The eraser toolbar button should be checked whenever any eraser mode is active.
                return true;
            }

            return false;
        }

        public object ConvertBack(object value, Type targetType, object parameter, System.Globalization.CultureInfo culture)
        {
            // No reverse conversion. Don't update the source
            return Binding.DoNothing;
        }

        #endregion
    }

    /// <summary>
    /// A converter which returns the corresponding DrawingAttributes object based on a pen/highlighter id.
    /// The DrawingAttributes object can be visualized by our DataTemplates.
    /// </summary>
    public class EditingModeIdToDrawingAttributesConverter : IValueConverter
    {
        #region IValueConverter Members

        public object Convert(object value, Type targetType, object parameter, System.Globalization.CultureInfo culture)
        {
            if (value == null) return null;
            
            // Get the pre-defined DrawingAttributes collection
            ObservableCollection<DrawingAttributes> predefinedDrawingAttributes = value as ObservableCollection<DrawingAttributes>;
            // Get the pen/highlighter id associated to this control.
            EditingModeId expectedEditingModeId = (EditingModeId)parameter;

            if (expectedEditingModeId >= EditingModeId.Pen1 && expectedEditingModeId <= EditingModeId.Highlighter5)
            {
                // return the specified DrawingAttributes
                return predefinedDrawingAttributes[(int)expectedEditingModeId];
            }

            return null;
        }

        public object ConvertBack(object value, Type targetType, object parameter, System.Globalization.CultureInfo culture)
        {
            // Don't update the source
            return Binding.DoNothing;
        }

        #endregion
    }

    /// <summary>
    /// A converter which returns the string name of a DrawingAttributes. This string doesn't contain the color name.
    /// </summary>
    public class DrawingAttributesDropDownItemCaptionConverter : IValueConverter
    {
        #region IValueConverter Members

        public virtual object Convert(object value, Type targetType, object parameter, System.Globalization.CultureInfo culture)
        {
            if (value == null) return null;

            // Get the DrawingAttributes object
            DrawingAttributes da = (DrawingAttributes)value;

            // Returns the built-in string name.
            string name = string.Empty;
            if (!da.IsHighlighter)
            {
                string strSize = string.Empty;
                string strShape = string.Empty;
                if (da.Height == 1.44)
                {
                    strSize = "Fine";
                }
                else if (da.Height == 0.96)
                {
                    strSize = "Very Fine";
                }
                else if (da.Height == 2.88)
                {
                    strSize = "Medium";
                }
                else if (da.Height == 8.64)
                {
                    strSize = "Marker (2mm)";
                }

                if (da.StylusTip == StylusTip.Rectangle)
                {
                    strShape = "Chisel";
                }
                else
                {
                    strShape = "Point";
                }

                return string.Format("{0} {1}", strSize, strShape);
            }
            else
            {
                string strSize = string.Empty;
                if (da.Height == 26.4)
                {
                    strSize = "Medium";
                }
                else if (da.Height == 39.648)
                {
                    strSize = "Thick";
                }
                else if (da.Height == 47.232)
                {
                    strSize = "Extra Thick";
                }

                return strSize;
            }
        }

        public object ConvertBack(object value, Type targetType, object parameter, System.Globalization.CultureInfo culture)
        {
            // Don't update the source
            return Binding.DoNothing;
        }

        #endregion
    }

    /// <summary>
    /// A converter which returns the string name of a DrawingAttributes. This string also contains the color name.
    /// </summary>
    public class DrawingAttributesMenuCaptionConverter : DrawingAttributesDropDownItemCaptionConverter
    {
        public override object Convert(object value, Type targetType, object parameter, System.Globalization.CultureInfo culture)
        {
            if (value == null) return null;

            // Call base to get the string name
            string caption = (string)(base.Convert(value, targetType, parameter, culture));

            // Add the color suffix
            DrawingAttributes da = value as DrawingAttributes;
            if (da.Color == Colors.Black)
            {
                caption += " (Black)";
            }
            else if (da.Color == Colors.Indigo)
            {
                caption += " (Indigo)";
            }
            else if (da.Color == Colors.Red)
            {
                caption += " (Red)";
            }
            else if (da.Color == Colors.Yellow)
            {
                caption += " (Yellow)";
            }
            else if (da.Color == Colors.LightGreen)
            {
                caption += " (Bright Green)";
            }
            else if (da.Color == Colors.Turquoise)
            {
                caption += " (Turquoise)";
            }
            else if (da.Color == Colors.Pink)
            {
                caption += " (Pink)";
            }
            else if (da.Color == Color.FromArgb(0xFF, 0xFF, 0x99, 0x00))
            {
                caption += " (Light Orange)";
            }
            else
            {
                caption += " (" + da.Color.ToString() + ")";
            }

            return caption;
        }
    }

    /// <summary>
    /// A converter which returns the string name for a given eraser mode id.
    /// </summary>
    public class EraserNameConverter : IValueConverter
    {
        #region IValueConverter Members

        public object Convert(object value, Type targetType, object parameter, CultureInfo culture)
        {
            if (value == null) return null;

            string name = string.Empty;

            // Returns the string for a given eraser id.
            switch ((EditingModeId)value)
            {
                case EditingModeId.StrokeEraser:
                    name = "Stroke";
                    break;
                case EditingModeId.PointEraser1:
                    name = "Small";
                    break;
                case EditingModeId.PointEraser2:
                    name = "Medium";
                    break;
                case EditingModeId.PointEraser3:
                    name = "Large";
                    break;
            }

            return name;
        }

        public object ConvertBack(object value, Type targetType, object parameter, CultureInfo culture)
        {
            return null;
        }

        #endregion
    }

    /// <summary>
    /// A multivalue converter which returns the DrawingAttributes of the current active pen or the active highlighter.
    /// The DrawingAttributes will be visualized on the pen toolbar button or the highlighter toolbar button.
    /// </summary>
    public class CurrentDrawingAttributesConverter : IMultiValueConverter
    {
        public object Convert(object[] values, Type targetType, object parameter, CultureInfo culture)
        {
            if (values == null || values.Length == 0) return null;

            for (int i = 0, count = values.Length; i < count; i++)
            {
                // Some source binding could have no value being set yet. If it's the case, return UnsetValue
                // to indicate no value produced by the converter.
                if (values[i] == DependencyProperty.UnsetValue)
                {
                    return DependencyProperty.UnsetValue;
                }
            }

            // Get the active pen or highlighter id
            EditingModeId currentActivePenOrHighlighterModeId = (EditingModeId)(values[0]);
            // Get the collection.
            ObservableCollection<DrawingAttributes> predefinedDrawingAttributes = values[1] as ObservableCollection<DrawingAttributes>;

            // Return the DrawingAttributes object.
            return predefinedDrawingAttributes[(int)currentActivePenOrHighlighterModeId];
        }

        public object[] ConvertBack(object value, Type[] targetTypes, object parameter, CultureInfo culture)
        {
            return null;
        }
    }

    /// <summary>
    /// Interaction logic for SampleWindow.xaml
    /// </summary>
    public partial class SampleWindow : System.Windows.Window
    {
        /// <summary>
        /// Initialization.
        /// </summary>
        protected override void OnInitialized(EventArgs e)
        {
            base.OnInitialized(e);
            _cmdStack = new CommandStack(MyInkCanvas.Strokes);
        }
        #region Constructor

        /// <summary>
        /// Constructor
        /// </summary>
        public SampleWindow()
        {
            InitializeComponent();

            // Initialize Ink Mode option list
            _inkModeOptions = new List<InkCanvasEditingMode>(
                new InkCanvasEditingMode[]{
                    InkCanvasEditingMode.InkAndGesture,
                    InkCanvasEditingMode.Ink,
                    InkCanvasEditingMode.GestureOnly,
                });

            // Initiliaze the InkCanvas
            ChangeToInkMode(EditingModeId.Pen1);

            // Setup the command bindings
            this.CommandBindings.Add(new CommandBinding(ApplicationCommands.Close,
                                        new ExecutedRoutedEventHandler(OnExecutedCommands),
                                        new CanExecuteRoutedEventHandler(OnCanExecutedCommands)));

            this.CommandBindings.Add(new CommandBinding(SampleWindow.ClearCommand,
                                        new ExecutedRoutedEventHandler(OnExecutedCommands),
                                        new CanExecuteRoutedEventHandler(OnCanExecutedCommands)));

            this.CommandBindings.Add(new CommandBinding(SampleWindow.EditingCommand,
                                        new ExecutedRoutedEventHandler(OnExecutedCommands),
                                        new CanExecuteRoutedEventHandler(OnCanExecutedCommands)));

            this.CommandBindings.Add(new CommandBinding(SampleWindow.OptionCommand,
                                        new ExecutedRoutedEventHandler(OnExecutedCommands),
                                        new CanExecuteRoutedEventHandler(OnCanExecutedCommands)));

            this.CommandBindings.Add(new CommandBinding(ApplicationCommands.Undo,
                                        new ExecutedRoutedEventHandler(OnExecutedCommands),
                                        new CanExecuteRoutedEventHandler(OnCanExecutedCommands)));

            this.CommandBindings.Add(new CommandBinding(ApplicationCommands.Redo,
                                        new ExecutedRoutedEventHandler(OnExecutedCommands),
                                        new CanExecuteRoutedEventHandler(OnCanExecutedCommands)));

            MyInkCanvas.Gesture += new InkCanvasGestureEventHandler(OnInkCanvasGesture);
            MyHyperline.Click += new RoutedEventHandler(OnHyperlinkClick);

            MyInkCanvas.Strokes.StrokesChanged += new StrokeCollectionChangedEventHandler(Strokes_StrokesChanged);
            MyInkCanvas.SelectionMoving += new InkCanvasSelectionEditingEventHandler(MyInkCanvas_SelectionMovingOrResizing);
            MyInkCanvas.SelectionResizing += new InkCanvasSelectionEditingEventHandler(MyInkCanvas_SelectionMovingOrResizing);
            MyInkCanvas.MouseUp += new MouseButtonEventHandler(MyInkCanvas_MouseUp);
        }

        #endregion Constructor

        #region Public Commands

        /// <summary>
        /// The Clear Command
        /// </summary>
        public static readonly RoutedCommand ClearCommand = new RoutedCommand("Clear", typeof(SampleWindow));

        /// <summary>
        /// The Editing Command
        /// </summary>
        public static readonly RoutedCommand EditingCommand = new RoutedCommand("Editing", typeof(SampleWindow));

        /// <summary>
        /// The Option Command
        /// </summary>
        public static readonly RoutedCommand OptionCommand = new RoutedCommand("Option", typeof(SampleWindow));

        #endregion Public Commands

        #region Public Properties

        /// <summary>
        /// Returns the built-in DrawingAttributes collection
        /// </summary>
        public ObservableCollection<DrawingAttributes> DrawingAttributesCollection
        {
            get
            {
                if (_predefinedDrawingAttributes == null)
                {
                    // Initialize the DrawingAttributes list
                    _predefinedDrawingAttributes = new ObservableCollection<DrawingAttributes>();

                    DrawingAttributes[] drawingAttributesList = (DrawingAttributes[])(this.FindResource("MyPenDrawingAttributes"));
                    int count = drawingAttributesList.Length;
                    for (int i = 0; i < count; i++)
                    {
                        _predefinedDrawingAttributes.Add(drawingAttributesList[i]);
                    }

                    drawingAttributesList = (DrawingAttributes[])(this.FindResource("MyHighlighterDrawingAttributes"));
                    count = drawingAttributesList.Length;
                    for (int i = 0; i < count; i++)
                    {
                        _predefinedDrawingAttributes.Add(drawingAttributesList[i]);
                    }
                }
                return _predefinedDrawingAttributes;
            }
        }

        /// <summary>
        /// Returns the built-in PointEraser Collection
        /// </summary>
        public ObservableCollection<StylusShape> PointEraserShapeCollection
        {
            get
            {
                if (_predefinedStylusShapes == null)
                {
                    // Initialize EraserShape list
                    _predefinedStylusShapes = new ObservableCollection<StylusShape>();

                    // Small
                    _predefinedStylusShapes.Add(new RectangleStylusShape(6, 6));

                    // Medium
                    _predefinedStylusShapes.Add(new RectangleStylusShape(18, 18));

                    // Large
                    _predefinedStylusShapes.Add(new RectangleStylusShape(32, 32));

                }

                return _predefinedStylusShapes;
            }
        }

        /// <summary>
        /// The DependencyProperty for the EditingMode property.
        /// </summary>
        public static readonly DependencyProperty EditingModeProperty =
                DependencyProperty.Register(
                        "EditingMode",
                        typeof(EditingModeId),
                        typeof(SampleWindow),
                        new FrameworkPropertyMetadata(
                                EditingModeId.Pen1,
                                new PropertyChangedCallback(OnEditingModeChanged)));

        /// <summary>
        /// Gets/Sets the EditingMode property.
        /// </summary>
        public EditingModeId EditingMode
        {
            get { return (EditingModeId)GetValue(EditingModeProperty); }
            set { SetValue(EditingModeProperty, value); }
        }

        /// <summary>
        /// EditingMode property change callback handler 
        /// </summary>
        /// <param name="d"></param>
        /// <param name="e"></param>
        private static void OnEditingModeChanged(DependencyObject d, DependencyPropertyChangedEventArgs e)
        {
            SampleWindow myWindow = (SampleWindow)d;
            myWindow.ChangeEditingMode((EditingModeId)e.NewValue);
        }

        /// <summary>
        /// A private DependencyPropertyKey is used for the read-only CurrentInkModeOption DependencyProperty
        /// </summary>
        private static readonly DependencyPropertyKey CurrentInkModeOptionPropertyKey =
                DependencyProperty.RegisterReadOnly("CurrentInkModeOption",
                    typeof(OptionId),
                    typeof(SampleWindow), new FrameworkPropertyMetadata(OptionId.InkAndGesture));

        /// <summary>
        /// The read-only CurrentInkModeOption Dependency Property
        /// </summary>
        public static readonly DependencyProperty CurrentInkModeOptionProperty =
                CurrentInkModeOptionPropertyKey.DependencyProperty;

        /// <summary>
        /// CLR getter for CurrentPenMode
        /// </summary>
        public OptionId CurrentInkModeOption
        {
            get
            {
                return (OptionId)GetValue(CurrentInkModeOptionProperty);
            }
        }

        /// <summary>
        /// A private DependencyPropertyKey is used for the read-only CurrentPenMode DependencyProperty
        /// </summary>
        private static readonly DependencyPropertyKey CurrentPenModePropertyKey =
                DependencyProperty.RegisterReadOnly("CurrentPenMode",
                    typeof(EditingModeId),
                    typeof(SampleWindow), new FrameworkPropertyMetadata(EditingModeId.Pen1));

        /// <summary>
        /// The read-only CurrentPenMode Dependency Property
        /// </summary>
        public static readonly DependencyProperty CurrentPenModeProperty =
                CurrentPenModePropertyKey.DependencyProperty;

        /// <summary>
        /// CLR getter for CurrentPenMode
        /// </summary>
        public EditingModeId CurrentPenMode
        {
            get
            {
                return (EditingModeId)GetValue(CurrentPenModeProperty);
            }
        }

        /// <summary>
        /// A private DependencyPropertyKey is used for the read-only CurrentHighlighterMode DependencyProperty
        /// </summary>
        private static readonly DependencyPropertyKey CurrentHighlighterModePropertyKey =
                DependencyProperty.RegisterReadOnly("CurrentHighlighterMode",
                    typeof(EditingModeId),
                    typeof(SampleWindow), new FrameworkPropertyMetadata(EditingModeId.Highlighter1));

        /// <summary>
        /// The read-only CurrentHighlighterMode Dependency Property
        /// </summary>
        public static readonly DependencyProperty CurrentHighlighterModeProperty =
                CurrentHighlighterModePropertyKey.DependencyProperty;

        /// <summary>
        /// CLR getter for CurrentHighlighterMode
        /// </summary>
        public EditingModeId CurrentHighlighterMode
        {
            get
            {
                return (EditingModeId)GetValue(CurrentHighlighterModeProperty);
            }
        }

        /// <summary>
        /// A private DependencyPropertyKey is used for the read-only CurrentEraserMode DependencyProperty
        /// </summary>
        private static readonly DependencyPropertyKey CurrentEraserModePropertyKey =
                DependencyProperty.RegisterReadOnly("CurrentEraserMode",
                    typeof(EditingModeId),
                    typeof(SampleWindow), new FrameworkPropertyMetadata(EditingModeId.StrokeEraser));

        /// <summary>
        /// The read-only CurrentEraserMode Dependency Property
        /// </summary>
        public static readonly DependencyProperty CurrentEraserModeProperty =
                CurrentEraserModePropertyKey.DependencyProperty;

        /// <summary>
        /// CLR getter for CurrentEraserMode
        /// </summary>
        public EditingModeId CurrentEraserMode
        {
            get
            {
                return (EditingModeId)GetValue(CurrentEraserModeProperty);
            }
        }

        #endregion Public Properties

        #region Private Methods

        /// <summary>
        /// A class handler which handles the various commands.
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private static void OnExecutedCommands(object sender, ExecutedRoutedEventArgs e)
        {
            SampleWindow myWindow = (SampleWindow)sender;

            if (e.Command == ApplicationCommands.Close)
            {
                // Close the main window.
                myWindow.Close();
            }
            else if (e.Command == SampleWindow.ClearCommand)
            {
                // Clear the current strokes.
                myWindow.ClearStrokes();
            }
            else if (e.Command == SampleWindow.EditingCommand)
            {
                EditingModeId newEditingMode = (EditingModeId)e.Parameter;

                if (newEditingMode == EditingModeId.CurrentPen)
                {
                    // The Pen toolbar button is clicked. 
                    // We will switch to the mode with the active pen setting.
                    newEditingMode = myWindow.CurrentPenMode;
                }
                else if (newEditingMode == EditingModeId.CurrentHighlighter)
                {
                    // The Highlighter toolbar button is clicked. 
                    // We will switch to the mode with the active highlighter setting.
                    newEditingMode = myWindow.CurrentHighlighterMode;
                }
                else if (newEditingMode == EditingModeId.CurrentEraser)
                {
                    // The Eraser toolbar button is clicked. 
                    // We will switch to the mode with the active eraser setting.
                    newEditingMode = myWindow.CurrentEraserMode;
                }

                // Switch to the specified mode.
                myWindow.EditingMode = newEditingMode;
            }
            else if (e.Command == SampleWindow.OptionCommand)
            {
                // Switch to the specified ink mode (Ink, GestureOnly or InkAndGesture).
                myWindow.ChangeInkModeOption((OptionId)e.Parameter);
            }
            else if (e.Command == ApplicationCommands.Undo)
            {
                myWindow.Undo(sender, e);
            }
            else if (e.Command == ApplicationCommands.Redo)
            {
                myWindow.Redo(sender, e);
            }
        }

        /// <summary>
        /// A handler which handles the enabled status for a command.
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private static void OnCanExecutedCommands(object sender, CanExecuteRoutedEventArgs e)
        {
            SampleWindow myWindow = (SampleWindow)sender;

            // By default, enable all commands.
            e.CanExecute = true;

            if (e.Command == SampleWindow.ClearCommand)
            {
                // Enable Clear command only if there is a non-empty stroke collection.
                e.CanExecute = myWindow.MyInkCanvas.Strokes.Count != 0;
            }
            else if (e.Command == ApplicationCommands.Undo)
            {
                // Enable only if there are items on the command stack
                e.CanExecute = myWindow._cmdStack.CanUndo;
            }
            else if (e.Command == ApplicationCommands.Redo)
            {
                // Enable only if there are items on the command stack
                e.CanExecute = myWindow._cmdStack.CanRedo;
            }
        }

        /// <summary>
        /// A handler which handles the Gesture events.
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void OnInkCanvasGesture(object sender, InkCanvasGestureEventArgs e)
        {
            ReadOnlyCollection<GestureRecognitionResult> results = e.GetGestureRecognitionResults();

            if (results.Count != 0 && results[0].RecognitionConfidence == RecognitionConfidence.Strong)
            {
                if (CurrentInkModeOption == OptionId.GestureOnly)
                {
                    // Show gesture feedback in the GestureOnly mode
                    GestureResultAdorner.ShowMessage(results[0].ApplicationGesture.ToString(), e.Strokes.GetBounds().TopLeft);
                }
                else
                {
                    // In InkAndGesture mode, and if not using highlighter, if a ScratchOut
                    // gesture is detected then remove the underlying strokes
                    if (results[0].ApplicationGesture == ApplicationGesture.ScratchOut 
                        && !e.Strokes[0].DrawingAttributes.IsHighlighter)
                    {
                        StrokeCollection strokesToRemove = MyInkCanvas.Strokes.HitTest(e.Strokes.GetBounds(), 10);
                        if (strokesToRemove.Count != 0)
                        {
                            MyInkCanvas.Strokes.Remove(strokesToRemove);
                        }
                    }
                    else
                    {
                        // Otherwise cancel the gesture.
                        e.Cancel = true;
                    }
                }
            }
            else
            {
                // Cancel the gesture.
                e.Cancel = true;
            }
        }

        /// <summary>
        /// Hyperlink Click event handler
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void OnHyperlinkClick(object sender, RoutedEventArgs e)
        {
            // Launch the brower with the gesture information url.
            System.Diagnostics.Process.Start(GestureInfoUrl);
        }

        /// <summary>
        /// Clear the stroke collection on the InkCanvas
        /// </summary>
        private void ClearStrokes()
        {
            MyInkCanvas.Strokes.Clear();
        }

        /// <summary>
        /// Change the editing mode.
        /// </summary>
        /// <param name="editingModeId"></param>
        private void ChangeEditingMode(EditingModeId editingModeId)
        {
            switch (editingModeId)
            {
                case EditingModeId.Pen1:
                case EditingModeId.Pen2:
                case EditingModeId.Pen3:
                case EditingModeId.Pen4:
                case EditingModeId.Pen5:
                case EditingModeId.Highlighter1:
                case EditingModeId.Highlighter2:
                case EditingModeId.Highlighter3:
                case EditingModeId.Highlighter4:
                case EditingModeId.Highlighter5:
                    {
                        // Change to the ink mode with the correct pen or highlighter setting.
                        ChangeToInkMode(editingModeId);
                        break;
                    }
                case EditingModeId.PointEraser1:
                case EditingModeId.PointEraser2:
                case EditingModeId.PointEraser3:
                case EditingModeId.StrokeEraser:
                    {
                        // Change to the erase mode with the correct eraser.
                        ChangeToEraseMode(editingModeId);
                        break;
                    }
                case EditingModeId.SelectionTool:
                    {
                        // Change to the select mode.
                        EnsureEditingMode(InkCanvasEditingMode.Select);
                        break;
                    }
                case EditingModeId.None:
                    {
                        // Change to the none mode.
                        EnsureEditingMode(InkCanvasEditingMode.None);
                        break;
                    }
            }
        }

        /// <summary>
        /// Change the ink mode option
        /// </summary>
        /// <param name="option"></param>
        private void ChangeInkModeOption(OptionId option)
        {
            // Change the option setting.
            SetValue(CurrentInkModeOptionPropertyKey, option);

            // If the InkCanvas is under one of any inking mode, we need to update the new inking mode at once.
            InkCanvasEditingMode currentEditingMode = MyInkCanvas.EditingMode;
            if (currentEditingMode == InkCanvasEditingMode.Ink
                || currentEditingMode == InkCanvasEditingMode.InkAndGesture
                || currentEditingMode == InkCanvasEditingMode.GestureOnly)
            {
                EnsureEditingMode(_inkModeOptions[CurrentInkModeOption - OptionId.InkAndGesture]);
            }
        }

        /// <summary>
        /// Change to the ink mode with a pen/highlighter setting.
        /// </summary>
        /// <param name="editingModeId"></param>
        private void ChangeToInkMode(EditingModeId editingModeId)
        {
            // Ensure the ink Mode based on the current option.
            InkCanvasEditingMode currentInkMode = _inkModeOptions[CurrentInkModeOption - OptionId.InkAndGesture];
            EnsureEditingMode(currentInkMode);

            // Get the Drawing Attributes which is associated to the pen/highlighter setting.
            // Then update the InkCanvas' DefaultDrawingAttributes property.
            DrawingAttributes da = DrawingAttributesCollection[editingModeId - EditingModeId.Pen1];
            MyInkCanvas.DefaultDrawingAttributes = da;

            // Update the read-only DependencyProperties so that the UI can react to the change correspondingly.
            if (editingModeId >= EditingModeId.Pen1 && editingModeId <= EditingModeId.Pen5)
            {
                SetValue(SampleWindow.CurrentPenModePropertyKey, editingModeId);
            }
            else if (editingModeId >= EditingModeId.Highlighter1 && editingModeId <= EditingModeId.Highlighter5)
            {
                SetValue(SampleWindow.CurrentHighlighterModePropertyKey, editingModeId);
            }
        }

        /// <summary>
        /// Change to the ink mode with an eraser setting.
        /// </summary>
        /// <param name="editingModeId"></param>
        private void ChangeToEraseMode(EditingModeId editingModeId)
        {
            switch (editingModeId)
            {
                case EditingModeId.PointEraser1:
                case EditingModeId.PointEraser2:
                case EditingModeId.PointEraser3:
                    {
                        // Change to the point erase mode
                        StylusShape newSS = PointEraserShapeCollection[editingModeId - EditingModeId.PointEraser1];
                        ChangeToPointErase(newSS);
                        break;
                    }
                case EditingModeId.StrokeEraser:
                    {
                        // Change to the stroke erase mode
                        EnsureEditingMode(InkCanvasEditingMode.EraseByStroke);
                        break;
                    }
            }

            // Update the readonly dependency property.
            SetValue(SampleWindow.CurrentEraserModePropertyKey, editingModeId);
        }

        /// <summary>
        /// Change to the point erase mode
        /// </summary>
        /// <param name="ss"></param>
        private void ChangeToPointErase(StylusShape ss)
        {
            // Update InkCanvas EditingMode and the point eraser shape.
            EnsureEditingMode(InkCanvasEditingMode.EraseByPoint);
            MyInkCanvas.EraserShape = ss;
            MyInkCanvas.RenderTransform = new MatrixTransform();
        }

        /// <summary>
        /// Ensure the new InkCanvasEditingMode on the InkCanvas
        /// </summary>
        /// <param name="mode"></param>
        private void EnsureEditingMode(InkCanvasEditingMode mode)
        {
            if (MyInkCanvas.EditingMode != mode)
            {
                MyInkCanvas.EditingMode = mode;
            }

            if ((mode == InkCanvasEditingMode.EraseByPoint || mode == InkCanvasEditingMode.EraseByStroke)
                && MyInkCanvas.EditingModeInverted != mode)
            {
                // We update the EditingModeInverted if the new mode is one of the erase modes.
                MyInkCanvas.EditingModeInverted = mode;
            }
        }

        /// <summary>
        /// Enabling undo/redo for changes to the strokes collection.
        /// </summary>
        void Strokes_StrokesChanged(object sender, StrokeCollectionChangedEventArgs e)
        {
            StrokeCollection added = new StrokeCollection(e.Added);
            StrokeCollection removed = new StrokeCollection(e.Removed);

            CommandItem item = new StrokesAddedOrRemovedCI(_cmdStack, MyInkCanvas.EditingMode, added, removed, _editingOperationCount);
            _cmdStack.Enqueue(item);
        }

        /// <summary>
        /// Enabling undo/redo for selections moving or resizing.
        /// </summary>
        void MyInkCanvas_SelectionMovingOrResizing(object sender, InkCanvasSelectionEditingEventArgs e)
        {
            // Enforce stroke bounds to positive territory.
            Rect newRect = e.NewRectangle; Rect oldRect = e.OldRectangle;

            if (newRect.Top < 0d || newRect.Left < 0d)
            {
                Rect newRect2 =
                    new Rect(   newRect.Left < 0d ? 0d : newRect.Left,
                                newRect.Top < 0d ? 0d : newRect.Top,
                                newRect.Width,
                                newRect.Height);
                
                e.NewRectangle = newRect2;
            }
            CommandItem item = new SelectionMovedOrResizedCI(_cmdStack, MyInkCanvas.GetSelectedStrokes(), newRect, oldRect, _editingOperationCount);
            _cmdStack.Enqueue(item);
        }

        /// <summary>
        /// Track when mouse or stylus goes up to increment the editingOperationCount for undo / redo
        /// </summary>
        void MyInkCanvas_MouseUp(object sender, MouseButtonEventArgs e)
        {
            _editingOperationCount++;
        }

        /// <summary>
        /// Undo the last edit.
        /// </summary>
        private void Undo(object sender, RoutedEventArgs e)
        {
            _cmdStack.Undo();
        }

        /// <summary>
        /// Redo the last edit.
        /// </summary>
        private void Redo(object sender, RoutedEventArgs e)
        {
            _cmdStack.Redo();
        }

        #endregion Private Methods

        #region Private Properties

        /// <summary>
        /// An Adorner which is used for displaying the gesture feeback.
        /// </summary>
        private MessageAdorner GestureResultAdorner
        {
            get
            {
                // Initialize the MessageAdorner if it isn't created yet.
                if (_gestureResultAdorner == null)
                {
                    _gestureResultAdorner = new MessageAdorner(MyInkCanvas);
                    
                    // The MessageAdorner only needs to be rendered. Disable the HitTest on it.
                    _gestureResultAdorner.IsHitTestVisible = false;

                    AdornerLayer adornerLayer = AdornerLayer.GetAdornerLayer(MyInkCanvas);
                    adornerLayer.Add(_gestureResultAdorner);
                }

                return _gestureResultAdorner;
            }
        }

        #endregion Private Properties

        #region Private Fields

        private CommandStack _cmdStack;

        private ObservableCollection<DrawingAttributes>     _predefinedDrawingAttributes;
        private ObservableCollection<StylusShape>           _predefinedStylusShapes;
        private List<InkCanvasEditingMode>                  _inkModeOptions;
        private MessageAdorner                              _gestureResultAdorner;
        private const string                                GestureInfoUrl = "http://msdn.microsoft.com/library/default.asp?url=/library/en-us/tpcsdk10/lonestar/whitepapers/designguide/tbconusingapplicationgesturesandtheirsemantics.asp";
        private int                                         _editingOperationCount;

        #endregion Private Fields
    }
}