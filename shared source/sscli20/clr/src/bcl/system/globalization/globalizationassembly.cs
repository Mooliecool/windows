// ==++==
// 
//   
//    Copyright (c) 2006 Microsoft Corporation.  All rights reserved.
//   
//    The use and distribution terms for this software are contained in the file
//    named license.txt, which can be found in the root of this distribution.
//    By using this software in any fashion, you are agreeing to be bound by the
//    terms of this license.
//   
//    You must not remove this notice, or any other, from this software.
//   
// 
// ==--==
namespace System.Globalization {
    using System;
    using System.Reflection;
    using System.Collections;
    using System.Threading;
    using System.Runtime.CompilerServices;
    using System.Runtime.ConstrainedExecution;
    using System.IO;

    
    /*=================================GlobalizationAssembly==========================
    **
    ** This class manages the assemblies used in the NLS+ classes.
    ** Assembly contains the data tables used by NLS+ classes.  An aseembly will carry a version
    ** of all the NLS+ data tables.  The default assembly is provided by 
    ** Assembly.GetAssembly(typeof(GlobalizationAssembly)).
    **
    ** We use assembly to support versioning of NLS+ data tables.  Take CompareInfo for example. 
    ** In CompareInfo.GetCompareInfo(int culture, Assembly),
    ** you can pass an assembly which contains the different NLS+ data tables.  By doing this, the constructed CompareInfo
    ** will read the sorting tables from the specified Assembly, instead of from the default assembly shipped with the runtime.
    **
    ** For every assembly used, we will create one corresponding GlobalizationAssembly.  
    ** Within the GlobalizationAssembly, we will hold the following information:
    **      1. the 32-bit pointer value pointing to the corresponding native C++ NativeGlobalizationAssembly object for it.  This pointer
    **        is needed when we create SortingTable.  See CompareInfo ctor for an example.
    **      2. the caches for different type of objects. For instances, we will have caches for CompareInfo, CultureInfo, RegionInfo, etc.
    **         The idea is to create one instance of CompareInfo for a specific culture.
    ** 
    ** For only, only CompareInfo versioning is supported.  However, this class can be expanded to support the versioning of 
    ** CultureInfo, RegionInfo, etc.
    **
    ============================================================================*/

    internal sealed class GlobalizationAssembly {
        // ----------------------------------------------------------------------------------------------------
        //
        // Static data members and static methods.
        //
        // ----------------------------------------------------------------------------------------------------
    
        //
        // Hash to store Globalization assembly.
        //
        private static Hashtable m_assemblyHash = new Hashtable(4);

        //
        // The pointer to the default C++ native NativeGlobalizationAssembly object for the class library.
        // We use default native NativeGlobalizationAssembly to access NLS+ data table shipped with the runtime.
        //
        // Classes like CompareInfo will access this data member directly when the default assembly is used.
        //
        internal static GlobalizationAssembly m_defaultInstance = GetGlobalizationAssembly(Assembly.GetAssembly(typeof(GlobalizationAssembly)));

        internal static GlobalizationAssembly DefaultInstance
        {
            //
            // In case the GlobalizationAssembly class failed during the type initilaization then we'll have m_defaultInstance = null.
            // to be reliable we throw exception instead of getting access violation.
            //
            get
            {
                if (m_defaultInstance == null)
                    throw new TypeLoadException(CultureTable.TypeLoadExceptionMessage);

                return m_defaultInstance;
            }
        }

        /*=================================GetGlobalizationAssembly==========================
        **Action: Return the GlobalizationAssembly instance for the specified assembly.
        **  Every assembly should have one and only one instance of GlobalizationAssembly.
        **Returns: An instance of GlobalizationAssembly.
        **Arguments:
        **Exceptions:
        ============================================================================*/
        internal static GlobalizationAssembly GetGlobalizationAssembly(Assembly assembly) {
            GlobalizationAssembly ga;

            if ((ga = (GlobalizationAssembly)m_assemblyHash[assembly]) == null) 
            {
                // This is intentionally taking a process-global lock on an
                // internal Type, using ExecuteCodeWithLock to guarantee we 
                // release the lock in a stack overflow safe way.
                RuntimeHelpers.TryCode createGlobalizationAssem = new RuntimeHelpers.TryCode(CreateGlobalizationAssembly);
                RuntimeHelpers.ExecuteCodeWithLock(typeof(CultureTableRecord),
                                                    createGlobalizationAssem,
                                                    assembly);
                ga = (GlobalizationAssembly)m_assemblyHash[assembly];
            }            
            BCLDebug.Assert(ga != null, "GlobalizationAssembly was not created");
            return (ga);
        }

        [PrePrepareMethod]
        private unsafe static void CreateGlobalizationAssembly(object assem)
        {
            GlobalizationAssembly ga;
            Assembly assembly = (Assembly) assem;
            if ((ga = (GlobalizationAssembly)m_assemblyHash[assembly]) == null) 
            {
                // This assembly is not used before, create a corresponding native NativeGlobalizationAssembly object for it.
                ga = new GlobalizationAssembly();
                ga.pNativeGlobalizationAssembly = nativeCreateGlobalizationAssembly(assembly);
                System.Threading.Thread.MemoryBarrier();
                m_assemblyHash[assembly] = ga;
            }
        }
        
        // This method requires synchonization because class global data member is used
        // in the native side.      
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        private unsafe static extern void* nativeCreateGlobalizationAssembly(Assembly assembly);        
        

        // ----------------------------------------------------------------------------------------------------
        //
        // Instance data members and instance methods.
        //
        // ----------------------------------------------------------------------------------------------------

        // This is the cache to store CompareInfo for a particular culture.
        // The key is culture, and the content is an instance of CompareInfo.
        internal Hashtable compareInfoCache;
        // We will have more caches here for CultureInfo, RegionInfo, etc. if we want to versioning in 
        // these classes.

        // The pointer to C++ native NativeGlobalizationAssembly
        unsafe internal void* pNativeGlobalizationAssembly;
        
        internal GlobalizationAssembly() {
            // Create cache for CompareInfo instances.
            compareInfoCache = new Hashtable(4);
        }        

        internal unsafe static byte* GetGlobalizationResourceBytePtr(Assembly assembly, String tableName) {
            BCLDebug.Assert(assembly != null, "assembly can not be null.  This should be generally the mscorlib.dll assembly.");
            BCLDebug.Assert(tableName != null, "table name can not be null");
            
            Stream stream = assembly.GetManifestResourceStream(tableName);
            UnmanagedMemoryStream bytesStream = stream as UnmanagedMemoryStream;
            if (bytesStream != null) {
                byte* bytes = bytesStream.PositionPointer;
                if (bytes != null) {
                    return (bytes);
                }
            }
            
            BCLDebug.Assert(
                    false, 
                    String.Format(
                        CultureInfo.CurrentCulture,
                        "Didn't get the resource table {0} for System.Globalization from {1}", 
                        tableName, 
                        assembly));
            // We can not continue if we can't get the resource.
            throw new ExecutionEngineException();
        }

    }
}
