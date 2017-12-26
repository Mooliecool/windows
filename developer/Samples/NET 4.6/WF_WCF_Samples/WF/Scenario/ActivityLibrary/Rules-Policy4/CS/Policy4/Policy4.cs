//-----------------------------------------------------------------------------
// Copyright (c) Microsoft Corporation.  All rights reserved.
//-----------------------------------------------------------------------------

using System;
using System.Activities;
using System.ComponentModel;
using System.Workflow.Activities.Rules;
using System.Workflow.ComponentModel.Compiler;

namespace Microsoft.Samples.Activities.Rules
{
    [Designer(typeof(Microsoft.Samples.Activities.Rules.Policy4Designer))]
    public sealed class Policy4<TResult> : CodeActivity<TResult>
    {
        public RuleSet RuleSet { get; set; }

        [RequiredArgument]
        public InArgument<TResult> Input{ get; set; }
        
        public OutArgument<ValidationErrorCollection> ValidationErrors { get; set; }

        protected override void CacheMetadata(CodeActivityMetadata metadata)
        {
            if (this.RuleSet == null)
            {
                metadata.AddValidationError("RuleSet property can't be null");
            }

            base.CacheMetadata(metadata);
        }

        protected override TResult Execute(CodeActivityContext context)
        {
            // validate before running
            Type targetType = this.Input.Get(context).GetType();
            RuleValidation validation = new RuleValidation(targetType, null);
            if (!this.RuleSet.Validate(validation))
            {
                // set the validation error out argument
                this.ValidationErrors.Set(context, validation.Errors);

                // throw a validation exception
                throw new ValidationException(string.Format("The ruleset is not valid. {0} validation errors found (check the ValidationErrors property for more information).", validation.Errors.Count));
            }

            // execute the ruleset
            TResult evaluatedTarget = this.Input.Get(context);
            RuleEngine engine = new RuleEngine(this.RuleSet, validation);
            engine.Execute(evaluatedTarget);            
            return evaluatedTarget;            
        }        
    }
}