using System;
using System.Collections;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.Text;
using System.IO;
using System.Xml;
using System.Windows;
using System.Windows.Ink;
using System.Windows.Input;
using System.Windows.Controls;
using System.Windows.Media;
using System.Windows.Shapes;
using System.Windows.Markup;
using System.Windows.Data;
using System.Globalization;

namespace InkCanvasClipboardSample
{
    /// <summary>
    /// Interaction logic for SampleWindow.xaml
    /// </summary>
    public partial class SampleWindow : System.Windows.Window
    {
        public SampleWindow()
        {
            InitializeComponent();
        }

        #region Drag and Drop Handlers

        /// <summary>
        /// OnPreviewMouseDown is called before InkCanvas sees the Down.  We use this 
        /// handler to hit test any selection and initiate a drag and drop operation
        /// if the user clicks on the selection to move it.
        /// </summary>
        private void OnPreviewMouseDown(object sender, MouseButtonEventArgs e)
        {
            InkCanvas inkCanvas = (InkCanvas)sender;
            Point downPosition = e.GetPosition(inkCanvas);

            //see if we hit the selection, and not a grab handle
            if (inkCanvas.HitTestSelection(downPosition) == InkCanvasSelectionHitResult.Selection)
            {
                //clone the selected strokes so we can transform them without affecting the actual selection
                StrokeCollection selectedStrokes = inkCanvas.GetSelectedStrokes();

                List<UIElement> selectedElements = new List<UIElement>(inkCanvas.GetSelectedElements());
                DataObject dataObject = new DataObject();

                //copy ink to the clipboard if we have any.  we do this even if there are no
                //strokes since clonedStrokes will be used by the Xaml clipboard code below
                StrokeCollection clonedStrokes = selectedStrokes.Clone();
                if (clonedStrokes.Count > 0)
                {
                    //translate the strokes relative to the down position
                    Matrix translation = new Matrix();
                    translation.Translate(-downPosition.X, -downPosition.Y);
                    clonedStrokes.Transform(translation, false);

                    //save the strokes to a dataobject to use during the dragdrop operation
                    MemoryStream ms = new MemoryStream();
                    clonedStrokes.Save(ms);
                    ms.Position = 0;
                    dataObject.SetData(StrokeCollection.InkSerializedFormat, ms);
                    
                    //we don't close the MemoryStream here, we'll do it in OnDrop
                }

                //Now we're going to add Xaml to the dragdrop dataobject.  We'll create an 
                //InkCanvas and add any selected strokes and elements in the selection to it
                InkCanvas inkCanvasForDragDrop = new InkCanvas();
                foreach (UIElement childElement in selectedElements)
                {
                    //we can't add elements in the selection to the InkCanvas that 
                    //represents selection (since they are already parented to the
                    //inkCanvas that has the selection) so we need to clone them.
                    //To clone each element, we need to convert it to Xaml and back again
                    string childXaml = XamlWriter.Save(childElement);
                    UIElement clonedChildElement =
                        (UIElement)XamlReader.Load(new XmlTextReader(new StringReader(childXaml)));

                    //adjust top and left relative to the down position
                    double childLeft = InkCanvas.GetLeft(clonedChildElement);
                    InkCanvas.SetLeft(clonedChildElement, childLeft - downPosition.X);

                    double childTop = InkCanvas.GetTop(clonedChildElement);
                    InkCanvas.SetTop(clonedChildElement, childTop - downPosition.Y);

                    inkCanvasForDragDrop.Children.Add(clonedChildElement);
                }

                //last, add the cloned strokes in case our drop location only supports Xaml
                //this preserves both the ink and the selected elements
                inkCanvasForDragDrop.Strokes = clonedStrokes;

                //now copy the Xaml for the InkCanvas that represents selection to the clipboard
                string inkCanvasXaml = XamlWriter.Save(inkCanvasForDragDrop);
                dataObject.SetData(DataFormats.Xaml, inkCanvasXaml);

                //The call to DragDrop.DoDragDrop will block until the drag and drop operation is completed
                //once it does, 'effects' will have been updated by OnDragOver getting called
                //if we're moving the strokes and elements, we'll remove them.  If we're copying them
                //(the CTRL key is pressed) we won't remove them.
                DragDropEffects effects =
                    DragDrop.DoDragDrop(inkCanvas, dataObject, DragDropEffects.Move | DragDropEffects.Copy);

                if (effects == DragDropEffects.Move)
                {
                    inkCanvas.Strokes.Remove(selectedStrokes);

                    foreach (UIElement childElement in selectedElements)
                    {
                        inkCanvas.Children.Remove(childElement);
                    }
                }
            }
        }

