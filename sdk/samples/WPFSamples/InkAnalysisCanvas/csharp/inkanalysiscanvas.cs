using System;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.Text;
using System.ComponentModel;
using System.Diagnostics;
using System.Windows;
using System.Windows.Media;
using System.Windows.Controls;
using System.Windows.Threading;
using System.Windows.Documents;
using System.Windows.Ink;

namespace InkAnalysisCanvasSample
{
    /// <summary>
    /// InkAnalysisCanvas demostrates the correct way to 
    /// interact with the InkAnalyzer in WPF
    /// </summary>
    public class InkAnalysisCanvas : InkCanvas
    {
        public InkAnalysisCanvas()
        {
            _inkAnalyzer = new InkAnalyzer(this.Dispatcher);


            // Add a listener to ResultsUpdated event.
            _inkAnalyzer.ResultsUpdated += OnInkAnalyzerResultsUpdated;

            // Add a listener to StrokesChanged event of InkAnalysis.Strokes collection.
            this.Strokes.StrokesChanged += OnStrokesChanged;

            this.ShowInkAnalysisFeedback = true;
        }

        /// <summary>
        /// InkAnalyzer this InkAnalysisCanvas is updating
        /// </summary>
        public InkAnalyzer InkAnalyzer
        {
            get { return _inkAnalyzer; }
        }

        /// <summary>
        /// Set to true to display the parse structure and recognition results
        /// overlayed on the InkAnalysisCanvas
        /// </summary>
        public bool ShowInkAnalysisFeedback
        {
            get { return _showInkAnalysisFeedback; }
            set
            {
                _showInkAnalysisFeedback = value;

                if (_adornerDecorator == null)
                {
                    //We want to adorn the InkCanvas's inner canvas with an adorner 
                    //that we use to display the parse and recognition results
                    _adornerDecorator = (AdornerDecorator)GetVisualChild(0);
                    DependencyObject inkPresenter = VisualTreeHelper.GetChild(_adornerDecorator, 0);
                    DependencyObject innerCanvas = VisualTreeHelper.GetChild(inkPresenter, 0);

                    _feedbackAdorner = new InkAnalysisFeedbackAdorner((UIElement)innerCanvas, _inkAnalyzer);
                    _adornerDecorator.AdornerLayer.Add(_feedbackAdorner);
                }

                if (_showInkAnalysisFeedback)
                {
                    _feedbackAdorner.Visibility = Visibility.Visible;
                }
                else
                {
                    _feedbackAdorner.Visibility = Visibility.Collapsed;
                }
            }
        }


        /// <summary>
        /// Protected override called when the InkAnalysisCanvas.Strokes StrokeCollection is replaced
        /// </summary>
        protected override void OnStrokesReplaced(InkCanvasStrokesReplacedEventArgs e)
        {
            //update the InkAnalyzer and unsubscribe / subscribe to StrokeChanged events
            _inkAnalyzer.RemoveStrokes(e.PreviousStrokes);
            e.PreviousStrokes.StrokesChanged -= OnStrokesChanged;
            foreach (Stroke stroke in e.PreviousStrokes)
            {
                //we're removing this stroke so we don't need to listen 
                //to StylusPointsChanged anymore
                stroke.StylusPointsChanged -= OnStrokeStylusPointsChanged;
            }

            _inkAnalyzer.AddStrokes(e.NewStrokes);
            e.NewStrokes.StrokesChanged += OnStrokesChanged;
            Debug.Assert(e.NewStrokes == this.Strokes);
            foreach (Stroke stroke in e.NewStrokes)
            {
                //listen for StylusPointsChanged, which can happen
                //during move and resize operations
                stroke.StylusPointsChanged += OnStrokeStylusPointsChanged;
            }

            _inkAnalyzer.BackgroundAnalyze();
        }

        /// <summary>
        /// called when a gesture is detected, we use this to detect a scratchout gesture
        /// and erase strokes
        /// </summary>
        protected override void OnGesture(InkCanvasGestureEventArgs e)
        {
            base.OnGesture(e);

            ReadOnlyCollection<GestureRecognitionResult> results =
                e.GetGestureRecognitionResults();

            if (results.Count > 0 &&
                results[0].ApplicationGesture == ApplicationGesture.ScratchOut &&
                results[0].RecognitionConfidence == RecognitionConfidence.Strong)
            {
                //hit test the InkAnalysisCanvas's Strokes
                StrokeCollection hitStrokes = this.Strokes.HitTest(e.Strokes.GetBounds(), 1 /*percent*/);
                //remove any hit strokes from the InkAnalysisCanvas
                if (hitStrokes.Count > 0)
                {
                    //OnStrokesChanged will be called after this and 
                    //remove the erased strokes from the InkAnalyzer
                    this.Strokes.Remove(hitStrokes);
                }
                e.Cancel = false;
            }
            else
            {
                //cancel the event, it wasn't a scratchout
                e.Cancel = true;
            }
        }



