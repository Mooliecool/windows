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

[assembly:System.Runtime.InteropServices.Guid("D3295D86-D604-11D4-A704-00C04FA137E4")]
[assembly:System.Security.AllowPartiallyTrustedCallers()]
namespace Microsoft.Vsa
{
    using System.CodeDom;
    using System.Runtime.InteropServices;
    using System;
    using System.Runtime.Serialization;
    using System.Security.Permissions;

    /* VsaItemType is also defined in vs\src\vsa\vbapkg\src\vbapkg.idl
       If you want to change the definition here, you have to make changes there too.
       See comment in vs\src\vsa\vbapkg\src\vbapkg.idl which explain why we need
       this definition twice */  
    [Guid("a9f6f86e-fcf5-3d8d-97e4-0fe6c7fe2274")]
    [Obsolete(VsaObsolete.Description)]
    public enum VsaItemType
    {
        Reference,             // IVsaReferenceItem - COM+ Assembly or RCW
        AppGlobal,             // IVsaItem - application object
        Code,                  // IVsaCodeItem - vsa code
    }
    
    [Guid("40d8ce77-8139-3483-8674-bb9e3acd9b89")]
    [Obsolete(VsaObsolete.Description)]
    public enum VsaItemFlag
    {
        None,                  // No special flags required or flags not accepted
        Module,                // Global Module
        Class,                 // Instantiable COM+ class.
    }

    [Guid("05a1147d-6c8d-3bb8-888f-8bfa319b2e6f")]
    [Obsolete(VsaObsolete.Description)]
    public enum VsaIDEMode
    {
        Break,                  // Break mode - debugger is attached and in break mode
        Design,                 // Design mode - debugger is not attached
        Run,                    // Run mode - debugger is attached
    }
         
    [Guid("E0C0FFE1-7eea-4ee2-b7e4-0080c7eb0b74")]
    [InterfaceType(ComInterfaceType.InterfaceIsIUnknown)]
    [Obsolete(VsaObsolete.Description)]
    public interface IVsaEngine
    {
        // === Site ===
        IVsaSite Site
        { 
            [PermissionSetAttribute(SecurityAction.LinkDemand, Name = "FullTrust")]
            get; 
            [PermissionSetAttribute(SecurityAction.LinkDemand, Name = "FullTrust")]
            set; 
        }
    
        // === Properties ===
        // Name, RootMoniker, RootNameSpace are first class properties of the engine
        // Never assume get, set operations of first class properties can be the
        // equivalent of calling SetOption and GetOption
    
        string Name 
        { 
            [PermissionSetAttribute(SecurityAction.LinkDemand, Name = "FullTrust")]
            get; 
            [PermissionSetAttribute(SecurityAction.LinkDemand, Name = "FullTrust")]
            set; 
        }
        string RootMoniker 
        { 
            [PermissionSetAttribute(SecurityAction.LinkDemand, Name = "FullTrust")]
            get; 
            [PermissionSetAttribute(SecurityAction.LinkDemand, Name = "FullTrust")]
            set; 
        }        
        // Default namespace used by the engine
        string RootNamespace
        { 
            [PermissionSetAttribute(SecurityAction.LinkDemand, Name = "FullTrust")]
            get; 
            [PermissionSetAttribute(SecurityAction.LinkDemand, Name = "FullTrust")]
            set; 
        }
    
        int LCID
        { 
            [PermissionSetAttribute(SecurityAction.LinkDemand, Name = "FullTrust")]
            get; 
            [PermissionSetAttribute(SecurityAction.LinkDemand, Name = "FullTrust")]
            set; 
        }

        bool GenerateDebugInfo
        { 
            [PermissionSetAttribute(SecurityAction.LinkDemand, Name = "FullTrust")]
            get; 
            [PermissionSetAttribute(SecurityAction.LinkDemand, Name = "FullTrust")]
            set; 
        }

        System.Security.Policy.Evidence Evidence
        { 
            [PermissionSetAttribute(SecurityAction.LinkDemand, Name = "FullTrust")]
            get; 
            [PermissionSetAttribute(SecurityAction.LinkDemand, Name = "FullTrust")]
            set; 
        }

