//----------------------------------------------------------------
// Copyright (c) Microsoft Corporation.  All rights reserved.
//----------------------------------------------------------------

using System.Activities;
using System.Collections.ObjectModel;
using System.ComponentModel;
using System.Data.Objects;
using System.Windows.Markup;

namespace Microsoft.Samples.Activities.Data
{

    [ContentProperty("Body")]
    public class ObjectContextScope : NativeActivity
    {
        internal static string ObjectContextPropertyName = "_EF_ObjectContext";

        Variable<NoPersistHandle> noPersistHandle;
        Collection<Variable> variables;
        ObjectContext efContext;

        public ObjectContextScope()
            : base()
        {
            this.noPersistHandle = new Variable<NoPersistHandle>();
        }

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

        public Activity Body { get; set; }

        [RequiredArgument]        
        [DefaultValue(null)]
        public InArgument<string> ConnectionString { get ; set; }

        [RequiredArgument]
        public InArgument<string> ContainerName { get; set; }

        protected override void CacheMetadata(NativeActivityMetadata metadata)
        {
            // child activities
            metadata.AddChild(this.Body);            
            
            // arguments
            metadata.AddArgument(new RuntimeArgument("ConnectionString", typeof(string), ArgumentDirection.In, true));            

            // variables
            metadata.AddImplementationVariable(this.noPersistHandle);            
            foreach (Variable variable in this.variables)
            {
                metadata.AddVariable(variable);
            }

            base.CacheMetadata(metadata);
        }
       
        protected override void Execute(NativeActivityContext context)
        {
            // enter a no persist scope
            NoPersistHandle noPersistHandle = this.noPersistHandle.Get(context);
            noPersistHandle.Enter(context);

            // get the connection string
            string connectionString = this.ConnectionString.Get(context);

            // create the connection context
            this.efContext = new ObjectContext(connectionString);
            
            // use NorthwindEntities as the container name for this example until the default container feature is in
            this.efContext.DefaultContainerName = this.ContainerName.Get(context);
            
            // set the object context in the execution properties (to make it an ambient object context)
            context.Properties.Add(ObjectContextPropertyName, efContext);

            // schedule the body activity
            context.ScheduleActivity(this.Body, new CompletionCallback(OnBodyComplete));
        }

        void OnBodyComplete(NativeActivityContext context, ActivityInstance completedActivity)
        {
            if (efContext != null)
            {
                efContext.SaveChanges();
                efContext.Dispose();
            }         
        }       
    }
}
