//----------------------------------------------------------------
// Copyright (c) Microsoft Corporation.  All rights reserved.
//----------------------------------------------------------------

using System.Activities;
using System.Collections.Generic;
using System.Data.Objects;
using System.Linq;

namespace Microsoft.Samples.Activities.Data
{
    
    public class EntitySqlQuery<TResult> : NativeActivity<IEnumerable<TResult>>
    {
        IDictionary<string, Argument> parameters;
       
        [RequiredArgument]
        public InArgument<string> EntitySql { get; set; }       
        
        public IDictionary<string, Argument> Parameters
        {
            get 
            {
                if (this.parameters == null)
                {
                    this.parameters = new Dictionary<string, Argument>();
                }
                return parameters;
            }            
        }

        public EntitySqlQuery()
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

            ObjectQuery<TResult> query;

            // setup parameters            
            if (this.parameters.Count() > 0)
            {
                ObjectParameter[] objectParameters = new ObjectParameter[this.Parameters.Count()];
                int i = 0;
                foreach (KeyValuePair<string, Argument> item in this.Parameters)
                {
                    objectParameters[i] = new ObjectParameter(item.Key, item.Value.Get(context));
                    i++;
                }
                query = efObjectContext.CreateQuery<TResult>(EntitySql.Get(context), objectParameters); 
            }
            else
            {
                query = efObjectContext.CreateQuery<TResult>(EntitySql.Get(context)); 
            }            

            // set the result value
            Result.Set(context, query.ToList<TResult>());
        }
    }    
}