        // collection of items
        IVsaItems Items
        { 
            [PermissionSetAttribute(SecurityAction.LinkDemand, Name = "FullTrust")]
            get; 
        }
    
        // returns TRUE if any item in the engine has been dirtied
        bool IsDirty
        { 
            [PermissionSetAttribute(SecurityAction.LinkDemand, Name = "FullTrust")]
            get; 
        }
    
        // return the programming language of the engine (e.g. VB or JScript)
        string Language
        { 
            [PermissionSetAttribute(SecurityAction.LinkDemand, Name = "FullTrust")]
            get; 
        }
    
        // returns the engine version
        string Version
        { 
            [PermissionSetAttribute(SecurityAction.LinkDemand, Name = "FullTrust")]
            get; 
        }

        // engine options (methods)
        [PermissionSetAttribute(SecurityAction.LinkDemand, Name = "FullTrust")]
        object GetOption(string name);
        [PermissionSetAttribute(SecurityAction.LinkDemand, Name = "FullTrust")]
        void SetOption(string name, object value);
    
        // === State management ===
        [PermissionSetAttribute(SecurityAction.LinkDemand, Name = "FullTrust")]
        bool Compile();
        [PermissionSetAttribute(SecurityAction.LinkDemand, Name = "FullTrust")]
        void Run();
        [PermissionSetAttribute(SecurityAction.LinkDemand, Name = "FullTrust")]
        void Reset();
        [PermissionSetAttribute(SecurityAction.LinkDemand, Name = "FullTrust")]
        void Close();

        bool IsRunning
        { 
            [PermissionSetAttribute(SecurityAction.LinkDemand, Name = "FullTrust")]
            get; 
        }
        bool IsCompiled
        { 
            [PermissionSetAttribute(SecurityAction.LinkDemand, Name = "FullTrust")]
            get; 
        }

        [PermissionSetAttribute(SecurityAction.LinkDemand, Name = "FullTrust")]
        void RevokeCache();
    
        // === Source State ===
        [PermissionSetAttribute(SecurityAction.LinkDemand, Name = "FullTrust")]
        void SaveSourceState(IVsaPersistSite site);
        [PermissionSetAttribute(SecurityAction.LinkDemand, Name = "FullTrust")]
        void LoadSourceState(IVsaPersistSite site);

        // === Compiled State ===
        [PermissionSetAttribute(SecurityAction.LinkDemand, Name = "FullTrust")]
        void SaveCompiledState(out byte[] pe, out byte[] pdb);
    
        // Called if the engine state will be set by calls (rather than a call to by LoadSourceState)
        [PermissionSetAttribute(SecurityAction.LinkDemand, Name = "FullTrust")]
        void InitNew();

        // returns TRUE if the identifier if legal for the language of the engine
        [PermissionSetAttribute(SecurityAction.LinkDemand, Name = "FullTrust")]
        bool IsValidIdentifier(string identifier);

        // === Assembly === 
        System.Reflection.Assembly Assembly { get; }
    } // IVsaEngine

    [Guid("E0C0FFE2-7eea-4ee2-b7e4-0080c7eb0b74")]      
    [InterfaceType(ComInterfaceType.InterfaceIsIUnknown)]
    [Obsolete(VsaObsolete.Description)]
    public interface IVsaSite
    {
        [PermissionSetAttribute(SecurityAction.LinkDemand, Name = "FullTrust")]
        void GetCompiledState(out byte[] pe, out byte[] debugInfo);
    
        [PermissionSetAttribute(SecurityAction.LinkDemand, Name = "FullTrust")]
        bool OnCompilerError(IVsaError error);       

        object GetGlobalInstance( string name );

        object GetEventSourceInstance( string itemName,
                                       string eventSourceName);
     
	[PermissionSetAttribute(SecurityAction.LinkDemand, Name = "FullTrust")]
        void Notify(string notify, object info);

    } // IVSaSite
  
    [Guid("E0C0FFE3-7eea-4ee2-b7e4-0080c7eb0b74")]      
    [InterfaceType(ComInterfaceType.InterfaceIsIUnknown)]
    [Obsolete(VsaObsolete.Description)]
    public interface IVsaPersistSite
    {
      [PermissionSetAttribute(SecurityAction.LinkDemand, Name = "FullTrust")]
      void SaveElement(string name, string source);
      
