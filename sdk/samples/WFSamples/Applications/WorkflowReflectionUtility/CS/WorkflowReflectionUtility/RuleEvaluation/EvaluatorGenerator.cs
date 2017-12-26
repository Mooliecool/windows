//---------------------------------------------------------------------
//  This file is part of the Windows Workflow Foundation SDK Code Samples.
// 
//  Copyright (C) Microsoft Corporation.  All rights reserved.
// 
//This source code is intended only as a supplement to Microsoft
//Development Tools and/or on-line documentation.  See these other
//materials for detailed information regarding Microsoft code samples.
// 
//THIS CODE AND INFORMATION ARE PROVIDED AS IS WITHOUT WARRANTY OF ANY
//KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
//IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
//PARTICULAR PURPOSE.
//---------------------------------------------------------------------

namespace Microsoft.Samples.Workflow.WorkflowReflectionUtility.Rules
{
    using System;
    using System.CodeDom;
    using System.CodeDom.Compiler;
    using System.Collections.Generic;
    using System.IO;
    using System.Reflection;
    using System.Runtime.Serialization.Formatters.Binary;
    using System.Globalization;

    /// <summary>
    /// Data attached to each node of a CodeExpression
    /// tree in order to support generation and use
    /// of the IExpressionEvaluator.
    /// </summary>
    [Serializable]
    public class EvaluationData
    {
        private Type typeValue;
        private int parameterIndexValue;

        /// <summary>
        /// Get or set the type this node evaluates to.
        /// </summary>
        public Type Type
        {
            get
            {
                return this.typeValue;
            }
            set
            {
                this.typeValue = value;
            }
        }

        /// <summary>
        /// Get or set this node's index into the parameter array.
        /// </summary>
        public int ParameterIndex
        {
            get
            {
                return this.parameterIndexValue;
            }
            set
            {
                this.parameterIndexValue = value;
            }
        }

        /// <summary>
        /// Default contstructor.  Sets the values to defaults.
        /// </summary>
        public EvaluationData()
        {
            this.Type = null;
            this.ParameterIndex = -1;
        }

        /// <summary>
        /// Set the fields to the specified values.
        /// </summary>
        /// <param name="type">Type to which this CodeExpression
        /// evaluates</param>
        /// <param name="parameterIndex">Index into the parameter
        /// collection which corresponds to this CodeExpression</param>
        public EvaluationData(Type type, int parameterIndex)
        {
            this.Type = type;
            this.ParameterIndex = parameterIndex;
        }
    }

    /// <summary>
    /// Interface used for expression evaluators.
    /// </summary>
    public interface IExpressionEvaluator
    {
        /// <summary>
        /// Create a new EvaluatorParameters object with
        /// all parameters empty.
        /// </summary>
        /// <returns></returns>
        EvaluatorParameters CreateEmptyParameters();
        /// <summary>
        /// Evaluate the expression with the specified
        /// parameters.
        /// </summary>
        /// <param name="parameters">Replacement parameters
        /// to use for the evaluation.</param>
        /// <returns>The object result of the evaluation</returns>
        object Evaluate(EvaluatorParameters parameters);
        /// <summary>
        /// Evaluate the subexpression with the specified
        /// parameters.
        /// </summary>
        /// <param name="expressionParamaterNumber">Parameter number
        /// corresponding to the subexpression which is to be
        /// evaluated.</param>
        /// <param name="parameters">Replacement parameters
        /// to use for evaluation.</param>
        /// <returns>The object result of the evaluation</returns>
        object EvaluateSubExpression(int expressionParameterNumber, EvaluatorParameters parameters);
    }

    /// <summary>
    /// Abstract base class for IExpressionEvaluator classes.  Provides
    /// some helpers for namely the implementation of CreateEmptyParameters()
    /// which makes use of internal classes.
    /// </summary>
    public abstract class ExpressionEvaluatorBase : IExpressionEvaluator
    {
        private int parameterCountValue;

        /// <summary>
        /// Total number of parameters which need to be supplied
        /// to the evaluator.
        /// </summary>
        protected int ParameterCount
        {
            get
            {
                return this.parameterCountValue;
            }
            set
            {
                this.parameterCountValue = value;
            }
        }

