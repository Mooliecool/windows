========================================================================
    CONSOLE APPLICATION : CSEmitAssembly Project Overview
========================================================================

/////////////////////////////////////////////////////////////////////////////
Use:

Reflection provides objects (of type Type) that encapsulate assemblies, 
modules and types. It allows us to

1. Access attributes in your program's metadata.
2. Examine and instantiate types in an assembly.
3. Dynamically load and use types.
4. Emit new types at runtime.

This example shows the use of 4. CSReflection demonstrates 2 and 3. 

The System.Reflection.Emit namespace allows emitting metadata and Microsoft
intermediate language (MSIL) at run time and optionally generate a portable
executable (PE) file on disk. 

CSEmitAssembly emits these two types and save them to an assembly on disk:

public class ClassA {
    // Fields
    private ClassB classBField;
    private String stringField;

    // Methods
    public void ClassAMethod()
    { this.classBField.ClassBMethod(null); }

    // Properties
    public ClassB ClassBProperty {
        get { return this.classBField; }
        set { this.classBField = value; }
    }
}

public class ClassB {
    // Fields
    private List<ClassA> classAList;
    private ClassA classAField;

    // Methods
    public void ClassBMethod(List<ClassA> list) {
        this.classAField.ClassAMethod();
    }

    // Properties
    public List<ClassA> ClassAList {
        get { return this.classAList; }
        set { this.classAList.AddRange(value); }
    }
}


/////////////////////////////////////////////////////////////////////////////
Implementation:

The ReflectionEmitLanguage Add-in of .NET Reflector 
(http://reflectoraddins.codeplex.com/)
translates the IL code of a given method into the C# code that would be 
needed to generate the same IL code using System.Reflection.Emit. We can 
first build a .NET assembly with the target types, then use the tool to 
generate the System.Reflection.Emit codes that emits the same types.

1. Define the assembly and the module.
(AppDomain.DefineDynamicAssembly, AssemblyBuilder.DefineDynamicModule)

2. Declare the types. (ModuleBuilder.DefineType)

3. Define the types' constructors, fields, properties, and methods.
(TypeBuilder.DefineConstructor, TypeBuilder.DefineField, 
TypeBuilder.DefineProperty, TypeBuilder.DefineMethod)

4. Create the types. (TypeBuilder.CreateType)

5. Save the assembly. (AssemblyBuilder.Save)


/////////////////////////////////////////////////////////////////////////////
References:

Emitting Dynamic Methods and Assemblies
http://msdn.microsoft.com/en-us/library/8ffc3x75.aspx

System.Reflection.Emit Namespace
http://msdn.microsoft.com/en-us/library/system.reflection.emit.aspx

Dynamic Assemblies using Reflection.Emit. Part II of II - Reflection.Emit
By Piyush S Bhatnagar
http://www.codeproject.com/KB/cs/DynamicCodeGeneration2.aspx

ReflectionEmitLanguage 
http://www.codeplex.com/reflectoraddins/Wiki/View.aspx?title=ReflectionEmitLanguage&referringTitle=Home


/////////////////////////////////////////////////////////////////////////////