      [PermissionSetAttribute(SecurityAction.LinkDemand, Name = "FullTrust")]
      string LoadElement(string name);
    } // IVsaPersistSite
      
    [Guid("E0C0FFE4-7eea-4ee2-b7e4-0080c7eb0b74")]      
    [InterfaceType(ComInterfaceType.InterfaceIsIUnknown)]
    [Obsolete(VsaObsolete.Description)]
    public interface IVsaError
    {
        int Line
        { 
            [PermissionSetAttribute(SecurityAction.LinkDemand, Name = "FullTrust")]
            get; 
        }
        int Severity
        { 
            [PermissionSetAttribute(SecurityAction.LinkDemand, Name = "FullTrust")]
            get; 
        }
        string Description
        { 
            [PermissionSetAttribute(SecurityAction.LinkDemand, Name = "FullTrust")]
            get; 
        }
        string LineText
        { 
            [PermissionSetAttribute(SecurityAction.LinkDemand, Name = "FullTrust")]
            get; 
        }
        IVsaItem SourceItem
        { 
            [PermissionSetAttribute(SecurityAction.LinkDemand, Name = "FullTrust")]
            get; 
        }
        // returns the ending column of the text creating the error
        int EndColumn
        { 
            [PermissionSetAttribute(SecurityAction.LinkDemand, Name = "FullTrust")]
            get; 
        }
        // returns the beginning column of the text creating the error
        int StartColumn
        { 
            [PermissionSetAttribute(SecurityAction.LinkDemand, Name = "FullTrust")]
            get; 
        }
        // Error number
        int Number
        { 
            [PermissionSetAttribute(SecurityAction.LinkDemand, Name = "FullTrust")]
            get; 
        }
        // (fully qualified?) moniker of item with error
        string SourceMoniker
        { 
            [PermissionSetAttribute(SecurityAction.LinkDemand, Name = "FullTrust")]
            get; 
        }
    } // IVsaError

    [Guid("E0C0FFE5-7eea-4ee5-b7e4-0080c7eb0b74")]      
    [InterfaceType(ComInterfaceType.InterfaceIsIUnknown)]
    [Obsolete(VsaObsolete.Description)]
    public interface IVsaItem
    {   
        string Name
        { 
            [PermissionSetAttribute(SecurityAction.LinkDemand, Name = "FullTrust")]
            get; 
            [PermissionSetAttribute(SecurityAction.LinkDemand, Name = "FullTrust")]
            set; 
        }
        VsaItemType ItemType
        { 
            [PermissionSetAttribute(SecurityAction.LinkDemand, Name = "FullTrust")]
            get; 
        }
        // returns TRUE if item is dirty
        bool IsDirty
        { 
            [PermissionSetAttribute(SecurityAction.LinkDemand, Name = "FullTrust")]
            get; 
        }
        // item option (property)
        [PermissionSetAttribute(SecurityAction.LinkDemand, Name = "FullTrust")]
        object GetOption(string name);
        [PermissionSetAttribute(SecurityAction.LinkDemand, Name = "FullTrust")]
        void SetOption(string name, object value);
    } // IVsaItem
    
    [Guid("0AB1EB6A-12BD-44d0-B941-0580ADFC73DE")]      
    [InterfaceType(ComInterfaceType.InterfaceIsIUnknown)]
    [Obsolete(VsaObsolete.Description)]
    public interface IVsaItems : System.Collections.IEnumerable
    {
        int Count
        { 
            [PermissionSetAttribute(SecurityAction.LinkDemand, Name = "FullTrust")]
            get; 
        }
        IVsaItem this[string name]
        { 
            [PermissionSetAttribute(SecurityAction.LinkDemand, Name = "FullTrust")]
            get; 
        }
        IVsaItem this[int index]
        { 
            [PermissionSetAttribute(SecurityAction.LinkDemand, Name = "FullTrust")]
            get; 
        }
        // Create an item
        [PermissionSetAttribute(SecurityAction.LinkDemand, Name = "FullTrust")]
        IVsaItem CreateItem(string name, VsaItemType itemType, VsaItemFlag itemFlag);
        [PermissionSetAttribute(SecurityAction.LinkDemand, Name = "FullTrust")]
        void Remove(string name);
        [PermissionSetAttribute(SecurityAction.LinkDemand, Name = "FullTrust")]
        void Remove(int index);
    } // IVsaItems
          
