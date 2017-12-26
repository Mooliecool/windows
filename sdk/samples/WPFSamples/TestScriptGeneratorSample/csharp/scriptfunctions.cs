/******************************************************************************
 * File: ScriptFunctions.cs
 *
 * Description: 
 * This is the supporting class for the ExecuteScriptClient application. 
 * It provides the functionality required to execute the generated script  
 * that has been copied from the ScriptGeneratorClient application and 
 * pasted into Script.cs. 
 * The target application is the same one used by the script generator.
 * 
 * Note:
 * This sample relies on the 
 * 
 * See Client.cs in the ScriptGeneratorClient project for a full description 
 * of the sample code.
 *      
 * This file is part of the Microsoft Windows SDK Code Samples.
 * 
 * Copyright (C) Microsoft Corporation.  All rights reserved.
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
 *****************************************************************************/
using System;
using System.Collections.Generic;
using System.Text;
using System.Windows.Automation;
using System.Windows.Forms;
using System.Threading;

namespace ExecuteScript
{
    ///--------------------------------------------------------------------
    /// <summary>
    /// Supporting logic for generated script in Script.cs.
    /// </summary>
    ///--------------------------------------------------------------------
    class ScriptFunctions
    {
        private ExecuteScriptClient clientApp;
        private AutomationElement rootElement;

        ///--------------------------------------------------------------------
        /// <summary>
        /// Constructor
        /// </summary>
        /// <param name="targetAutomationID">
        /// The AutomationID of the target window.
        /// </param>
        /// <param name="client">
        /// The ExecuteScript client form.
        /// </param>
        /// <remarks>
        /// Initializes components.
        /// </remarks>
        ///--------------------------------------------------------------------
        public ScriptFunctions(
            string targetAutomationID, ExecuteScriptClient client)
        {
            clientApp = client;
            rootElement = AutomationElement.RootElement;
            rootElement = FindElement(targetAutomationID);
            if (rootElement == null)
            {
                Feedback("Could not find target.");
                return;
            }
            Feedback("Target found.");
        }

        ///--------------------------------------------------------------------
        /// <summary>
        /// Focus support function.
        /// </summary>
        /// <param name="automationID">
        /// The AutomationID of the target element.
        /// </param>
        ///--------------------------------------------------------------------
        internal void SetFocus(string automationID)
        {
            AutomationElement element;
            element = FindElement(automationID);
            if (element == null)
            {
                Feedback(automationID.ToString() + " could not be found.");
                return;
            }
            if ((bool)element.GetCurrentPropertyValue(AutomationElement.IsEnabledProperty) == false)
            {
                Feedback("Element not enabled.");
                return;
            }
            element.SetFocus();
            Feedback(automationID.ToString() + " focused.");
        }

        ///--------------------------------------------------------------------
        /// <summary>
        /// Invoke support function.
        /// </summary>
        /// <param name="automationID">
        /// The AutomationID of the target element.
        /// </param>
        ///--------------------------------------------------------------------
        internal void Invoke(string automationID)
        {
            AutomationElement element;
            element = FindElement(automationID);
            if (element == null)
            {
                Feedback(automationID.ToString() + " could not be found.");
                return;
            }
            if ((bool)element.GetCurrentPropertyValue(AutomationElement.IsEnabledProperty) == false)
            {
                Feedback("Element not enabled.");
                return;
            }
            InvokePattern invokePattern = 
                element.GetCurrentPattern(InvokePattern.Pattern) 
                as InvokePattern;
            invokePattern.Invoke();
            Feedback(automationID.ToString() + " invoked.");
        }

        ///--------------------------------------------------------------------
        /// <summary>
        /// RangeValue support function.
        /// </summary>
        /// <param name="automationID">
        /// The AutomationID of the target element.
        /// </param>
        /// <param name="rangevalue">
        /// The new range value.
        /// </param>
        ///--------------------------------------------------------------------
        internal void SetRangeValue(string automationID, int rangevalue)
        {
            AutomationElement element;
            element = FindElement(automationID);
            if (element == null)
            {
                Feedback(automationID.ToString() + " could not be found.");
                return;
            }
            if ((bool)element.GetCurrentPropertyValue(AutomationElement.IsEnabledProperty) == false)
            {
                Feedback("Element not enabled.");
                return;
            }
            RangeValuePattern rangevaluePattern = 
                element.GetCurrentPattern(RangeValuePattern.Pattern) 
                as RangeValuePattern;
            rangevaluePattern.SetValue(rangevalue);
            Feedback(automationID.ToString() + " value changed.");
        }

        ///--------------------------------------------------------------------
        /// <summary>
        /// Select support function.
        /// </summary>
        /// <param name="automationID">
        /// The AutomationID of the target element.
        /// </param>
        ///--------------------------------------------------------------------
        internal void Select(string automationID)
        {
            AutomationElement element;
            element = FindElement(automationID);
            if (element == null)
            {
                Feedback(automationID.ToString() + " could not be found.");
                return;
            }
            if ((bool)element.GetCurrentPropertyValue(AutomationElement.IsEnabledProperty) == false)
            {
                Feedback("Element not enabled.");
                return;
            }
            SelectionItemPattern selectionitemPattern = 
                element.GetCurrentPattern(SelectionItemPattern.Pattern) 
                as SelectionItemPattern;
            selectionitemPattern.Select();
            Feedback(automationID.ToString() + " item selected.");
        }

        ///--------------------------------------------------------------------
        /// <summary>
        /// Find the element of interest in tne target app.
        /// </summary>
        /// <param name="automationID">
        /// The AutomationID of the target element.
        /// </param>
        ///--------------------------------------------------------------------
        private AutomationElement FindElement(string automationID)
        {
            PropertyCondition propertyCondition =
                new PropertyCondition(
                AutomationElement.AutomationIdProperty, 
                automationID);
            return 
                rootElement.FindFirst(
                TreeScope.Element | TreeScope.Descendants, 
                propertyCondition);
        }

        ///--------------------------------------------------------------------
        /// <summary>
        /// Prints a line of text to the ExecuteScriptClient textbox.
        /// </summary>
        /// <param name="outputStr">The string to print.</param>
        /// <remarks>
        /// Must use Invoke so that UI is not being called directly from this thread.
        /// </remarks>
        ///--------------------------------------------------------------------
        private void Feedback(string outputStr)
        {

            clientApp.OutputFeedback(outputStr + Environment.NewLine);
            //clientApp.Invoke(new MethodInvoker(delegate()
            //{
            //    clientApp.OutputFeedback(outputStr + Environment.NewLine);
            //}
            //));
        }
    }
}
