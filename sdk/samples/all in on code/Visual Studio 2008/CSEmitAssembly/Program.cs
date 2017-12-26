/****************************** Module Header ******************************\
* Module Name:  Program.cs
* Project:      CSEmitAssembly
* Copyright (c) Microsoft Corporation.
* 
* This example emits these two classes that are mutually referenced.
* 
* public class ClassA {
*     // Fields
*     private ClassB classBField;
*     private String stringField;
* 
*     // Methods
*     public void ClassAMethod()
*     { this.classBField.ClassBMethod(null); }
* 
*     // Properties
*     public ClassB ClassBProperty {
*         get { return this.classBField; }
*         set { this.classBField = value; }
*     }
* }
* 
* public class ClassB {
*     // Fields
*     private List<ClassA> classAList;
*     private ClassA classAField;
* 
*     // Methods
*     public void ClassBMethod(List<ClassA> list) {
*         this.classAField.ClassAMethod();
*     }
* 
*     // Properties
*     public List<ClassA> ClassAList {
*         get { return this.classAList; }
*         set { this.classAList.AddRange(value); }
*     }
* }
* 
* This source is subject to the Microsoft Public License.
* See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
* All other rights reserved.
* 
* THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND, 
* EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED 
* WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
\***************************************************************************/

#region Using directives
using System;
using System.Collections.Generic;
using System.Linq;
using System.Reflection;
using System.Reflection.Emit;
using System.Text;
#endregion