    [Guid("E0C0FFE6-7eea-4ee5-b7e4-0080c7eb0b74")]      
    [InterfaceType(ComInterfaceType.InterfaceIsIUnknown)]
    [Obsolete(VsaObsolete.Description)]
    public interface IVsaReferenceItem : IVsaItem
    { 
        string AssemblyName
        { 
            [PermissionSetAttribute(SecurityAction.LinkDemand, Name = "FullTrust")]
            get; 
            [PermissionSetAttribute(SecurityAction.LinkDemand, Name = "FullTrust")]
            set; 
        }
    } // IVsaReferenceItem
    
    [Guid("E0C0FFE7-7eea-4ee5-b7e4-0080c7eb0b74")]      
    [InterfaceType(ComInterfaceType.InterfaceIsIUnknown)]
    [Obsolete(VsaObsolete.Description)]
    public interface IVsaCodeItem : IVsaItem
    {
        string SourceText
        { 
            [PermissionSetAttribute(SecurityAction.LinkDemand, Name = "FullTrust")]
            get; 
            [PermissionSetAttribute(SecurityAction.LinkDemand, Name = "FullTrust")]
            set; 
        }
    
        // returns a CodeDom tree for this code item.  The CodeObject returned is of type
        // System.CodeDom.CodeCompileUnit
        System.CodeDom.CodeObject CodeDOM
        { 
            [PermissionSetAttribute(SecurityAction.LinkDemand, Name = "FullTrust")]
            get; 
        }

        [PermissionSetAttribute(SecurityAction.LinkDemand, Name = "FullTrust")]    
        void AppendSourceText(string text);

        [PermissionSetAttribute(SecurityAction.LinkDemand, Name = "FullTrust")]    
        void AddEventSource( string eventSourceName,
                             string eventSourceType);
  
        [PermissionSetAttribute(SecurityAction.LinkDemand, Name = "FullTrust")]      
        void RemoveEventSource( string eventSourceName);
        
    } // IVsaCodeItem
      
    
    [Guid("4E76D92E-E29D-46f3-AE22-0333158109F1")]
    [InterfaceType(ComInterfaceType.InterfaceIsIUnknown)]
    [Obsolete(VsaObsolete.Description)]
    public interface IVsaGlobalItem : IVsaItem
    {
        string TypeString
        { 
            [PermissionSetAttribute(SecurityAction.LinkDemand, Name = "FullTrust")]
            set; 
        }
    
        bool ExposeMembers
        { 
            [PermissionSetAttribute(SecurityAction.LinkDemand, Name = "FullTrust")]
            get; 
            [PermissionSetAttribute(SecurityAction.LinkDemand, Name = "FullTrust")]
            set; 
        }
    } // IVsaGlobalItem
    
    [Guid("7BD84086-1FB5-4b5d-8E05-EAA2F17218E0")]
    [InterfaceType(ComInterfaceType.InterfaceIsIUnknown)]
    [Obsolete(VsaObsolete.Description)]
    public interface IVsaIDESite
    {
        [PermissionSetAttribute(SecurityAction.LinkDemand, Name = "FullTrust")]
        void Notify(string notify, object optional);
    } // IVsaIDESite
    
    [Guid("78470A10-8153-407d-AB1B-05067C54C36B")]
    [InterfaceType(ComInterfaceType.InterfaceIsIUnknown)]
    [Obsolete(VsaObsolete.Description)]
    public interface IVsaIDE
    {
        // === Site ===
        IVsaIDESite Site
        { 
            [PermissionSetAttribute(SecurityAction.LinkDemand, Name = "FullTrust")]
            get; 
            [PermissionSetAttribute(SecurityAction.LinkDemand, Name = "FullTrust")]
            set; 
        }
        [PermissionSetAttribute(SecurityAction.LinkDemand, Name = "FullTrust")]
        void ShowIDE(bool showOrHide);
        
