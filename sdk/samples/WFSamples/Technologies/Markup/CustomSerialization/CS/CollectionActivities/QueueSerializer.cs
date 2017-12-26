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
using System.Collections;
using System.ComponentModel.Design.Serialization;
using System.Workflow.ComponentModel.Serialization;

namespace Microsoft.Samples.Workflow.CustomSerialization
{
    public class QueueSerializer : WorkflowMarkupSerializer, IDesignerSerializationProvider
    {
        protected override void AddChild(WorkflowMarkupSerializationManager serializationManager, object parentObject, object childObject)
        {
            if (parentObject == null)
                throw new ArgumentNullException("parentObject");

            if (childObject == null)
                throw new ArgumentNullException("childObject");

            Queue queue = parentObject as Queue;
            if (queue == null)
                throw new Exception(string.Format("The type of parentObject is not {0}", typeof(Queue).FullName));

            queue.Enqueue(childObject);
        }

        protected override IList GetChildren(WorkflowMarkupSerializationManager serializationManager, object obj)
        {
            if (obj == null)
                throw new ArgumentNullException("obj");

            Queue queue = obj as Queue;
            if (queue == null)
                throw new Exception(string.Format("The type of obj is not {0}", typeof(Queue).FullName));

            ArrayList arrayList = new ArrayList(queue.ToArray());
            return arrayList;
        }

        #region IDesignerSerializationProvider Members

        public object GetSerializer(IDesignerSerializationManager manager, object currentSerializer, Type objectType, Type serializerType)
        {
            if (objectType == typeof(Queue))
                return this;
            else
                return currentSerializer;
        }

        #endregion
    }
}
