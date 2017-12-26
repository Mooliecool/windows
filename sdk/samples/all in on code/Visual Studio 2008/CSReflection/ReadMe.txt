=============================================================================
          CONSOLE APPLICATION : CSReflection Project Overview
=============================================================================

/////////////////////////////////////////////////////////////////////////////
Summary:

Reflection provides objects (of type Type) that encapsulate assemblies, 
modules and types. It allows us to

1. Access attributes in your program's metadata.
2. Examine and instantiate types in an assembly.
3. Dynamically load and use types.
4. Emit new types at runtime.

This sample demonstrates 2 and 3. CSEmitAssembly shows the use of 4.


/////////////////////////////////////////////////////////////////////////////
Project Relation:

CSReflection -> CSClassLibrary
CSReflection dynamically loads the assembly, CSClassLibrary.dll, and 
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

Reflection (C# Programming Guide)
http://msdn.microsoft.com/en-us/library/ms173183.aspx

Dynamically Loading and Using Types
http://msdn.microsoft.com/en-us/library/k3a58006.aspx


/////////////////////////////////////////////////////////////////////////////
