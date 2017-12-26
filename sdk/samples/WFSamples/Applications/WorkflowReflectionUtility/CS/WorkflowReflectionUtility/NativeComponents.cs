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
using System.Collections.Generic;
using System.Drawing;
using System.Reflection;
using System.Windows.Forms;
using Microsoft.Samples.Workflow.WorkflowReflectionUtility.Reflection;
using System.Globalization;

namespace Microsoft.Samples.Workflow.WorkflowReflectionUtility
{
    /// <summary>
    /// Abstract base class for WorkflowReflectorComponent classes.
    /// </summary>
    public abstract class WorkflowReflectionComponent
    {
        /// <summary>
        /// Initialize the component.
        /// </summary>
        /// <param name="context">Context within which to
        /// perform the initialization.</param>
        public abstract void Initialize(InitializationContext context);
        /// <summary>
        /// Perform reflection with the component.  Do the actual
        /// component work.
        /// </summary>
        /// <param name="context">Context within which to 
        /// perform the reflection.</param>
        public abstract void PerformReflection(ReflectionContext context);

        /// <summary>
        /// Invokes all child components of the current
        /// component.
        /// </summary>
        /// <param name="contextToPass">ReflectionContext that is
        /// passed to all childrens' PerformReflection method.</param>
        protected void ReflectChildComponents(ReflectionContext contextToPass)
        {
            List<Type> childTypes = contextToPass.GetChildComponents(this.GetType());

            foreach (Type childType in childTypes)
            {
                WorkflowReflectionComponent componentInstance = childType.InvokeMember(string.Empty, BindingFlags.CreateInstance, null, null, null, CultureInfo.InvariantCulture) as WorkflowReflectionComponent;

                if (componentInstance == null)
                {
                    throw new ApplicationException("Could not cast component to WorkflowReflectionComponent. Type is: " + childType.FullName);
                }

                componentInstance.PerformReflection(contextToPass);
            }
        }
    }

    /// <summary>
    /// Context for performing initialization of a component.
    /// </summary>
    public class InitializationContext
    {
        private Dictionary<Type, List<Type>> componentMapping;
        private ImageList images;
        private Dictionary<Type, Dictionary<string, int>> imageIndexMapping;

        internal InitializationContext(Dictionary<Type, List<Type>> componentMapping, ImageList images, Dictionary<Type, Dictionary<string, int>> imageIndexMapping)
        {
            this.componentMapping = componentMapping;
            this.imageIndexMapping = imageIndexMapping;
            this.images = images;
        }

        /// <summary>
        /// Add a child component to the list.
        /// </summary>
        /// <param name="parentType">Type of the child's
        /// parent.</param>
        /// <param name="childType">Type of the child
        /// component.</param>
        public void AddChildComponent(Type parentType, Type childType)
        {
            if (!typeof(WorkflowReflectionComponent).IsAssignableFrom(parentType))
            {
                throw new ArgumentException("Parent type must derive from WorkflowReflectionComponent.", "parentType");
            }

            if (!typeof(WorkflowReflectionComponent).IsAssignableFrom(childType))
            {
                throw new ArgumentException("Child type must derive from WorkflowReflectionComponent.", "childType");
            }

            List<Type> childTypes = null;

            if (!componentMapping.TryGetValue(parentType, out childTypes))
            {
                childTypes = new List<Type>();
                componentMapping.Add(parentType, childTypes);
            }

            childTypes.Add(childType);
        }

        /// <summary>
        /// Add an image to the context in which components 
        /// run.
        /// </summary>
        /// <param name="toAdd">Image to be added.</param>
        /// <param name="transparentColor">Color to treat make
        /// transparent when displaying the image.</param>
        /// <param name="owningType">Component type that 
        /// owns the image.</param>
        /// <param name="name">Unique key name for retrieving
        /// the image.</param>
        public void AddImage(Image toAdd, Color transparentColor, Type owningType, string name)
        {
            int imageIndex = images.Images.Add(toAdd, transparentColor);

            Dictionary<string, int> imageIndices = null;

            if (!imageIndexMapping.TryGetValue(owningType, out imageIndices))
            {
                imageIndices = new Dictionary<string, int>();
                imageIndexMapping.Add(owningType, imageIndices);
            }

            imageIndices.Add(name, imageIndex);
        }
    }

