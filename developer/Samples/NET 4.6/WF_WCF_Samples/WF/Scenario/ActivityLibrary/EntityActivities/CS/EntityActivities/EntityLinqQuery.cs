//----------------------------------------------------------------
// Copyright (c) Microsoft Corporation.  All rights reserved.
//----------------------------------------------------------------

using System;
using System.Activities;
using System.Collections.Generic;
using System.Data.Objects;
using System.Linq;

namespace Microsoft.Samples.Activities.Data
{

    public class EntityLinqQuery<TResult> : NativeActivity<IEnumerable<TResult>> where TResult : class
    {
        public InArgument<Func<TResult, bool>> Predicate { get; set; }

        public EntityLinqQuery()
        {
            this.Constraints.Add(ConstraintHelper.VerifyParentIsObjectContextScope(this));
        }

        protected override void Execute(NativeActivityContext context)
        {
            // get the ambient Entity Framework object context
            ObjectContext efObjectContext = context.Properties.Find(ObjectContextScope.ObjectContextPropertyName) as ObjectContext;
            if (efObjectContext == null)
            {
                throw new ValidationException("Entity Framework Object Context not found");
            }

            // execute the query            
            var query = efObjectContext.CreateObjectSet<TResult>();
            
            // apply the predicate to the list
            var output = query.Where(this.Predicate.Get(context));

            // set the result value
            this.Result.Set(context, output.ToList<TResult>());            
        }       
    }
}