        /// <summary>
        /// Create a new EvaluatorParameters object with
        /// all parameters empty.
        /// </summary>
        /// <returns></returns>
        public EvaluatorParameters CreateEmptyParameters()
        {
            return new EvaluatorParameters(ParameterCount);
        }

        /// <summary>
        /// Evaluate the subexpression with the specified
        /// parameters.
        /// </summary>
        /// <param name="expressionParameterNumber">Parameter number
        /// corresponding to the subexpression which is to be
        /// evaluated.</param>
        /// <param name="parameters">Replacement parameters
        /// to use for evaluation.</param>
        /// <returns>The object result of the evaluation</returns>
        public object EvaluateSubExpression(int expressionParameterNumber, EvaluatorParameters parameters)
        {
            MethodInfo methodInfo = this.GetType().GetMethod(string.Format("Eval{0}", expressionParameterNumber), BindingFlags.Instance | BindingFlags.InvokeMethod | BindingFlags.NonPublic);

            if (methodInfo == null)
            {
                throw new ArgumentException("Could not find a matching evaluation method for that number.", "expressionParameterNumber");
            }

            return methodInfo.Invoke(this, new object[] { parameters.Parameters });
        }

        /// <summary>
        /// Evaluate the expression with the specified
        /// parameters.
        /// </summary>
        /// <param name="parameters">Replacement parameters
        /// to use for the evaluation.</param>
        /// <returns>The object result of the evaluation</returns>
        public abstract object Evaluate(EvaluatorParameters parameters);

        /// <summary>
        /// Helper method to check whether a specified
        /// object represents an Empty parameter.
        /// </summary>
        /// <param name="parameter">Object to check.</param>
        /// <returns>True if the object means empty, false otherwise.</returns>
        protected bool IsEmpty(object parameter)
        {
            if (parameter is EvalNull)
            {
                return true;
            }
            else
            {
                return false;
            }
        }
    }

    internal class EvalNull
    {
    }

    /// <summary>
    /// Collection of replacement parameters used in
    /// evaluation of an IExpressionEvaluator.
    /// </summary>
    public class EvaluatorParameters
    {
        private object[] parametersValue;

        internal EvaluatorParameters(int parameterCount)
        {
            this.parametersValue = new object[parameterCount];

            for (int i = 0; i < parameterCount; i++)
            {
                this.parametersValue[i] = new EvalNull();
            }
        }

        /// <summary>
        /// Set a parameter to the specified value.
        /// </summary>
        /// <param name="parameterNumber">Index into the
        /// parameter list (taken from EvaluationData).</param>
        /// <param name="value">Value to which the parameter
        /// should be set.</param>
        public void SetParameter(int parameterNumber, object value)
        {
            this.parametersValue[parameterNumber] = value;
        }

        /// <summary>
        /// Set a parameter to Empty.
        /// </summary>
        /// <param name="parameterNumber">Index into the
        /// parameter list (taken from EvaluationData).</param>
        public void SetParameterEmpty(int parameterNumber)
        {
            this.parametersValue[parameterNumber] = new EvalNull();
        }

        /// <summary>
        /// Get the array of parameters.
        /// </summary>
        public object[] Parameters
        {
            get
            {
                return this.parametersValue;
            }
        }
    }

    /// <summary>
    /// Static generator of IExpressionEvaluator objects.
    /// </summary>
    public static class EvaluatorGenerator
    {
        /// <summary>
        /// Helper method used for setting the type of
        /// a CodeExpression through use of EvaluationData.
        /// </summary>
        /// <param name="toSet">CodeExpression to adorn with
        /// type information.</param>
        /// <param name="newType">Type of the CodeExpression's result.</param>
        public static void SetType(CodeObject toSet, Type newType)
        {
            EvaluationData data = toSet.UserData[typeof(EvaluationData)] as EvaluationData;

            if (data == null)
            {
                data = new EvaluationData();
                toSet.UserData.Add(typeof(EvaluationData), data);
            }

            data.Type = newType;
        }

