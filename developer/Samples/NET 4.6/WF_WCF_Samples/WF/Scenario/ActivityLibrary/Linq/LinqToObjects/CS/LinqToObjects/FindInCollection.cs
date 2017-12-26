//-----------------------------------------------------------------------------
// Copyright (c) Microsoft Corporation.  All rights reserved.
//-----------------------------------------------------------------------------
using System;
using System.Activities;
using System.Collections;
using System.Collections.Generic;
using System.Linq;

namespace Microsoft.Samples.Activities.Statements
{

    /// <summary>
    /// This activity finds items in a collection using Linq To Objects 
    /// It receives a collection an a predicate and return all the elements in the collection 
    /// that satisfy that predicate
    /// </summary>
    /// <typeparam name="TResult">Type of the items of the collection</typeparam>
    public sealed class FindInCollection<TResult> : CodeActivity<IList<TResult>> where TResult : class
    {
        [RequiredArgument]
        public InArgument<IEnumerable<TResult>> Collections { get; set; }

        [RequiredArgument]
        public InArgument<Func<TResult, bool>> Predicate { get; set; }       

        public FindInCollection()
        {
        }

        protected override IList<TResult> Execute(CodeActivityContext context)
        {            
            IEnumerable<TResult> q = 
                    from row in this.Collections.Get(context) 
                    where this.Predicate.Get(context)(row)
                    select row;

            return new List<TResult>(q);
        }
    }
}