        /// <summary>
        /// Called during the drag operation, we set e.Effects both to update
        /// the cursor and to inform the OnPreviewMouseDown method if it should 
        /// remove the strokes.
        /// </summary>
        private void OnDragOver(object sender, DragEventArgs e)
        {
            if (e.Data.GetDataPresent(StrokeCollection.InkSerializedFormat) ||
                e.Data.GetDataPresent(DataFormats.Xaml))
            {
                SetDragDropEffects(e);
                e.Handled = true;
            }
        }

        /// <summary>
        /// SetDragDropEffects changes the cursor based on key and mouse state
        /// as well as what effects are allowed
        /// </summary>
        private void SetDragDropEffects(DragEventArgs e)
        {
            e.Effects = DragDropEffects.None;

            if ((e.AllowedEffects & DragDropEffects.Move) != 0)
            {
                e.Effects = DragDropEffects.Move;
            }

            //if the CTRL key is down, treat this as a copy
            if ((e.KeyStates & DragDropKeyStates.ControlKey) != 0 &&
                (e.AllowedEffects & DragDropEffects.Copy) != 0)
            {
                e.Effects = DragDropEffects.Copy;
            }
        }

        /// <summary>
        /// OnQueryContinueDrag is called to see if we should continue the drag and drop
        /// operation.  If the escape key is pressed, we cancel it.
        /// </summary>
        private void OnQueryContinueDrag(object sender, QueryContinueDragEventArgs e)
        {
            if (e.EscapePressed)
            {
                e.Action = DragAction.Cancel;
                e.Handled = true;
            }
        }

        /// <summary>
        /// OnDrop - called when drag and drop contents are dropped on an InkCanvas
        /// </summary>
        private void OnDrop(object sender, DragEventArgs e)
        {
            InkCanvas inkCanvas = (InkCanvas)sender;

            //see if ISF is present in the drag and drop IDataObject
            if (e.Data.GetDataPresent(StrokeCollection.InkSerializedFormat) ||
                e.Data.GetDataPresent(DataFormats.Xaml))
            {
                SetDragDropEffects(e);

                if (e.Effects == DragDropEffects.Move || e.Effects == DragDropEffects.Copy)
                {
                    Point dropPosition = e.GetPosition(inkCanvas);

                    //after we drop, we need to select the elements and strokes that were
                    //copied into the inkCanvas
                    List<UIElement> elementsToSelect = new List<UIElement>();
                    if (e.Data.GetDataPresent(DataFormats.Xaml))
                    {
                        //paste Xaml
                        string xamlData = (string)e.Data.GetData(DataFormats.Xaml);
                        if (!String.IsNullOrEmpty(xamlData))
                        {
                            UIElement element =
                                XamlReader.Load(new XmlTextReader(new StringReader(xamlData))) as UIElement;
                            if (element != null)
                            {
                                //check to see if this is an InkCanvas
                                InkCanvas inkCanvasFromDragDrop = element as InkCanvas;
                                if (inkCanvasFromDragDrop != null)
                                {
                                    //we assume this was put on the data object by us.
                                    //remove the children and add them to the drop inkCanvas.
                                    while (inkCanvasFromDragDrop.Children.Count > 0)
                                    {
                                        UIElement childElement = inkCanvasFromDragDrop.Children[0];
                                        double childLeft = InkCanvas.GetLeft(childElement);
                                        InkCanvas.SetLeft(childElement, childLeft + dropPosition.X);

                                        double childTop = InkCanvas.GetTop(childElement);
                                        InkCanvas.SetTop(childElement, childTop + dropPosition.Y);

                                        inkCanvasFromDragDrop.Children.Remove(childElement);

                                        inkCanvas.Children.Add(childElement); 
                                        elementsToSelect.Add(childElement);
                                    }
                                }
                                else
                                {
                                    //just add the element, it wasn't another InkCanvas that we
                                    //added to the dataobject as a container
                                    inkCanvas.Children.Add(element);
                                    elementsToSelect.Add(element);
                                }
                            }
                        }
                    }

                    //now check to see if we have ISF as well
                    StrokeCollection strokesToSelect = new StrokeCollection();
                    if (e.Data.GetDataPresent(StrokeCollection.InkSerializedFormat))
                    {
                        //only ISF was present
                        MemoryStream ms = (MemoryStream)e.Data.GetData(StrokeCollection.InkSerializedFormat);
                        StrokeCollection sc = new StrokeCollection(ms);
                        ms.Close();

                        //translate the strokes back from the origin to the current position
                        Matrix translation = new Matrix();
                        translation.Translate(dropPosition.X, dropPosition.Y);
                        sc.Transform(translation, false);

                        //add the strokes from the IDataObject to the inkCanvas and select them.
                        inkCanvas.Strokes.Add(sc);
                        strokesToSelect.Add(sc);
                    }

                    //now that we're done, we select
                    if (elementsToSelect.Count > 0 || strokesToSelect.Count > 0)
                    {
                        inkCanvas.Select(strokesToSelect, elementsToSelect);
                    }

                    e.Handled = true;
                }
            }
        }