        /// <summary>
        /// Helper method used for getting the type of
        /// a CodeExpression through use of EvaluationData.
        /// </summary>
        /// <param name="toGet">CodeExpression which is adorned
        /// with type information.</param>
        /// <param name="throwIfNotExist">Whether to throw an
        /// exception if the type data is not available.  True to
        /// throw.</param>
        /// <returns>The type of the result of the CodeExpression.</returns>
        public static Type GetType(CodeObject toGet, bool throwIfNotExist)
        {
            EvaluationData data = toGet.UserData[typeof(EvaluationData)] as EvaluationData;

            if (data == null)
            {
                if (throwIfNotExist)
                {
                    throw new ApplicationException("The expression did not contain an EvaluationData object.");
                }

                return null;
            }
            else
            {
                return data.Type;
            }
        }

        /// <summary>
        /// Recursively initialize the types of the CodeExpression
        /// assuming that either the leaf expressions have already
        /// been adorned with types or that CodeThisReferenceExpressions
        /// are the target of all leaf nodes.
        /// </summary>
        /// <param name="root"></param>
        /// <param name="thisType"></param>
        private static void InitializeTypes(CodeExpression root, Type thisType)
        {
            if (root is CodeBinaryOperatorExpression)
            {
                CodeBinaryOperatorExpression binaryRoot = (CodeBinaryOperatorExpression)root;

                // Do the recursive call.
                InitializeTypes(binaryRoot.Left, thisType);
                InitializeTypes(binaryRoot.Right, thisType);

                switch (binaryRoot.Operator)
                {
                    case CodeBinaryOperatorType.Add:
                    case CodeBinaryOperatorType.BitwiseAnd:
                    case CodeBinaryOperatorType.BitwiseOr:
                    case CodeBinaryOperatorType.Divide:
                    case CodeBinaryOperatorType.Modulus:
                    case CodeBinaryOperatorType.Multiply:
                    case CodeBinaryOperatorType.Subtract:
                        // Assume that the result will be the type of the
                        // left branch.  This is NOT always true: Consider
                        // adding a decimal to an integer.
                        SetType(root, GetType(binaryRoot.Left, true));
                        break;
                    case CodeBinaryOperatorType.BooleanAnd:
                    case CodeBinaryOperatorType.BooleanOr:
                    case CodeBinaryOperatorType.GreaterThan:
                    case CodeBinaryOperatorType.GreaterThanOrEqual:
                    case CodeBinaryOperatorType.IdentityEquality:
                    case CodeBinaryOperatorType.IdentityInequality:
                    case CodeBinaryOperatorType.LessThan:
                    case CodeBinaryOperatorType.LessThanOrEqual:
                    case CodeBinaryOperatorType.ValueEquality:
                        // These operations always result in a Boolean
                        SetType(root, typeof(bool));
                        break;
                }
            }
            else
            {
                SetType(root, FindTargetType(root, thisType)); 
            }
        }