        // determines default starting search path of the Find dialog
        string DefaultSearchPath
        { 
            [PermissionSetAttribute(SecurityAction.LinkDemand, Name = "FullTrust")]
            get; 
            [PermissionSetAttribute(SecurityAction.LinkDemand, Name = "FullTrust")]
            set; 
        }
        
        object ExtensibilityObject 
        {
            [PermissionSetAttribute(SecurityAction.LinkDemand, Name = "FullTrust")]
            get; 
        }
        [PermissionSetAttribute(SecurityAction.LinkDemand, Name = "FullTrust")]
        void EnableMainWindow(bool isEnable);
        VsaIDEMode IDEMode
        { 
            [PermissionSetAttribute(SecurityAction.LinkDemand, Name = "FullTrust")]
            get; 
        }
    } // IVsaIDE

    [Guid("E0C0FFEE-7eea-4ee5-b7e4-0080c7eb0b74")]      
    [InterfaceType(ComInterfaceType.InterfaceIsIUnknown)]
    [Obsolete(VsaObsolete.Description)]
    public interface IVsaDTEngine
    {
        // remote debugging properties
        string TargetURL
        { 
            [PermissionSetAttribute(SecurityAction.LinkDemand, Name = "FullTrust")]
            get; 
            [PermissionSetAttribute(SecurityAction.LinkDemand, Name = "FullTrust")]
            set; 
        }
        // methods
        [PermissionSetAttribute(SecurityAction.LinkDemand, Name = "FullTrust")]       
        void InitCompleted();
        IVsaIDE GetIDE();
        [PermissionSetAttribute(SecurityAction.LinkDemand, Name = "FullTrust")]
        void AttachDebugger(bool isAttach);
    } // IVsaDTEngine


    [Guid("E0C0FFED-7eea-4ee5-b7e4-0080c7eb0b74")]      
    [InterfaceType(ComInterfaceType.InterfaceIsIUnknown)]
    [Obsolete(VsaObsolete.Description)]
    public interface IVsaDTCodeItem
    {        
        // IDE control attributes
        bool CanDelete
        { 
            [PermissionSetAttribute(SecurityAction.LinkDemand, Name = "FullTrust")]
            get; 
            [PermissionSetAttribute(SecurityAction.LinkDemand, Name = "FullTrust")]
            set; 
        }
        bool CanMove
        { 
            [PermissionSetAttribute(SecurityAction.LinkDemand, Name = "FullTrust")]
            get; 
            [PermissionSetAttribute(SecurityAction.LinkDemand, Name = "FullTrust")]
            set; 
        }
        bool CanRename
        { 
            [PermissionSetAttribute(SecurityAction.LinkDemand, Name = "FullTrust")]
            get; 
            [PermissionSetAttribute(SecurityAction.LinkDemand, Name = "FullTrust")]
            set; 
        }
        bool Hidden
        { 
            [PermissionSetAttribute(SecurityAction.LinkDemand, Name = "FullTrust")]
            get; 
            [PermissionSetAttribute(SecurityAction.LinkDemand, Name = "FullTrust")]
            set; 
        }

        // TRUE means buffer cannot be edited
        bool ReadOnly
        { 
            [PermissionSetAttribute(SecurityAction.LinkDemand, Name = "FullTrust")]
            get; 
            [PermissionSetAttribute(SecurityAction.LinkDemand, Name = "FullTrust")]
            set; 
        }
    } // IVsaDTCodeItem

