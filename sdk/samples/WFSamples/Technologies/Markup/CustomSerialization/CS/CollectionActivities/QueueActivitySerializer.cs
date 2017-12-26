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
using System.Workflow.ComponentModel.Serialization;

namespace Microsoft.Samples.Workflow.CustomSerialization
{
    public class QueueActivitySerializer : WorkflowMarkupSerializer
    {
        private QueueSerializer serializer = new QueueSerializer();

        protected override void OnBeforeDeserialize(WorkflowMarkupSerializationManager serializationManager, object obj)
        {
            base.OnBeforeDeserialize(serializationManager, obj);

            serializationManager.AddSerializationProvider(serializer);
        }

        protected override void OnAfterDeserialize(WorkflowMarkupSerializationManager serializationManager, object obj)
        {
            base.OnAfterDeserialize(serializationManager, obj);

            serializationManager.RemoveSerializationProvider(serializer);
        }

        protected override void OnBeforeSerialize(WorkflowMarkupSerializationManager serializationManager, object obj)
        {
            base.OnBeforeSerialize(serializationManager, obj);

            serializationManager.AddSerializationProvider(serializer);
        }

        protected override void OnAfterSerialize(WorkflowMarkupSerializationManager serializationManager, object obj)
        {
            base.OnAfterSerialize(serializationManager, obj);

            serializationManager.RemoveSerializationProvider(serializer);
        }
    }
}