        /// <summary>
        /// Track down the real type of a CodeExpression's target.
        /// </summary>
        /// <param name="targetObject"></param>
        /// <param name="thisType"></param>
        /// <returns></returns>
        private static Type FindTargetType(CodeExpression targetObject, Type thisType)
        {
            Type targetType = GetType(targetObject, false);

            if (targetType != null)
            {
                return targetType;
            }
            else if (targetObject is CodeVariableReferenceExpression)
            {
                throw new ApplicationException("Cannot determine the type of a CodeVariableReferenceExpression: " + Serialize(targetObject));
            }
            else if (targetObject is CodeTypeOfExpression)
            {
                // Results in a System.Type.
                return typeof(Type);
            }
            else if (targetObject is CodeThisReferenceExpression)
            {
                if (thisType == null)
                {
                    throw new ApplicationException("There is a CodeThisReferenceExpression and the thisType supplied was null.");
                }

                return thisType;
            }
            else if (targetObject is CodePrimitiveExpression)
            {
                return ((CodePrimitiveExpression)targetObject).Value.GetType();
            }
            else if (targetObject is CodePropertyReferenceExpression)
            {
                Type toExamine = FindTargetType(((CodePropertyReferenceExpression)targetObject).TargetObject, thisType);
                PropertyInfo propInfo = toExamine.GetProperty(((CodePropertyReferenceExpression)targetObject).PropertyName);
                return propInfo.PropertyType;
            }
            else if (targetObject is CodeMethodInvokeExpression)
            {
                Type toExamine = FindTargetType(((CodeMethodInvokeExpression)targetObject).Method.TargetObject, thisType);
                MethodInfo methodInfo = toExamine.GetMethod(((CodeMethodInvokeExpression)targetObject).Method.MethodName);
                return methodInfo.ReturnType;
            }
            else if (targetObject is CodeFieldReferenceExpression)
            {
                Type toExamine = FindTargetType(((CodeFieldReferenceExpression)targetObject).TargetObject, thisType);
                String fieldName = ((CodeFieldReferenceExpression)targetObject).FieldName;
                FieldInfo fieldInfo = toExamine.GetField(fieldName, BindingFlags.Public | BindingFlags.NonPublic | BindingFlags.Instance | BindingFlags.Static);
                return fieldInfo.FieldType;
            }
            else if (targetObject is CodeTypeReferenceExpression)
            {
                CodeTypeReference typeReference = ((CodeTypeReferenceExpression)targetObject).Type;
                return thisType.Assembly.GetType(typeReference.BaseType);
            }
            throw new NotSupportedException("Specified CodeExpression type is not currently supported: " + targetObject.GetType().FullName);
        }

        /// <summary>
        /// Generate the evaluator class for the given CodeExpression.
        /// </summary>
        /// <param name="root">Expression for which to generate an 
        /// IExpressionEvaluator.</param>
        /// <param name="thisType">The Type meant by a
        /// CodeThisReferenceExpression.</param>
        /// <returns></returns>
        public static IExpressionEvaluator GenerateEvaluator(CodeExpression root, Type thisType)
        {
            InitializeTypes(root, thisType);

            // Create the compile unit
            CodeCompileUnit compileUnit = new CodeCompileUnit();
            CodeNamespace globalNamespace = new CodeNamespace();
            compileUnit.Namespaces.Add(globalNamespace);

            // Create the type declaration
            CodeTypeDeclaration typeDeclaration = new CodeTypeDeclaration("Evaluator");
            typeDeclaration.BaseTypes.Add(typeof(ExpressionEvaluatorBase));
            typeDeclaration.TypeAttributes = TypeAttributes.Public;
            globalNamespace.Types.Add(typeDeclaration);

            // Generate the sub expression evaluation methods.
            int methodSuffix = 0;
            string rootEvaluationMethodName = AddExpressionEvaluation(typeDeclaration, root, ref methodSuffix);

            // Create the constructor
            CodeConstructor constructor = new CodeConstructor();
            constructor.Attributes = MemberAttributes.Public;
            constructor.Statements.Add(
                new CodeAssignStatement(
                    new CodePropertyReferenceExpression(
                        new CodeThisReferenceExpression(),
                        "ParameterCount"),
                    new CodePrimitiveExpression(methodSuffix + 1)));
            typeDeclaration.Members.Add(constructor);

            // Create the Evaluate method
            CodeMemberMethod method = new CodeMemberMethod();
            method.Name = "Evaluate";
            method.Attributes = MemberAttributes.Public | MemberAttributes.Override;
            method.ReturnType = new CodeTypeReference(typeof(object));
            method.Parameters.Add(
                new CodeParameterDeclarationExpression(
                    typeof(EvaluatorParameters),
                    "eParams"));
            method.Statements.Add(
                new CodeMethodReturnStatement(
                    new CodeMethodInvokeExpression(
                        new CodeThisReferenceExpression(),
                        rootEvaluationMethodName,
                        new CodePropertyReferenceExpression(
                            new CodeVariableReferenceExpression("eParams"),
                            "Parameters"))));
            typeDeclaration.Members.Add(method);

            // Serialize the class for debugging purposes.
            CodeDomProvider provider = CodeDomProvider.CreateProvider("C#");

            string fileName = Path.GetTempFileName();
            using (StreamWriter fileOut = new StreamWriter(fileName))
            {
                System.Diagnostics.Trace.WriteLine("Evaluator Generated Code at: " + fileName);
                provider.GenerateCodeFromCompileUnit(compileUnit, fileOut, new CodeGeneratorOptions());
            }

            // Compile the evaluator
            CompilerParameters parameters = new CompilerParameters();
            parameters.ReferencedAssemblies.Add(typeof(EvalNull).Assembly.Location);

            //Add the assemblies referenced by the workflow
            AddReferencedAssemblies(parameters, thisType.Assembly, Path.GetDirectoryName(thisType.Assembly.Location).ToString());
            
            parameters.GenerateInMemory = true;
            CompilerResults results = provider.CompileAssemblyFromDom(parameters, compileUnit);

            if (results.Errors.HasErrors)
            {
                foreach (CompilerError err in results.Errors)
                {
                    System.Diagnostics.Trace.WriteLine(err.ToString());
                }
                throw new ApplicationException("CompilerErrors.  See Trace output.");
            }

            return results.CompiledAssembly.GetType(typeDeclaration.Name).InvokeMember(string.Empty, BindingFlags.CreateInstance, null, null, null, CultureInfo.InvariantCulture) as IExpressionEvaluator;
        }

