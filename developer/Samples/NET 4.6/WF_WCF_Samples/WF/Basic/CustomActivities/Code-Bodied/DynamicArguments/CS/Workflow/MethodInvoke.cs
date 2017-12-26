//----------------------------------------------------------------
// Copyright (c) Microsoft Corporation.  All rights reserved.
//----------------------------------------------------------------

using System;
using System.Activities;
using System.Collections.ObjectModel;
using System.Linq;
using System.Reflection;

namespace Microsoft.Samples.DynamicArguments
{

    public sealed class MethodInvoke : CodeActivity
    {
        Collection<Argument> parameters;
        RuntimeArgument resultArgument;
        MethodInfo methodInfo;

        public MethodInvoke()
        {
            this.parameters = new Collection<Argument>();
        }

        public string MethodName { get; set; }
        public Type TargetType { get; set; }

        public Collection<Argument> Parameters
        {
            get
            {
                return this.parameters;
            }
        }

        public OutArgument Result { get; set; }

        protected override void CacheMetadata(CodeActivityMetadata metadata)
        {
            Collection<RuntimeArgument> arguments = new Collection<RuntimeArgument>();

            Type resultType = this.Result != null ? this.Result.ArgumentType : typeof(object);

            //create a new RuntimeArgument to handle any result from method invocation
            this.resultArgument = new RuntimeArgument("Result", resultType, ArgumentDirection.Out);
            //bind it to the user provided OutArgument
            metadata.Bind(Result, this.resultArgument);
            //add it to this activity's collection of arguments
            arguments.Add(this.resultArgument);

            //Determine the correct MethodInfo
            this.methodInfo = DetermineMethodInfo(metadata);            

            if (this.methodInfo != null)
            {
                //create RuntimeArguments for each ParameterInfo on the returned MethodInfo
                //bind the user provided arguments to the RuntimeArguments
                ParameterInfo[] formalParameters;
                int formalParameterCount;

                formalParameters = this.methodInfo.GetParameters();
                formalParameterCount = formalParameters.Length;

                for (int i = 0; i < formalParameterCount; i++)
                {
                    string name = formalParameters[i].Name;

                    RuntimeArgument argument = new RuntimeArgument(name, Parameters[i].ArgumentType, Parameters[i].Direction, true);
                    metadata.Bind(Parameters[i], argument);
                    arguments.Add(argument);
                }
            }

            //Set the arguments collection that has been built as part of the Activity's metadata
            metadata.SetArgumentsCollection(arguments);
        }

        protected override void Execute(CodeActivityContext context)
        {
            object targetInstance = null;

            if (!this.methodInfo.IsStatic)
            {
                targetInstance = Activator.CreateInstance(this.TargetType);
            }

            ParameterInfo[] formalParameters = this.methodInfo.GetParameters();
            int formalParamCount = formalParameters.Length;
            object[] actualParameters = new object[formalParamCount];

            for (int i = 0; i < formalParamCount; i++)
            {
                //evaluate each user provided parameter
                actualParameters[i] = Parameters[i].Get<object>(context);
            }

            object result = null;

            try
            {
                //invoke the methodInfo
                result = this.methodInfo.Invoke(targetInstance, actualParameters);
            }
            catch (TargetInvocationException e)
            {
                throw e.InnerException;
            }

            for (int index = 0; index < Parameters.Count; index++)
            {
                //set any Out or InOut parameters
                if (Parameters[index].Direction != ArgumentDirection.In)
                {
                    Parameters[index].Set(context, actualParameters[index]);
                }
            }

            //set the result
            this.resultArgument.Set(context, result);
        }

        MethodInfo DetermineMethodInfo(CodeActivityMetadata metadata)
        {
            bool failedValidation = false;

            if (string.IsNullOrEmpty(this.MethodName))
            {
                //if no MethodName was specified - then this activity has been misconfigured - add a validation error
                metadata.AddValidationError("MethodName was not set on " + this.DisplayName);
                failedValidation = true;
            }
            Type targetType = this.TargetType;

            if (targetType == null)
            {
                //if no TargetType was specified - then this activity has been misconfigured - add a validation error
                metadata.AddValidationError("TargetType was not set on " + this.DisplayName);
                failedValidation = true;
            }

            if (failedValidation)
            {
                return null;
            }

            MethodInfo info;

            //setup the array of parameter types based on the Parameters collection of Arguments.
            Type[] parameterTypes = this.Parameters.Select<Argument, Type>(argument => argument.Direction == ArgumentDirection.In ? argument.ArgumentType : argument.ArgumentType.MakeByRefType()).ToArray<Type>();

            Type resultType = this.Result == null ? null : this.Result.ArgumentType;

            try
            {
                //try to acquire the MethodInfo from the targetType based on the provided MethodName and the array of parameterTypes
                info = targetType.GetMethod(this.MethodName, parameterTypes);
            }
            catch (AmbiguousMatchException)
            {
                //add a ValidationError in the face of an ambiguous match
                metadata.AddValidationError(string.Format("Could not resolve {0} on {1} to a single method. Activity: {2}", this.MethodName, targetType.Name, this.DisplayName));
                return null;
            }
            if (info == null)
            {
                //add a ValidationError if the Method could not be found
                metadata.AddValidationError(string.Format("Public Method {0} with matching parameter list does not exist on {1}.  Activity: {2}", this.MethodName, targetType.Name, this.DisplayName));
                return null;
            }
            else if (resultType != null && resultType != info.ReturnType)
            {
                //add a ValidationError if the return types don't match
                metadata.AddValidationError(string.Format("The return Type {0} of method {1} on type {2} does not match the return type {3} of the activity {4}",
                    info.ReturnType.Name, this.MethodName, targetType.Name, resultType == null ? "void" : resultType.Name , this.DisplayName));
            }
            return info;
        }
    }
}
