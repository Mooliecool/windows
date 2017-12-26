//---------------------------------------------------------------------
//  This file is part of the Windows Workflow Foundation SDK Code Samples.
// 
//  Copyright (C) Microsoft Corporation.  All rights reserved.
// 
//This source code is intended only as a supplement to Microsoft
//Development Tools and/or on-line documentation.  See these other
//materials for detailed information regarding Microsoft code samples.
// 
//THIS CODE AND INFORMATION ARE PROVIDED AS IS WITHOUT WARRANTY OF ANY
//KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
//IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
//PARTICULAR PURPOSE.
//---------------------------------------------------------------------

using System;
using System.Configuration;

namespace Microsoft.Samples.Workflow.WorkflowReflectionUtility
{
    /// <summary>
    /// WorkflowReflector configuration section.
    /// </summary>
    [System.Runtime.InteropServices.ComVisible(false)]
    public class WorkflowReflectionUtilitySection : ConfigurationSection
    {
        /// <summary>
        /// Default constructor.
        /// </summary>
        public WorkflowReflectionUtilitySection()
        {
        }

        /// <summary>
        /// Collection of WorkflowReflectorComponent types added
        /// through the components tag.
        /// </summary>
        [ConfigurationProperty("components")]
        public ReflectionComponentElementCollection Components
        {
            get
            {
                return base["components"] as ReflectionComponentElementCollection;
            }
        }

        [ConfigurationProperty("showAssemblyResolveExplanation")]
        public bool ShowAssemblyResolveExplanation
        {
            get
            {
                return (bool)base["showAssemblyResolveExplanation"];
            }
            set
            {
                base["showAssemblyResolveExplanation"] = value;
            }
        }
    }

    /// <summary>
    /// A WorkflowReflectorComponent element.
    /// </summary>
    [System.Runtime.InteropServices.ComVisible(false)]
    public class ReflectionComponentElement : ConfigurationElement
    {
        /// <summary>
        /// Default constructor.
        /// </summary>
        public ReflectionComponentElement()
        {
        }

        /// <summary>
        /// Constructor which takes the WorkflowReflectorComponent
        /// type.
        /// </summary>
        /// <param name="type"></param>
        public ReflectionComponentElement(Type type)
        {
            this.Type = type.AssemblyQualifiedName;
        }

        /// <summary>
        /// Type of the WorkflowReflectorComponent.
        /// Set through the type attribute.
        /// </summary>
        [ConfigurationProperty("type")]
        public string Type
        {
            get
            {
                return base["type"] as string;
            }
            set
            {
                base["type"] = value;
            }
        }
    }

    /// <summary>
    /// Collection of ReflectorComponentElements.
    /// </summary>
    [System.Runtime.InteropServices.ComVisible(false)]
    public class ReflectionComponentElementCollection : ConfigurationElementCollection
    {
        /// <summary>
        /// Create a new, empty element.
        /// </summary>
        /// <returns></returns>
        protected override ConfigurationElement CreateNewElement()
        {
            return new ReflectionComponentElement();
        }

        /// <summary>
        /// Get the key by which the element will be added
        /// to the collection.
        /// </summary>
        /// <param name="element">Element whose key is to be
        /// retreived.</param>
        /// <returns>The element's key.</returns>
        protected override object GetElementKey(ConfigurationElement element)
        {
            return ((ReflectionComponentElement)element).Type;
        }

        /// <summary>
        /// Explicit add of an element to the collection.
        /// </summary>
        /// <param name="element"></param>
        public void Add(ReflectionComponentElement element)
        {
            BaseAdd(element);
        }
    }
}