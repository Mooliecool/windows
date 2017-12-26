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
using System.Workflow.Activities;
using System.Workflow.ComponentModel;

namespace Microsoft.Samples.Workflow.Synchronized
{
    public sealed partial class SynchronizedActivityWorkflow
    {
        [System.Diagnostics.DebuggerNonUserCode()]
        private void InitializeComponent()
        {
            parallel1 = new ParallelActivity();
            sequence1 = new SequenceActivity();
            sequence2 = new SequenceActivity();
            begin = new CodeActivity();
            end = new CodeActivity();
            increment1 = new CodeActivity();
            increment2 = new CodeActivity();
            decrement1 = new CodeActivity();
            decrement2 = new CodeActivity();
            SynchronizedDecrement = new SynchronizationScopeActivity();
            SynchronizedIncrement = new SynchronizationScopeActivity();
            System.Collections.Generic.List<string> stringCollection1 = new System.Collections.Generic.List<string>();
            System.Collections.Generic.List<string> stringCollection2 = new System.Collections.Generic.List<string>();

            this.CanModifyActivities = true;
            // parallel
            this.parallel1.Activities.Add(sequence1);
            this.parallel1.Activities.Add(sequence2);
            this.parallel1.Name = "parallel1";

            // sequences
            this.sequence1.Activities.Add(SynchronizedIncrement);
            this.sequence1.Name = "sequence1";
            this.sequence2.Activities.Add(SynchronizedDecrement);
            this.sequence2.Name = "sequence2";

            // both synchronization activities are set to  the same syncMutex string
            // in order for the parallel activity to provide synchronization guarantees for shared data access to count variable
            this.SynchronizedIncrement.Name = "SynchronizedIncrement";
            stringCollection1.Add("syncMutex");
            this.SynchronizedIncrement.SynchronizationHandles = stringCollection1;
            this.SynchronizedIncrement.Activities.Add(increment1);
            this.SynchronizedIncrement.Activities.Add(increment2);

            this.SynchronizedDecrement.Name = "SynchronizedDecrement";
            stringCollection2.Add("syncMutex");
            this.SynchronizedDecrement.SynchronizationHandles = stringCollection2;
            this.SynchronizedDecrement.Activities.Add(decrement1);
            this.SynchronizedDecrement.Activities.Add(decrement2);

            // code activities
            begin.Name = "begin";
            begin.ExecuteCode += new EventHandler(Begin_ExecuteCode);

            end.Name = "end";
            end.ExecuteCode += new EventHandler(End_ExecuteCode);

            increment1.Name = "increment1";
            increment1.ExecuteCode += new EventHandler(Increment1_ExecuteCode);

            increment2.Name = "increment2";
            increment2.ExecuteCode += new EventHandler(Increment2_ExecuteCode);

            decrement1.Name = "decrement1";
            decrement1.ExecuteCode += new EventHandler(Decrement1_ExecuteCode);

            decrement2.Name = "decrement2";
            decrement2.ExecuteCode += new EventHandler(Decrement2_ExecuteCode);

            // workflow activities
            this.Activities.Add(begin);
            this.Activities.Add(parallel1);
            this.Activities.Add(end);
            this.Name = "SynchronizedActivityWorkflow";
            this.CanModifyActivities = false;
        }
        private ParallelActivity parallel1;
        private SequenceActivity sequence1;
        private SequenceActivity sequence2;
        private CodeActivity begin;
        private CodeActivity end;
        private CodeActivity increment1;
        private CodeActivity decrement1;
        private CodeActivity increment2;
        private CodeActivity decrement2;
        private SynchronizationScopeActivity SynchronizedIncrement;
        private SynchronizationScopeActivity SynchronizedDecrement;
    }
}
