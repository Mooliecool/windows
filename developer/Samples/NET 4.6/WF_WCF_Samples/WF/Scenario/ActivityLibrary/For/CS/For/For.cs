//------------------------------------------------------------
// Copyright (c) Microsoft Corporation.  All rights reserved.
//------------------------------------------------------------

using System.Activities;
using System.Collections.ObjectModel;
using System.ComponentModel;
using System.Windows.Markup;

namespace Microsoft.Samples.Activities.Statements
{
    /// <summary>
    /// For is a custom activity that mimics the C# for statement.
    /// </summary>

    [ContentProperty("Body")]
    public sealed class For : NativeActivity
    {
        CompletionCallback onInitComplete;
        CompletionCallback<bool> onConditionComplete;
        CompletionCallback onIterationComplete;
        CompletionCallback onBodyComplete;

        Collection<Variable> variables;

        public For()
            : base()
        {
        }

        // The variables accessible by all 3 clauses and the body of For.
        [DefaultValue(null)]
        public Collection<Variable> Variables
        {
            get
            {
                if (this.variables == null)
                {
                    this.variables = new Collection<Variable>();
                }
                return this.variables;
            }
        }

        [DefaultValue(null)]
        [DependsOn("Variables")]
        public Activity InitAction { get; set; }

        [DefaultValue(null)]
        [DependsOn("InitAction")]
        public Activity<bool> Condition { get; set; }

        [DefaultValue(null)]
        [DependsOn("Condition")]
        public Activity IterationAction { get; set; }

        [DefaultValue(null)]
        [DependsOn("IterationAction")]
        public Activity Body { get; set; }

        protected override void Execute(NativeActivityContext context)
        {
            // The C# For statement is a shortform that includes many other operations defined 
            // in the For clauses.
            // The For notation: for ([init action], [condition], [iteration action]) { [body] } 
            // 
            // can be expanded to the following form:
            // 
            // [init action]
            // while ([condition])
            // {
            //   [body]
            //   [iteration action]
            // }
            // 
            // Therefore we schedule the operations in the following order:
            // InitAction, Condition, Body, IterationAction

            if (this.InitAction != null)
            {
                this.ScheduleInitAction(context);
            }
            else if (this.Condition != null)
            {
                this.ScheduleCondition(context);
            }
            else if (this.Body != null)
            {
                this.ScheduleBody(context);
            }
            else if (this.IterationAction != null)
            {
                this.ScheduleIteration(context);
            }
        }

        // Schedule the initialization clause.
        void ScheduleInitAction(NativeActivityContext context)
        {
            if (this.onInitComplete == null)
            {
                this.onInitComplete = new CompletionCallback(this.OnInitCompleted);
            }

            context.ScheduleActivity(this.InitAction, this.onInitComplete);
        }

        // Once the initialization clause is complete, continue with the remaining clauses.
        void OnInitCompleted(NativeActivityContext context, ActivityInstance completedInstance)
        {
            if (this.Condition != null)
            {
                this.ScheduleCondition(context);
            }
            else if (this.Body != null)
            {
                this.ScheduleBody(context);
            }
            else if (this.IterationAction != null)
            {
                this.ScheduleIteration(context);
            }
        }

        // Schedule the condition clause.
        void ScheduleCondition(NativeActivityContext context)
        {
            if (this.onConditionComplete == null)
            {
                this.onConditionComplete = new CompletionCallback<bool>(this.OnConditionComplete);
            }

            context.ScheduleActivity(this.Condition, this.onConditionComplete);
        }

        // Once the condition clause is complete, continue with the remaining clauses.
        void OnConditionComplete(NativeActivityContext context, ActivityInstance completedInstance, bool result)
        {
            if (result)
            {
                if (this.Body != null)
                {
                    this.ScheduleBody(context);
                }
                else if (this.IterationAction != null)
                {
                    this.ScheduleIteration(context);
                }
                else
                {
                    this.ScheduleCondition(context);
                }
            }
        }

        // Schedule the body operation.
        void ScheduleBody(NativeActivityContext context)
        {
            if (this.onBodyComplete == null)
            {
                this.onBodyComplete = new CompletionCallback(this.OnBodyComplete);
            }

            context.ScheduleActivity(this.Body, this.onBodyComplete);
        }

        // Once the condition clause is complete, continue with the remaining clauses.
        void OnBodyComplete(NativeActivityContext context, ActivityInstance completedInstance)
        {
            if (this.IterationAction != null)
            {
                this.ScheduleIteration(context);
            }
            else if (this.Condition != null)
            {
                this.ScheduleCondition(context);
            }
            else
            {
                this.ScheduleBody(context);
            }
        }

        // Schedule the iteration operation.
        void ScheduleIteration(NativeActivityContext context)
        {
            if (this.onIterationComplete == null)
            {
                this.onIterationComplete = new CompletionCallback(this.OnIterationComplete);
            }

            context.ScheduleActivity(this.IterationAction, this.onIterationComplete);
        }

        // Once the iteration clause is complete, continue with the remaining clauses.
        void OnIterationComplete(NativeActivityContext context, ActivityInstance completedInstance)
        {
            if (this.Condition != null)
            {
                this.ScheduleCondition(context);
            }
            else if (this.Body != null)
            {
                this.ScheduleBody(context);
            }
            else
            {
                this.ScheduleIteration(context);
            }
        }
    }
}
