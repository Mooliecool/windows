//----------------------------------------------------------------
// Copyright (c) Microsoft Corporation.  All rights reserved.
//----------------------------------------------------------------
using System;
using System.Activities;
using System.Collections.Generic;
using System.Data.Linq;
using System.Linq;

namespace Microsoft.Samples.Activities.Data
{

    /// <summary>
    /// This activity retrieves domain entities from a Sql Server datasource using Linq To Sql
    /// It receives a connection string and a LINQ predicate and returns all the elements in the collection 
    /// that satisfy that predicate
    /// </summary>
    /// <typeparam name="TResult">Type of the items of the collection</typeparam>
    public sealed class FindInSqlTable<TResult>: CodeActivity<IList<TResult>> where TResult: class
    {
        public string ConnectionString { get; set; }

        public InArgument<Func<TResult, bool>> Predicate { get; set; }                
                
        public FindInSqlTable()
        {}

        protected override void CacheMetadata(CodeActivityMetadata metadata)
        {
            if (ConnectionString == null)
            {
                metadata.AddValidationError("Connection string can't be null");
            }

            base.CacheMetadata(metadata);
        }

        protected override IList<TResult> Execute(CodeActivityContext context)
        {            
            DataContext dataContext = new DataContext(ConnectionString);
            IQueryable<TResult> table = dataContext.GetTable<TResult>();

            IEnumerable<TResult> q =
                (this.Predicate.Get(context) == null)
                    ? from row in table select row
                    : table.Where<TResult>(this.Predicate.Get(context));

            return new List<TResult>(q);         
        }       
    }
}
