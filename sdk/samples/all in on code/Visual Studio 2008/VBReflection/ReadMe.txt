=============================================================================
            CONSOLE APPLICATION : VBReflection Project Overview
=============================================================================

/////////////////////////////////////////////////////////////////////////////
Summary:

Reflection provides objects (of type Type) that encapsulate assemblies, 
modules and types. It allows us to

1. Access attributes in your program's metadata.
2. Examine and instantiate types in an assembly.
3. Dynamically load and use types.
4. Emit new types at runtime.

This sample demonstrates 2 and 3.


/////////////////////////////////////////////////////////////////////////////
Project Relation:

VBReflection -> VBClassLibrary
VBReflection dynamically loads the assembly, VBClassLibrary.dll, and 
instantiate, examine and use its types.


/////////////////////////////////////////////////////////////////////////////
Implementation:

1. Dynamically load the assembly. 
(Assembly.LoadFrom, Assembly.Load, Assembly.LoadFile)

2. Get a type and instantiate the type in the assembly.
(Assembly.GetType, Activator.CreateInstance, Assembly.CreateInstance)

3. Examine the type. (Type.GetFields, Type.GetProperties, Type.GetEvents, 
Type.GetMethods, Type.GetConstructors)

4. Use the type (Late Binding). (MethodInfo.Invoke)

5. There is no API to unload an assembly
http://blogs.msdn.com/suzcook/archive/2003/07/08/57211.aspx
http://blogs.msdn.com/jasonz/archive/2004/05/31/145105.aspx


/////////////////////////////////////////////////////////////////////////////
References:

Reflection in Visual Basic .NET
http://msdn.microsoft.com/en-us/magazine/cc163750.aspx

Dynamically Loading and Using Types
http://msdn.microsoft.com/en-us/library/k3a58006.aspx

How to: Hook Up a Delegate Using Reflection
http://msdn.microsoft.com/en-us/library/ms228976.aspx


/////////////////////////////////////////////////////////////////////////////