        private static void AddReferencedAssemblies(CompilerParameters parameters, Assembly assembly, string searchPath)
        {
            parameters.ReferencedAssemblies.Add(assembly.Location);

            foreach (AssemblyName referencedAssemblyName in assembly.GetReferencedAssemblies())
            {
                Assembly referencedAssembly = null;

                //We dont have the extension to the referenced assembly
                //Check if it is a dll. Then load it directly.
                string possibleFilePath = Path.Combine(searchPath, referencedAssemblyName.Name + ".dll");
                try
                {
                    referencedAssembly = Assembly.LoadFile(possibleFilePath);
                }
                catch (Exception)
                {
                    //Check if it is an exe. Then load it directly.
                    try
                    {
                        possibleFilePath = Path.ChangeExtension(possibleFilePath, ".exe");
                        referencedAssembly = Assembly.LoadFile(possibleFilePath);
                    }
                    catch (Exception)
                    {
                        //If neither dll or exe, prompt the user.
                        referencedAssembly = Assembly.Load(referencedAssemblyName);
                    }
                }

                if (referencedAssembly.GlobalAssemblyCache == false)
                {
                    AddReferencedAssemblies(parameters, referencedAssembly, searchPath);
                }
            }
        }

        private static string AddExpressionEvaluation(CodeTypeDeclaration declaration, CodeExpression root, ref int methodSuffix)
        {
            int currentMethodSuffix = methodSuffix;
            CodeMemberMethod method = new CodeMemberMethod();
            method.Name = string.Format("Eval{0}", currentMethodSuffix);
            method.Attributes = MemberAttributes.Private | MemberAttributes.Final;
            method.ReturnType = new CodeTypeReference(typeof(object));
            method.Parameters.Add(
                new CodeParameterDeclarationExpression(
                    typeof(object[]),
                    "e"));

            if (root is CodeBinaryOperatorExpression)
            {
                CodeBinaryOperatorExpression binaryRoot = (CodeBinaryOperatorExpression)root;

                CodeExpression left = null;
                if (binaryRoot.Left is CodePrimitiveExpression)
                {
                    left = binaryRoot.Left;
                }
                else
                {
                    methodSuffix++;
                    Type expressionType = GetType(binaryRoot.Left, true);
                    if (expressionType == null)
                        return null;
                    left =
                        new CodeCastExpression(expressionType,
                            new CodeMethodInvokeExpression(
                                new CodeThisReferenceExpression(),
                                AddExpressionEvaluation(declaration, binaryRoot.Left, ref methodSuffix),
                                new CodeVariableReferenceExpression("e")));

                }

                CodeExpression right = null;
                if (binaryRoot.Right is CodePrimitiveExpression)
                {
                    right = binaryRoot.Right;
                }
                else
                {
                    methodSuffix++;
                    right =
                        new CodeCastExpression(GetType(binaryRoot.Right, true),
                            new CodeMethodInvokeExpression(
                                new CodeThisReferenceExpression(),
                                AddExpressionEvaluation(declaration, binaryRoot.Right, ref methodSuffix),
                                new CodeVariableReferenceExpression("e")));
                }

                method.Statements.Add(
                    new CodeConditionStatement(
                        new CodeMethodInvokeExpression(
                            new CodeThisReferenceExpression(),
                            "IsEmpty",
                            new CodeArrayIndexerExpression(
                                new CodeVariableReferenceExpression("e"),
                                new CodePrimitiveExpression(currentMethodSuffix))),
                        new CodeStatement[] {
                            new CodeMethodReturnStatement(
                                new CodeBinaryOperatorExpression(
                                    left,
                                    binaryRoot.Operator,
                                    right)) },
                        new CodeStatement[] {
                            new CodeMethodReturnStatement(
                                new CodeArrayIndexerExpression(
                                    new CodeVariableReferenceExpression("e"),
                                    new CodePrimitiveExpression(currentMethodSuffix))) }));
            }
            else
            {
                method.Statements.Add(
                    new CodeConditionStatement(
                        new CodeMethodInvokeExpression(
                            new CodeThisReferenceExpression(),
                            "IsEmpty",
                            new CodeArrayIndexerExpression(
                                new CodeVariableReferenceExpression("e"),
                                new CodePrimitiveExpression(currentMethodSuffix))),
                        new CodeStatement[] {
                            new CodeThrowExceptionStatement(
                                new CodeObjectCreateExpression(
                                    typeof(ArgumentException),
                                    new CodePrimitiveExpression("Cannot have an empty parameter for a non-binary expression"),
                                    new CodePrimitiveExpression("e[" + currentMethodSuffix + "]"))) },
                        new CodeStatement[] {
                            new CodeMethodReturnStatement(
                                new CodeArrayIndexerExpression(
                                    new CodeVariableReferenceExpression("e"),
                                    new CodePrimitiveExpression(currentMethodSuffix))) }));
            }
            declaration.Members.Add(method);

            SetParameterIndex(root, currentMethodSuffix);
            return method.Name;
        }

