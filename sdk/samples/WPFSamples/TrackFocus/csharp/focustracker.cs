/*******************************************************************************
 *
 * File: FocusTracker.cs
 *
 * Description: This is a simple console application that might be used as a 
 * starting-point for an application that uses UI Automation to track events on 
 * the desktop, such as a screen reader.
 * 
 * The program announces when the input focus changes. If the focus moves to a 
 * different application window, the name (usually the caption) of the window is 
 * announced. If the focus moves within an application window, the type and name
 * of the control being read are announced.
 * 
 * To know when the focus switches from one application to another, a TreeWalker 
 * is used to find the parent window, and that window is compared with the last 
 * window that had focus.
 * 
 * 
 *  This file is part of the Microsoft Windows SDK Code Samples.
 * 
 *  Copyright (C) Microsoft Corporation.  All rights reserved.
 * 
 * This source code is intended only as a supplement to Microsoft
 * Development Tools and/or on-line documentation.  See these other
 * materials for detailed information regarding Microsoft code samples.
 * 
 * THIS CODE AND INFORMATION ARE PROVIDED AS IS WITHOUT WARRANTY OF ANY
 * KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
 * PARTICULAR PURPOSE.
 * 
 ******************************************************************************/


using System;
using System.Collections.Generic;
using System.Text;
using System.Windows.Automation;
using System.Collections;

namespace FocusTracker
{
    class Tracker
    {
        AutomationElement lastTopLevelWindow;

        /// <summary>
        /// Constructor.
        /// </summary>
        public Tracker()
        {
            Startup();
        }

        /// <summary>
        /// Entry point.
        /// </summary>
        /// <param name="args">Command-line arguments; not used.</param>
        public static void Main(string[] args)
        {
            Console.Title = "UI Automation Focus-tracking Sample";
            Console.WriteLine("Please wait while UI Automation initializes...");
            Tracker reader = new Tracker();
            Console.WriteLine("Tracking focus. Press Enter to quit.");
            Console.ReadLine();
            Automation.RemoveAllEventHandlers();
        }

        /// <summary>
        /// Initialization.
        /// </summary>
        void Startup()
        {
            Automation.AddAutomationFocusChangedEventHandler(OnFocusChanged);
        }


        /// <summary>
        /// Retrieves the top-level window that contains the specified 
        /// UI Automation element.
        /// </summary>
        /// <param name="element">The contained element.</param>
        /// <returns>The  top-level window element.</returns>
        private AutomationElement GetTopLevelWindow(AutomationElement element)
        {
            TreeWalker walker = TreeWalker.ControlViewWalker;
            AutomationElement elementParent;
            AutomationElement node = element;
            try  // In case the element disappears suddenly, as menu items are 
                 // likely to do.
            {
               if (node == AutomationElement.RootElement)
                {
                    return node;
                }
                // Walk up the tree to the child of the root.
                while (true)
                {
                    elementParent = walker.GetParent(node);
                    if (elementParent == null)
                    {
                        return null;
                    }
                    if (elementParent == AutomationElement.RootElement)
                    {
                        break;
                    }
                    node = elementParent;
                }
            }
            catch (ElementNotAvailableException)
            {
                node = null;
            }
            catch (ArgumentNullException)
            {
                node = null;
            }
            return node;
        }


        /// <summary>
        /// Handles focus-changed events. If the element that received focus is 
        /// in a different top-level window, announces that. If not, just
        ///  announces which element received focus.
        /// </summary>
        /// <param name="src">Object that raised the event.</param>
        /// <param name="e">Event arguments.</param>
        private void OnFocusChanged(object src, AutomationFocusChangedEventArgs e)
        {
            try
            {
                AutomationElement elementFocused = src as AutomationElement;
                AutomationElement topLevelWindow = GetTopLevelWindow(elementFocused);
                if (topLevelWindow == null)
                {
                    return;
                }

                // If top-level window has changed, announce it.
                if (topLevelWindow != lastTopLevelWindow)
                {
                    lastTopLevelWindow = topLevelWindow;
                    Console.WriteLine("Focus moved to top-level window:");
                    Console.WriteLine("  " + topLevelWindow.Current.Name);
                    Console.WriteLine();
                }
                else
                {
                    // Announce focused element.
                    Console.WriteLine("Focused element: ");
                    Console.WriteLine("  Type: " +
                        elementFocused.Current.LocalizedControlType);
                    Console.WriteLine("  Name: " + elementFocused.Current.Name);
                    Console.WriteLine();
                }
            }
            catch (ElementNotAvailableException)
            {
                return;
            }
        }
    } 
}  
