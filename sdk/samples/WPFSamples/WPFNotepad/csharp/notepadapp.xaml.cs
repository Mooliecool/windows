/*******************************************************************************
 *
 * Copyright (c) 2004-2005 Microsoft Corporation.  All rights reserved.
 *
 * Description: The NotepadApp class defines Application object for the application.
 *
 *******************************************************************************/

namespace Microsoft.Samples.WPFNotepad
{
    using System;
    using System.Reflection;
    using System.Windows;
    using System.Windows.Controls;
    using System.Windows.Input;

    #region public Enums

    //------------------------------------------------------
    //
    //  public Enums
    //
    //------------------------------------------------------

    public enum EditingMode
    {
        Plain, Rich
    }

    #endregion public Enums

    public partial class NotepadApp : Application
    {

        #region public Properties

        //------------------------------------------------------
        //
        //  public Properties
        //
        //------------------------------------------------------

        public EditingMode CurrentEditingMode
        {
            get
            {
                return _currentEditingMode;
            }
            set
            {
                _currentEditingMode = value;
            }
        }


        #endregion public Properties


        #region private Methods

        //------------------------------------------------------
        //
        //  private Methods
        //
        //------------------------------------------------------

        
        protected override void OnStartup(StartupEventArgs e)
        {
            CurrentEditingMode = EditingMode.Plain;
            editor = new NotepadEditor();
            
            if (e.Args.Length > 0)
            {
#if AVPERFTEST
                if (!e.Args[0].StartsWith("/"))
                {
#endif
                // Parse the command-line arguments
                string document = null;
                bool printCommand = false;
                foreach (string arg in e.Args)
                {
                    if ((arg[0] == '-') || (arg[0] == '/'))
                    {
                        if (arg.Length > 1)
                        {
                            switch (arg[1])
                            {
                                case 'p':
                                case 'P':
                                    printCommand = true;
                                    break;

                                default:
                                    // Just ignore any unrecognized switches
                                    break;
                            }
                        }
                    }
                    else
                    {
                        // Assume this is the document to open/print.
                        // If there are multiple ones specified, last one wins.
                        document = arg;
                    }
                }

                if (document != null)
                {
                    editor.Document = new NotepadDocument(document);
                }
#if AVPERFTEST
                }
#endif

                if (printCommand)
                {
                    if (document != null)
                    {
                        // We need an instance of the print dialog for settings, even though we won't actually
                        // show it.
                        editor.SetUpPrintDialog();

                        editor.PrintDocument();
                    }
                    else
                    {
                        // Just do nothing if /p was specified without a document
                    }

                    Shutdown();
                }
                else
                {

#if AVPERFTEST
            // <!-- Performance Instrumentation : ProcessEnv -->
            // <!-- Performance Instrumentation : PerfGCVars  -->
#endif

                    editor.Show();
                }

            }
            else
            {
                // No arguments specified, so just show the editor empty
                editor.Show();
            }

        }

#if AVPERFTEST
            // <!-- Performance Instrumentation : PerfHandleGC -->
            // <!-- Performance Instrumentation : PerfVars -->
            // <!-- Performance Instrumentation : ResizeVars -->
            // <!-- Performance Instrumentation : HoverVars -->

            // <!-- Performance Instrumentation : BeginHandleIdle -->

            // <!-- Performance Instrumentation : HandleIdleResize -->
            // <!-- Performance Instrumentation : HandleIdleHover -->

            // <!-- Performance Instrumentation : EndHandleIdle -->
            // <!-- Performance Instrumentation : WorkerResize -->
            // <!-- Performance Instrumentation : WorkerHover -->
            // <!-- Performance Instrumentation : UiUtils -->
#endif
        #endregion private Methods


        #region private Fields

        //------------------------------------------------------
        //
        //  private Fields
        //
        //------------------------------------------------------

        NotepadEditor editor;
        EditingMode _currentEditingMode;
        #endregion private Fields



    }
}