        /// <summary>
        /// Get the CodeExpression object's corresponding index
        /// into the IExpressionEvaluator's parameter list using
        /// EvaluationData.
        /// </summary>
        /// <param name="toGet">Expression for which to get the 
        /// parameter.</param>
        /// <param name="throwIfNotExist">Whether to throw an
        /// exception if the parameter cannot be found.  True to
        /// throw.</param>
        /// <returns>The index into the parameter list for this
        /// expression's replacement value.</returns>
        public static int GetParameterIndex(CodeObject toGet, bool throwIfNotExist)
        {
            EvaluationData data = toGet.UserData[typeof(EvaluationData)] as EvaluationData;

            if (data == null)
            {
                if (throwIfNotExist)
                {
                    throw new ApplicationException("No EvaluationData from which to extract the parameter.");
                }

                return -1;
            }
            else
            {
                return data.ParameterIndex;
            }
        }

        /// <summary>
        /// Set the parameter index on the CodeExpression using
        /// EvaluationData.
        /// </summary>
        /// <param name="toSet">CodeExpression on which to set the
        /// parameter index.</param>
        /// <param name="parameterIndex">Index to set.</param>
        public static void SetParameterIndex(CodeObject toSet, int parameterIndex)
        {
            EvaluationData data = toSet.UserData[typeof(EvaluationData)] as EvaluationData;

            if (data == null)
            {
                data = new EvaluationData();
                toSet.UserData.Add(typeof(EvaluationData), data);
            }

            data.ParameterIndex = parameterIndex;
        }

        private static string Serialize(CodeExpression expression)
        {
            CodeDomProvider provider = CodeDomProvider.CreateProvider("C#");
            StringWriter stringWriter = new StringWriter();
            provider.GenerateCodeFromExpression(expression, stringWriter, new CodeGeneratorOptions());
            return stringWriter.ToString();
        }
    }
}