        #endregion

        #region File Menu Handlers

        /// <summary>
        /// clears strokes and elements from inkCanvas1 and inkCanvas2
        /// </summary>
        private void OnClearInkAndElements(object sender, RoutedEventArgs e)
        {
            inkCanvas1.Strokes.Clear();
            inkCanvas1.Children.Clear();

            inkCanvas2.Strokes.Clear();
            inkCanvas2.Children.Clear();
        }

        /// <summary>
        /// closes the application
        /// </summary>
        private void OnExit(object sender, RoutedEventArgs e)
        {
            this.Close();
        }
        #endregion

        #region InkCanvas Menu Handlers

        /// <summary>
        /// OnInkCanvasSelectChecked
        /// </summary>
        private void OnInkCanvasSelectChecked(object sender, RoutedEventArgs e)
        {
            MenuItem menuItem = (MenuItem)sender;
            if ((string)menuItem.Tag == "inkCanvas1")
            {
                inkCanvas1.EditingMode = InkCanvasEditingMode.Select;
                miInkCanvas1EditingModeSelect.IsChecked = true;
            }
            else
            {
                inkCanvas2.EditingMode = InkCanvasEditingMode.Select;
                miInkCanvas2EditingModeSelect.IsChecked = true;
            }
        }
        /// <summary>
        /// OnInkCanvas1InkChecked
        /// </summary>
        private void OnInkCanvasInkChecked(object sender, RoutedEventArgs e)
        {
            MenuItem menuItem = (MenuItem)sender;
            if ((string)menuItem.Tag == "inkCanvas1")
            {
                inkCanvas1.EditingMode = InkCanvasEditingMode.Ink;
                miInkCanvas1EditingModeInk.IsChecked = true;
            }
            else
            {
                inkCanvas2.EditingMode = InkCanvasEditingMode.Ink;
                miInkCanvas2EditingModeInk.IsChecked = true;
            }
            
        }

        #endregion

        #region Insert Menu Handlers

        /// <summary>
        /// inserts a new TextBox into InkCanvas
        /// </summary>
        private void OnInsertTextBox(object sender, RoutedEventArgs e)
        {
            InkCanvas inkCanvas = null;
            MenuItem menuItem = (MenuItem)sender;
            if ((string)menuItem.Tag == "inkCanvas1")
            {
                inkCanvas = inkCanvas1;
            }
            else
            {
                inkCanvas = inkCanvas2;
            }

            TextBox textBoxToInsert = new TextBox();
            textBoxToInsert.Text = "New TextBox";
            textBoxToInsert.AcceptsReturn = true;
            textBoxToInsert.SetValue(InkCanvas.TopProperty, inkCanvas.ActualHeight / 2d);
            textBoxToInsert.SetValue(InkCanvas.LeftProperty, inkCanvas.ActualWidth / 2d);

            inkCanvas.Children.Add(textBoxToInsert);

        }

        /// <summary>
        /// inserts a new Label into InkCanvas
        /// </summary>
        private void OnInsertLabel(object sender, RoutedEventArgs e)
        {
            InkCanvas inkCanvas = null;
            MenuItem menuItem = (MenuItem)sender;
            if ((string)menuItem.Tag == "inkCanvas1")
            {
                inkCanvas = inkCanvas1;
            }
            else
            {
                inkCanvas = inkCanvas2;
            }

            Label labelToInsert = new Label();
            labelToInsert.Content = "New Label";
            labelToInsert.Background = Brushes.LightBlue;
            labelToInsert.SetValue(InkCanvas.TopProperty, inkCanvas.ActualHeight / 2d);
            labelToInsert.SetValue(InkCanvas.LeftProperty, inkCanvas.ActualWidth / 2d);

            inkCanvas.Children.Add(labelToInsert);
        }

