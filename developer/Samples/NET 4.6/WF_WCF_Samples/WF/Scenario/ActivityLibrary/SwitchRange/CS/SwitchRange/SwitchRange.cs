//----------------------------------------------------------------
// Copyright (c) Microsoft Corporation.  All rights reserved.
//----------------------------------------------------------------
using System;
using System.Activities;
using System.Collections.Generic;
using System.ComponentModel;
using System.Globalization;
using System.Windows.Markup;

namespace Microsoft.Samples.Activities.Statements
{

    /// <summary>
    /// This activity allows switching on a range of values.     
    /// </summary>
    /// <typeparam name="T">Type of the values in the range</typeparam>
    [ContentProperty("Cases")]
    public sealed class SwitchRange<T> : NativeActivity where T : IComparable
    {
        IList<CaseRange<T>> cases;

        public SwitchRange()
        {
        }

        [RequiredArgument]
        [DefaultValue(null)]
        public InArgument<T> Expression { get; set; }

        public IList<CaseRange<T>> Cases
        {
            get
            {
                if (this.cases == null)
                {
                    this.cases = new List<CaseRange<T>>();
                }
                return this.cases;
            }
        }

        [DefaultValue(null)]
        public Activity Default { get; set; }
               
        protected override void CacheMetadata(NativeActivityMetadata metadata)
        {
            base.CacheMetadata(metadata);
            
            // Add CaseRanges and Default as children of SwitchRange so they can be scheduled
            foreach (CaseRange<T> range in Cases)
            {
                metadata.AddChild(range.Action);
            }

            // Perform validations
            int malformedRanges = 0;
            foreach (CaseRange<T> range in Cases)
            {
                if (range.From == null || range.To == null)
                {
                    malformedRanges++;
                }
            }
   
            if (malformedRanges > 0)
            {
                metadata.AddValidationError(string.Format(CultureInfo.InvariantCulture, "SwitchRange {0}: There is/are {1} CaseRange(s) that have either From or To unspecified", DisplayName, malformedRanges));
            }            
        }

        // Evaluate the expression, find a matching CaseRange or Default if specified and schedule that for execution
        protected override void Execute(NativeActivityContext context)
        {
            // Evaluate the expression for the switch
            T result = (T)this.Expression.Get(context);

            // Find the case that best matches the result of the expression and schedule it (if found)
            Activity matchingCase = FindMatchingCase(result);            
            if (matchingCase != null)
            {
                context.ScheduleActivity(matchingCase);
            }
        }
 
        // Iterate through the cases collection and find the first CaseRange in the range 
        // of the result of the expression (result argument of this method)
        Activity FindMatchingCase(T result)
        {
            Activity matchingCase = null;
            
            // Iterate through each CaseRange to check if the result falls within the range
            foreach (CaseRange<T> caseRange in Cases)
            {
                if (caseRange.IsInRange(result))
                {
                    matchingCase = caseRange.Action;
                    break;
                }
            }

            // If no matching CaseRange is found, then choose Default
            if (matchingCase == null && Default != null)
            {
                matchingCase = Default;
            }

            return matchingCase;
        }
    }
}

