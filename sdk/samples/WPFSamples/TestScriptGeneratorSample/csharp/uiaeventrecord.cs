/******************************************************************************
 *
 * File: UIAEventRecord.cs
 *
 * Description: UI Automation event recording class.
 * 
 * See Client.cs in the ScriptGeneratorClient project for a full description 
 * of the sample code.
 *   
 * 
 * This file is part of the Microsoft WinfFX SDK Code Samples.
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

using System.Windows.Automation;
using System;

namespace TestScriptGeneratorSample
{
    ///--------------------------------------------------------------------
    /// <summary>
    /// General storage class for script generator.
    /// </summary>
    ///--------------------------------------------------------------------
    public class UiaEventRecord
    {
        ///--------------------------------------------------------------------
        /// <summary>
        /// Stores and retrieves the AutomationElementInformation.AutomationID 
        /// for the current automation element.
        /// </summary>
        ///--------------------------------------------------------------------
        public string AutomationId
        {
            get
            {
                return automationId;
            }
            set
            {
                automationId = value;
            }
        }
        string automationId;

        ///--------------------------------------------------------------------
        /// <summary>
        /// Stores and retrieves the AutomationElementInformation.ClassName 
        /// for the current automation element.
        /// </summary>
        ///--------------------------------------------------------------------
        public string ClassName
        {
            get
            {
                return className;
            }
            set
            {
                className = value;
            }
        }
        string className;

        ///--------------------------------------------------------------------
        /// <summary>
        /// Stores and retrieves the 
        /// AutomationElementInformation.ControlType.ProgrammaticName 
        /// for the current automation element.
        /// </summary>
        ///--------------------------------------------------------------------
        public string ControlType
        {
            get
            {
                return controlType;
            }
            set
            {
                controlType = value;
            }
        }
        string controlType;

        ///--------------------------------------------------------------------
        /// <summary>
        /// Stores and retrieves the string equivalent of the event ID that
        /// the current automation element is reporting.
        /// </summary>
        ///--------------------------------------------------------------------
        public string EventId
        {
            get
            {
                return eventId;
            }
            set
            {
                eventId = value;
            }
        }
        string eventId;

        ///--------------------------------------------------------------------
        /// <summary>
        /// Stores and retrieves the time of an event.
        /// </summary>
        ///--------------------------------------------------------------------
        public DateTime EventTime
        {
            get
            {
                return eventTime;
            }
            set
            {
                eventTime = value;
            }
        }
        DateTime eventTime;

        ///--------------------------------------------------------------------
        /// <summary>
        /// Stores and retrieves the an array of supported Automation Patterns 
        /// for the current automation element.
        /// </summary>
        ///--------------------------------------------------------------------
        public AutomationPattern[] SupportedPatterns
        {
            get
            {
                return supportedPatterns;
            }
            set
            {
                supportedPatterns = value;
            }
        }
        AutomationPattern[] supportedPatterns;

        ///--------------------------------------------------------------------
        /// <summary>
        /// Stores and retrieves the parent automation id.
        /// </summary>
        ///--------------------------------------------------------------------
        public string Parent
        {
            get
            {
                return parent;
            }
            set
            {
                parent = value;
            }
        }
        string parent;

        ///--------------------------------------------------------------------
        /// <summary>
        /// Stores and retrieves the grandparent automation id.
        /// </summary>
        ///--------------------------------------------------------------------
        public string Grandparent
        {
            get
            {
                return grandparent;
            }
            set
            {
                grandparent = value;
            }
        }
        string grandparent;

        ///--------------------------------------------------------------------
        /// <summary>
        /// Stores and retrieves the Automation script action.
        /// </summary>
        ///--------------------------------------------------------------------
        public string ScriptAction
        {
            get
            {
                return scriptAction;
            }
            set
            {
                scriptAction = value;
            }
        }
        string scriptAction;
    }
}