    /// <summary>
    /// Context within which a WorkflowReflectorComponent performs
    /// reflection.
    /// </summary>
    public class ReflectionContext
    {
        private WorkflowAssembly workflowAssembly;
        private Dictionary<Type, Dictionary<string, int>> imageIndexMapping;
        private Dictionary<Type, List<Type>> componentMapping;
        private TreeNode currentNode;
        private TreeView treeView;

        internal ReflectionContext(WorkflowAssembly workflowAssembly, Dictionary<Type, Dictionary<string, int>> imageIndexMapping, TreeNode currentNode, Dictionary<Type, List<Type>> componentMapping, TreeView treeView)
        {
            this.workflowAssembly = workflowAssembly;
            this.imageIndexMapping = imageIndexMapping;
            this.currentNode = currentNode;
            this.componentMapping = componentMapping;
            this.treeView = treeView;
        }

        /// <summary>
        /// Gets the assembly being processed.
        /// </summary>
        public WorkflowAssembly WorkflowAssembly
        {
            get
            {
                return workflowAssembly;
            }
        }

        /// <summary>
        /// Gets an image's index from the context.
        /// </summary>
        /// <param name="owningType">Type that owns the image.</param>
        /// <param name="name">Unique key name of the image.</param>
        /// <returns>The index of the image in the TreeView's ImageList.</returns>
        public int GetImageIndex(Type owningType, string name)
        {
            Dictionary<string, int> imageIndices = null;

            if (!imageIndexMapping.TryGetValue(owningType, out imageIndices))
            {
                throw new ArgumentException("Specified type does not own any images.", "owningType");
            }

            int index = -1;

            if (!imageIndices.TryGetValue(name, out index))
            {
                throw new ArgumentException("No image with the given name exists.", "name");
            }

            return index;
        }

        /// <summary>
        /// Gets the current TreeNode being processed.
        /// </summary>
        public TreeNode CurrentTreeNode
        {
            get
            {
                return currentNode;
            }
        }

        /// <summary>
        /// Creates a cloned ReflectionContext with the specified
        /// TreeNode.
        /// </summary>
        /// <param name="newCurrentNode">TreeNode to set as the
        /// CurrentTreeNode.</param>
        /// <returns>The cloned ReflectionContext.</returns>
        public ReflectionContext CreateClone(TreeNode newCurrentNode)
        {
            return new ReflectionContext(workflowAssembly, imageIndexMapping, newCurrentNode, componentMapping, treeView);
        }

        /// <summary>
        /// Get the list of child component types.
        /// </summary>
        /// <param name="parentType">The type of the parent
        /// component.</param>
        /// <returns>The list of child components.</returns>
        public List<Type> GetChildComponents(Type parentType)
        {
            List<Type> childTypes = null;

            if (!componentMapping.TryGetValue(parentType, out childTypes))
            {
                childTypes = new List<Type>();
            }

            return childTypes;
        }

        /// <summary>
        /// Get the TreeView in which the nodes are hosted.
        /// </summary>
        public TreeView TreeView
        {
            get
            {
                return treeView;
            }
        }
    }

    /// <summary>
    /// The component that is considered the root parent component.
    /// This component is invoked by the application to start the
    /// processing and all it does is invoke its children.
    /// </summary>
    public class RootComponent : WorkflowReflectionComponent
    {
        public override void Initialize(InitializationContext context)
        {
            // No-op
        }

        public override void PerformReflection(ReflectionContext context)
        {
            ReflectChildComponents(context);
        }
    }
}