    // Make it contiguous so that it's easy to determine whether this is a VsaError.
    // WARNING: If you change the error code below, you MUST make the corresponding changes in vs\src\common\inc\vsa\vsares.h
    // Note we are using the int (as opposed to unsigned hex) values for CLS Compliance
    [Guid("c216f805-8fab-3d33-bf81-7b1035e917b9")]
    [Obsolete(VsaObsolete.Description)]
    public enum VsaError : int
    {
        AppDomainCannotBeSet        = -2146226176, // 0x80133000
        AppDomainInvalid            = -2146226175, // 0x80133001
        ApplicationBaseCannotBeSet  = -2146226174, // 0x80133002
        ApplicationBaseInvalid      = -2146226173, // 0x80133003
        AssemblyExpected            = -2146226172, // 0x80133004
        AssemblyNameInvalid         = -2146226171, // 0x80133005
        BadAssembly                 = -2146226170, // 0x80133006
        CachedAssemblyInvalid       = -2146226169, // 0x80133007
        CallbackUnexpected          = -2146226168, // 0x80133008
        CodeDOMNotAvailable         = -2146226167, // 0x80133009
        CompiledStateNotFound       = -2146226166, // 0x8013300A
        DebugInfoNotSupported       = -2146226165, // 0x8013300B
        ElementNameInvalid          = -2146226164, // 0x8013300C
        ElementNotFound             = -2146226163, // 0x8013300D
        EngineBusy                  = -2146226162, // 0x8013300E
        EngineCannotClose           = -2146226161, // 0x8013300F
        EngineCannotReset           = -2146226160, // 0x80133010
        EngineClosed                = -2146226159, // 0x80133011
        EngineEmpty                 = -2146226159, // 0x80133012
        EngineInitialized           = -2146226157, // 0x80133013
        EngineNameInUse             = -2146226156, // 0x80133014
        EngineNotCompiled           = -2146226155, // 0x80133015
        EngineNotInitialized        = -2146226154, // 0x80133016
        EngineNotRunning            = -2146226153, // 0x80133017
        EngineRunning               = -2146226152, // 0x80133018
        EventSourceInvalid          = -2146226151, // 0x80133019
        EventSourceNameInUse        = -2146226150, // 0x8013301A
        EventSourceNameInvalid      = -2146226149, // 0x8013301B
        EventSourceNotFound         = -2146226148, // 0x8013301C
        EventSourceTypeInvalid      = -2146226147, // 0x8013301D
        GetCompiledStateFailed      = -2146226146, // 0x8013301E
        GlobalInstanceInvalid       = -2146226145, // 0x8013301F
        GlobalInstanceTypeInvalid   = -2146226144, // 0x80133020
        InternalCompilerError       = -2146226143, // 0x80133021
        ItemCannotBeRemoved         = -2146226142, // 0x80133022
        ItemFlagNotSupported        = -2146226141, // 0x80133023
        ItemNameInUse               = -2146226140, // 0x80133024
        ItemNameInvalid             = -2146226139, // 0x80133025
        ItemNotFound                = -2146226138, // 0x80133026
        ItemTypeNotSupported        = -2146226137, // 0x80133027
        LCIDNotSupported            = -2146226136, // 0x80133028
        LoadElementFailed           = -2146226135, // 0x80133029
        NotificationInvalid         = -2146226134, // 0x8013302A
        OptionInvalid               = -2146226133, // 0x8013302B
        OptionNotSupported          = -2146226132, // 0x8013302C
        RevokeFailed                = -2146226131, // 0x8013302D
        RootMonikerAlreadySet       = -2146226130, // 0x8013302E
        RootMonikerInUse            = -2146226129, // 0x8013302F
        RootMonikerInvalid          = -2146226128, // 0x80133030
        RootMonikerNotSet           = -2146226127, // 0x80133031
        RootMonikerProtocolInvalid  = -2146226126, // 0x80133032
        RootNamespaceInvalid        = -2146226125, // 0x80133033
        RootNamespaceNotSet         = -2146226124, // 0x80133034
        SaveCompiledStateFailed     = -2146226123, // 0x80133035
        SaveElementFailed           = -2146226122, // 0x80133036
        SiteAlreadySet              = -2146226121, // 0x80133037
        SiteInvalid                 = -2146226120, // 0x80133038
        SiteNotSet                  = -2146226119, // 0x80133039
        SourceItemNotAvailable      = -2146226118, // 0x8013303A
        SourceMonikerNotAvailable   = -2146226117, // 0x8013303B
        URLInvalid                  = -2146226116, // 0x8013303C
        