class Program
{
    static void Main(string[] args)
    {
        /////////////////////////////////////////////////////////////////////
        // Define the assembly and the module.
        // 

        AppDomain appDomain = AppDomain.CurrentDomain;
        AssemblyName assemblyName = new AssemblyName("EmittedAssembly");
        AssemblyBuilder assembly = appDomain.DefineDynamicAssembly(
            assemblyName, AssemblyBuilderAccess.RunAndSave);

        // An assembly is made up of executable modules. For a single-module
        // assembly, the module name and file name are the same as the 
        // assembly name. 

        ModuleBuilder module = assembly.DefineDynamicModule(
            assemblyName.Name, assemblyName.Name + ".dll");


        /////////////////////////////////////////////////////////////////////
        // Declare the types (classes).
        // 

        // Declare the class "ClassA"
        TypeBuilder classA = module.DefineType("ClassA", TypeAttributes.Public);
        // Declare the class "ClassB"
        TypeBuilder classB = module.DefineType("ClassB", TypeAttributes.Public);


        /////////////////////////////////////////////////////////////////////
        // Define the type, ClassA.
        // 

        #region Constructors

        #endregion

        #region Fields

        // Define the fields stringField, classBField
        FieldBuilder stringField = classA.DefineField("stringField",
            typeof(string), FieldAttributes.Private);
        FieldBuilder classBField = classA.DefineField("classBField",
            classB, FieldAttributes.Public);

        #endregion

        #region Properties

        // Define the property ClassBProperty
        PropertyBuilder classBProperty = classA.DefineProperty(
            "ClassBProperty", PropertyAttributes.None, classB, null);

        // The special set of attributes for the property set&get methods
        MethodAttributes getSetAttr = MethodAttributes.Public |
            MethodAttributes.SpecialName | MethodAttributes.HideBySig;

        // Define the "get" accessor method for ClassBProperty
        MethodBuilder classBGetProp = classA.DefineMethod(
            "get_ClassBProperty", getSetAttr, classB, Type.EmptyTypes);
        ILGenerator classBGetIL = classBGetProp.GetILGenerator();
        classBGetIL.Emit(OpCodes.Ldarg_0);
        classBGetIL.Emit(OpCodes.Ldfld, classBField);
        classBGetIL.Emit(OpCodes.Ret);

        // Define the "set" accessor method for ClassBProperty
        MethodBuilder classBSetProp = classA.DefineMethod(
            "set_ClassBProperty", getSetAttr, null, new Type[] { classB });
        ILGenerator sampleSetIL = classBSetProp.GetILGenerator();
        sampleSetIL.Emit(OpCodes.Ldarg_0);
        sampleSetIL.Emit(OpCodes.Ldarg_1);
        sampleSetIL.Emit(OpCodes.Stfld, classBField);
        sampleSetIL.Emit(OpCodes.Ret);

        // Map the get&set methods to PropertyBuilder
        classBProperty.SetGetMethod(classBGetProp);
        classBProperty.SetSetMethod(classBSetProp);

        #endregion

        #region Methods

        // Define a method that uses the classBField
        MethodBuilder classAMethod = classA.DefineMethod("ClassAMethod", 
            MethodAttributes.Public);

        // Define the list generics and ienumerable generic
        Type listOf = typeof(List<>);
        Type enumOf = typeof(IEnumerable<>);
        Type listOfClassA = listOf.MakeGenericType(classA);
        Type enumOfClassA = enumOf.MakeGenericType(classA);

        // Define the method, ClassBMethod, for ClassB
        MethodBuilder classBMethod = classB.DefineMethod("ClassBMethod", 
            MethodAttributes.Public, typeof(void), new Type[] { listOfClassA });
        classBMethod.DefineParameter(1, ParameterAttributes.None, "list");

        // Write the body of ClassAMethod that calls ClassBMethod
        ILGenerator ilgenA = classAMethod.GetILGenerator();
        ilgenA.Emit(OpCodes.Nop);
        ilgenA.Emit(OpCodes.Ldarg_0);
        ilgenA.Emit(OpCodes.Ldfld, classBField);
        ilgenA.Emit(OpCodes.Ldnull);
        ilgenA.Emit(OpCodes.Callvirt, classBMethod);
        ilgenA.Emit(OpCodes.Ret);

        #endregion


        /////////////////////////////////////////////////////////////////////
        // Define the type, ClassB.
        // 

        #region Constructors

        #endregion

        #region Fields

        // Define the fields classAField, classAList
        FieldBuilder classAField = classB.DefineField("classAField", classA, 
            FieldAttributes.Private);
        FieldBuilder classAList = classB.DefineField("classAList", listOfClassA, 
            FieldAttributes.Private);

        #endregion

        #region Properties

        // Define the property ClassAList
        PropertyBuilder classAListProperty = classB.DefineProperty(
            "ClassAList", PropertyAttributes.None, listOfClassA, null);

        // Define the "get" accessor method for ClassAList
        MethodBuilder listGetProp = classB.DefineMethod("get_ClassAList",
            getSetAttr, listOfClassA, null);
        ILGenerator listGetIL = listGetProp.GetILGenerator();
        listGetIL.Emit(OpCodes.Ldarg_0);
        listGetIL.Emit(OpCodes.Ldfld, classAList);
        listGetIL.Emit(OpCodes.Ret);

        // Define the "set" accessor method for ClassAList
        MethodInfo addRangeBaseMethod = listOf.GetMethod("AddRange");
        MethodInfo addRangeMethod = TypeBuilder.GetMethod(
            listOfClassA, addRangeBaseMethod);

        MethodBuilder listSetProp = classB.DefineMethod("set_ClassAList",
            getSetAttr, null, new Type[] { listOfClassA });
        ILGenerator listSetIL = listSetProp.GetILGenerator();
        listSetIL.Emit(OpCodes.Nop);
        listSetIL.Emit(OpCodes.Ldarg_0);
        listSetIL.Emit(OpCodes.Ldfld, classAList);
        listSetIL.Emit(OpCodes.Ldarg_1);
        listSetIL.Emit(OpCodes.Callvirt, addRangeMethod);
        listSetIL.Emit(OpCodes.Ret);

        classAListProperty.SetGetMethod(listGetProp);
        classAListProperty.SetSetMethod(listSetProp);

        #endregion

        #region Methods

        // Write the body of ClassBMethod that calls ClassAMethod
        ILGenerator ilgenB = classBMethod.GetILGenerator();
        ilgenB.Emit(OpCodes.Nop);
        ilgenB.Emit(OpCodes.Ldarg_0);
        ilgenB.Emit(OpCodes.Ldfld, classAField);
        ilgenB.Emit(OpCodes.Callvirt, classAMethod);
        ilgenB.Emit(OpCodes.Ret);

        #endregion


        /////////////////////////////////////////////////////////////////////
        // Create the types.
        // 

        classA.CreateType();
        classB.CreateType();


        /////////////////////////////////////////////////////////////////////
        // Save the assembly.
        // 

        assembly.Save(assemblyName.Name + ".dll");
    }
}