        /// <summary>
        /// inserts a new Ellipse shape into InkCanvas
        /// </summary>
        private void OnInsertShape(object sender, RoutedEventArgs e)
        {
            InkCanvas inkCanvas = null;
            MenuItem menuItem = (MenuItem)sender;
            if ((string)menuItem.Tag == "inkCanvas1")
            {
                inkCanvas = inkCanvas1;
            }
            else
            {
                inkCanvas = inkCanvas2;
            }

            Ellipse ellipseToInsert = new Ellipse();
            ellipseToInsert.Width = 150d;
            ellipseToInsert.Height = 100d;
            ellipseToInsert.Fill = new LinearGradientBrush(Colors.Goldenrod, Colors.HotPink, 20d);
            ellipseToInsert.SetValue(InkCanvas.TopProperty, inkCanvas.ActualHeight / 2d);
            ellipseToInsert.SetValue(InkCanvas.LeftProperty, inkCanvas.ActualWidth / 2d);

            inkCanvas.Children.Add(ellipseToInsert);
        }
        #endregion

        #region Enable / Disable Drag Drop Handlers

        /// <summary>
        /// Enables drag and drop
        /// </summary>
        private void OnDragAndDropEnabled(object sender, RoutedEventArgs e)
        {
            //enable drag-drop
            inkCanvas1.AllowDrop = true;
            inkCanvas1.QueryContinueDrag += new QueryContinueDragEventHandler(OnQueryContinueDrag);
            inkCanvas1.Drop += new DragEventHandler(OnDrop);
            inkCanvas1.DragOver += new DragEventHandler(OnDragOver);
            inkCanvas1.PreviewMouseDown += new MouseButtonEventHandler(OnPreviewMouseDown);

            inkCanvas2.AllowDrop = true;
            inkCanvas2.QueryContinueDrag += new QueryContinueDragEventHandler(OnQueryContinueDrag);
            inkCanvas2.Drop += new DragEventHandler(OnDrop);
            inkCanvas2.DragOver += new DragEventHandler(OnDragOver);
            inkCanvas2.PreviewMouseDown += new MouseButtonEventHandler(OnPreviewMouseDown);
        }

        /// <summary>
        /// Disables drag and drop
        /// </summary>
        private void OnDragAndDropDisabled(object sender, RoutedEventArgs e)
        {
            //enable drag-drop
            inkCanvas1.AllowDrop = false;
            inkCanvas1.QueryContinueDrag -= new QueryContinueDragEventHandler(OnQueryContinueDrag);
            inkCanvas1.Drop -= new DragEventHandler(OnDrop);
            inkCanvas1.DragOver -= new DragEventHandler(OnDragOver);
            inkCanvas1.PreviewMouseDown -= new MouseButtonEventHandler(OnPreviewMouseDown);

            inkCanvas2.AllowDrop = false;
            inkCanvas2.QueryContinueDrag -= new QueryContinueDragEventHandler(OnQueryContinueDrag);
            inkCanvas2.Drop -= new DragEventHandler(OnDrop);
            inkCanvas2.DragOver -= new DragEventHandler(OnDragOver);
            inkCanvas2.PreviewMouseDown -= new MouseButtonEventHandler(OnPreviewMouseDown);
        }

        #endregion
    }

    #region DataCoverters
    /// <summary>
    /// A Converter class that return true if the EditingMode is Ink 
    /// </summary>
    public class InkEditingModeConverter : IValueConverter
    {
        /// <summary>
        /// Returns true if the EditingMode is Ink
        /// </summary>
        public object Convert(object o, Type type, object parameter, CultureInfo culture)
        {
            InkCanvasEditingMode editingMode = (InkCanvasEditingMode)o;

            if (editingMode == InkCanvasEditingMode.Ink)
            {
                return true;
            }
            return false;
        }

        /// <summary>
        /// Do not convert back.
        /// </summary>
        public object ConvertBack(object o, Type type, object parameter, CultureInfo culture)
        {
            return null;
        }
    }

    /// <summary>
    /// A Converter class that return true if the EditingMode is Select 
    /// </summary>
    public class SelectEditingModeConverter : IValueConverter
    {
        /// <summary>
        /// Returns true if the EditingMode is Select
        /// </summary>
        public object Convert(object o, Type type, object parameter, CultureInfo culture)
        {
            InkCanvasEditingMode editingMode = (InkCanvasEditingMode)o;

            if (editingMode == InkCanvasEditingMode.Select)
            {
                return true;
            }
            return false;
        }

        /// <summary>
        /// Do not convert back.
        /// </summary>
        public object ConvertBack(object o, Type type, object parameter, CultureInfo culture)
        {
            return null;
        }
    }
    #endregion
}