        BrowserNotExist             = -2146226115, // 0x8013303D
        DebuggeeNotStarted          = -2146226114, // 0x8013303E
        EngineNameInvalid           = -2146226113, // 0x8013303F
        EngineNotExist              = -2146226112, // 0x80133040
        FileFormatUnsupported       = -2146226111, // 0x80133041
        FileTypeUnknown             = -2146226110, // 0x80133042
        ItemCannotBeRenamed         = -2146226109, // 0x80133043
        MissingSource               = -2146226108, // 0x80133044
        NotInitCompleted            = -2146226107, // 0x80133045
        NameTooLong                 = -2146226106, // 0x80133046
        ProcNameInUse               = -2146226105, // 0x80133047
        ProcNameInvalid             = -2146226104, // 0x80133048
        VsaServerDown               = -2146226103, // 0x80133049   
        MissingPdb                  = -2146226102, // 0x8013304A
        NotClientSideAndNoUrl       = -2146226101, // 0x8013304B
        CannotAttachToWebServer     = -2146226100, // 0x8013304C
        EngineNameNotSet            = -2146226099, // 0x8013304D
                       
        UnknownError                = -2146225921, // 0x801330FF
    } // VsaErr

    [System.SerializableAttribute]
    [PermissionSetAttribute(SecurityAction.Demand, Name = "FullTrust")]
    [Guid("5f44bb1a-465a-37db-8e84-acc733bf7541")]
    [Obsolete(VsaObsolete.Description)]
    public class VsaException : ExternalException
    {
        //Standard constructors
        public VsaException() {}
        public VsaException(string message) : base(message) {}
        public VsaException(string message, Exception innerException) : base(message, innerException) {}

        //Serialization constructor
        public VsaException (SerializationInfo info, StreamingContext context) : base(info, context)
        {
          //deserialize value
          HResult = (Int32)info.GetValue("VsaException_HResult", typeof(Int32));
          HelpLink = (String)info.GetValue("VsaException_HelpLink", typeof(String));
          Source = (String)info.GetValue("VsaException_Source", typeof(String));
        }

	//serializes the object
	[PermissionSetAttribute(SecurityAction.Demand, Name = "FullTrust")]
	[SecurityPermissionAttribute(SecurityAction.Demand, SerializationFormatter = true)]	
        public override void GetObjectData(SerializationInfo info, StreamingContext context) 
        {  
           base.GetObjectData(info, context);

           //serialize value 
           info.AddValue("VsaException_HResult", HResult);  
           info.AddValue("VsaException_HelpLink", HelpLink);
           info.AddValue("VsaException_Source", Source);
        }

        public override String ToString()
        {
            if ((null != this.Message) && ("" != this.Message))
                return "Microsoft.Vsa.VsaException: " + System.Enum.GetName(((VsaError)HResult).GetType(), (VsaError)HResult) + " (0x" + String.Format(System.Globalization.CultureInfo.InvariantCulture, "{0,8:X}", HResult) + "): " + Message;    
            else
                return "Microsoft.Vsa.VsaException: " + System.Enum.GetName(((VsaError)HResult).GetType(), (VsaError)HResult) + " (0x" + String.Format(System.Globalization.CultureInfo.InvariantCulture, "{0,8:X}", HResult) + ").";    
        }

        public VsaException(VsaError error) : base(String.Empty, (int)error)
        {
        }

        public VsaException(VsaError error, string message) : base(message, (int)error)
        {            
        }

        public VsaException(VsaError error, string message, Exception innerException) : base(message, innerException)
        {
           HResult = (int)error;
        }

        public new VsaError ErrorCode
        {
            get { return (VsaError)HResult; }
        }
    } // VsaException

    // A custom attribute to identify this assembly as being generated by VSA
    [type:System.AttributeUsageAttribute(System.AttributeTargets.All)]
    [PermissionSetAttribute(SecurityAction.Demand, Name = "FullTrust")]
    [Guid("7f64f934-c1cc-338e-b695-f64d71e820fe")]
    [Obsolete(VsaObsolete.Description)]
    public class VsaModule : System.Attribute
    {
        public VsaModule(bool bIsVsaModule)
        {
            IsVsaModule = bIsVsaModule;
        }
        
        public bool IsVsaModule
        {
            get 
            { return isVsaModule; }
            set 
            { isVsaModule = value; }
        }
        
        private bool isVsaModule;    
    } // VsaModule
    
} // Microsoft.Vsa
