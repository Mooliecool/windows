//----------------------------------------------------------------
// Copyright (c) Microsoft Corporation.  All rights reserved.
//----------------------------------------------------------------

using System.Activities;
using System.Collections.Generic;
using System.Data.Objects;

namespace Microsoft.Samples.Activities.Data
{

    public class EntityAdd<T> : NativeActivity where T : class
    {
        [RequiredArgument]
        [OverloadGroup("SingleEntity")]
        public InArgument<T> Entity { get; set; }

        [RequiredArgument]
        [OverloadGroup("MultipleEntities")]
        public InArgument<IEnumerable<T>> Entities { get; set; }

        public EntityAdd()
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

            var objectSet = efObjectContext.CreateObjectSet<T>();

            if (this.Entity.Get(context) == null)
            {
                foreach (T entity in this.Entities.Get(context))
                {
                    objectSet.AddObject(entity);
                }
            }
            else
            {
                objectSet.AddObject(this.Entity.Get(context));
            }
        }
    }
}