        /// <summary>
        /// OnStrokesChanged - A handler for InkCanvas.Strokes.StrokesChanged event
        /// </summary>
        private void OnStrokesChanged(object sender, StrokeCollectionChangedEventArgs e)
        {
            //flag we set when we change the StrokeCollection within this method
            //to prevent recursion
            if (_ignoreStrokesChanged)
            {
                _ignoreStrokesChanged = false;
                return;
            }

            // special case: when point erasing and you trim the end 
            // of a single stroke, the InkCanvas will replace it with a shorter stroke
            // to improve recognition, we want to keep the original stroke but to update
            // it's stroke data
            if (this.ActiveEditingMode == InkCanvasEditingMode.EraseByPoint &&
                e.Removed.Count == 1 && e.Added.Count == 1)
            {
                _ignoreStrokesChanged = true;

                //get the index the stroke was replaced at
                int addedIndex = this.Strokes.IndexOf(e.Added[0]);
                Debug.Assert(addedIndex >= 0);


                Stroke removedStroke = e.Removed[0];
                removedStroke.StylusPoints = e.Added[0].StylusPoints.Clone();

                //let the InkAnalyzer know that we're about to invalidate 
                //the StylusPoints on removedStroke
                _inkAnalyzer.ClearStrokeData(removedStroke);

                //update the dirty region
                _inkAnalyzer.DirtyRegion.Union(e.Added[0].GetBounds());
                _inkAnalyzer.DirtyRegion.Union(e.Removed[0].GetBounds());

                //replace the added stroke with the removed stroke (which has had it's StylusPoints updated)
                this.Strokes[addedIndex] = removedStroke;
            }
            else
            {
                // Update the ink data of the ink analyzer.
                if (e.Removed.Count > 0)
                {
                    foreach (Stroke stroke in e.Removed)
                    {
                        //we're removing this stroke so we don't need to listen 
                        //to StylusPointsChanged anymore
                        stroke.StylusPointsChanged -= OnStrokeStylusPointsChanged;
                    }
                    _inkAnalyzer.RemoveStrokes(e.Removed);


                }
                if (e.Added.Count > 0)
                {
                    foreach (Stroke stroke in e.Added)
                    {
                        //listen for StylusPointsChanged, which can happen
                        //during move and resize operations
                        stroke.StylusPointsChanged += OnStrokeStylusPointsChanged;
                    }
                    _inkAnalyzer.AddStrokes(e.Added);
                    _inkAnalyzer.SetStrokesType(e.Added, StrokeType.Unspecified);
                }
            }

            _inkAnalyzer.BackgroundAnalyze();
        }

        /// <summary>
        /// OnSelectionChanging raised when selection is changing on the InkAnalysisCanvas
        /// we use this to select entire nodes instead of just a stroke
        /// </summary>
        protected override void OnSelectionChanging(InkCanvasSelectionChangingEventArgs e)
        {
            StrokeCollection currentSelectedStrokes = e.GetSelectedStrokes();
            if (currentSelectedStrokes.Count > 0)
            {
                ContextNodeCollection nodes =
                    _inkAnalyzer.FindInkLeafNodes(e.GetSelectedStrokes());

                if (nodes.Count > 0)
                {
                    StrokeCollection expandedSelection = new StrokeCollection();
                    //add all related strokes if they aren't part of selectedStrokes already
                    foreach (ContextNode node in nodes)
                    {
                        expandedSelection.Add(node.Strokes);
                    }
                    //modify the collection
                    e.SetSelectedStrokes(expandedSelection);
                }
            }

            base.OnSelectionChanging(e);
        }


        /// <summary>
        /// Called when the StylusPoints on a Stroke are changed
        /// we use this to invalidate the stroke data in the analyzer
        /// </summary>
        private void OnStrokeStylusPointsChanged(object sender, EventArgs e)
        {
            Stroke changedStroke = (Stroke)sender;

            //a stroke's StylusPoints have changed we need to find
            //all affected contextNodes's and mark the dirty region with them
            StrokeCollection strokesThatChanged = new StrokeCollection();
            strokesThatChanged.Add(changedStroke);
            ContextNodeCollection dirtyNodes =
                _inkAnalyzer.FindInkLeafNodes(strokesThatChanged);

            foreach (ContextNode dirtyNode in dirtyNodes)
            {
                //let the analyzer know that where the stroke previously 
                //existed is now dirty
                _inkAnalyzer.DirtyRegion.Union(dirtyNode.Location.GetBounds());
            }

            //let the analyzer know that the stroke data is no longer valid
            _inkAnalyzer.ClearStrokeData(changedStroke);

            //finally, make the region where the stroke now exists dirty also
            _inkAnalyzer.DirtyRegion.Union(changedStroke.GetBounds());

            _inkAnalyzer.BackgroundAnalyze();
        }

        /// <summary>
        /// OnInkAnalyzerResultsUpdated - A handler for InkAnalyzer.ResultsUpdated event
        /// which is called when BackgroundAnalyze completes
        /// </summary>
        private void OnInkAnalyzerResultsUpdated(object sender, ResultsUpdatedEventArgs e)
        {
            if (_feedbackAdorner != null)
            {
                //cause the feedback adorner to repaint itself
                _feedbackAdorner.InvalidateVisual();
            }

            // If the user has made edits while analysis was being performed, trigger
            // BackgroundAnalyze again to analyze these changes
            if (!_inkAnalyzer.DirtyRegion.IsEmpty)
            {
                _inkAnalyzer.BackgroundAnalyze();
            }
        }

        /// <summary>
        /// our private InkAnalyzer
        /// </summary>
        private InkAnalyzer _inkAnalyzer;
        /// <summary>
        /// Flag to prevent recursion in OnStrokesChanged
        /// </summary>
        private bool _ignoreStrokesChanged = false;
        /// <summary>
        /// Flag set via ShowInkAnalysisFeedback that determines if 
        /// we should show parsing structure feedback and analysis results 
        /// overlayed on the strokes
        /// </summary>
        private bool _showInkAnalysisFeedback = true;
        /// <summary>
        /// The private AdornerDecorator InkCanvas uses to render selection feedback.
        /// We use it to display feedback for InkAnalysis
        /// </summary>
        private AdornerDecorator _adornerDecorator = null;
        private InkAnalysisFeedbackAdorner _feedbackAdorner = null;

    }
}