Imports System
Imports System.ComponentModel
Imports System.Management
Imports System.Collections
Imports System.Globalization
Imports Microsoft.VisualBasic

Namespace WMI.Win32
    
    'Functions ShouldSerialize<PropertyName> are functions used by VS property browser to check if a particular property has to be serialized. These functions are added for all ValueType properties ( properties of type Int32, BOOL etc.. which cannot be set to null). These functions use Is<PropertyName>Null function. These functions are also used in the TypeConverter implementation for the properties to check for NULL value of property so that an empty value can be shown in Property browser in case of Drag and Drop in Visual studio.
    'Functions Is<PropertyName>Null() are used to check if a property is NULL.
    'Functions Reset<PropertyName> are added for Nullable Read/Write properties. These functions are used by VS designer in property browser to set a property to NULL.
    'Every property added to the class for WMI property has attributes set to define its behavior in Visual Studio designer and also to define a TypeConverter to be used.
    'Datetime conversion functions ToDateTime and ToDmtfDateTime are added to the class to convert DMTF datetime to System.DateTime and vice-versa.
    'An Early Bound class generated for the WMI class.Win32_Process
    Public Class Process
        Inherits System.ComponentModel.Component
        
        'Private property to hold the WMI namespace in which the class resides.
        Private Shared CreatedWmiNamespace As String = "root\cimv2"
        
        'Private property to hold the name of WMI class which created this class.
        Private Shared CreatedClassName As String = "Win32_Process"
        
        'Private member variable to hold the ManagementScope which is used by the various methods.
        Private Shared statMgmtScope As System.Management.ManagementScope = Nothing
        
        Private PrivateSystemProperties As ManagementSystemProperties
        
        'Underlying lateBound WMI object.
        Private PrivateLateBoundObject As System.Management.ManagementObject
        
        'Member variable to store the 'automatic commit' behavior for the class.
        Private AutoCommitProp As Boolean
        
        'Private variable to hold the embedded property representing the instance.
        Private embeddedObj As System.Management.ManagementBaseObject
        
        'The current WMI object used
        Private curObj As System.Management.ManagementBaseObject
        
        'Flag to indicate if the instance is an embedded object.
        Private isEmbedded As Boolean
        
        'Below are different overloads of constructors to initialize an instance of the class with a WMI object.
        Public Sub New()
            MyBase.New
            Me.InitializeObject(Nothing, Nothing, Nothing)
        End Sub
        
        Public Sub New(ByVal keyHandle As String)
            MyBase.New
            Me.InitializeObject(Nothing, New System.Management.ManagementPath(Process.ConstructPath(keyHandle)), Nothing)
        End Sub
        
        Public Sub New(ByVal mgmtScope As System.Management.ManagementScope, ByVal keyHandle As String)
            MyBase.New
            Me.InitializeObject(CType(mgmtScope,System.Management.ManagementScope), New System.Management.ManagementPath(Process.ConstructPath(keyHandle)), Nothing)
        End Sub
        
        Public Sub New(ByVal path As System.Management.ManagementPath, ByVal getOptions As System.Management.ObjectGetOptions)
            MyBase.New
            Me.InitializeObject(Nothing, path, getOptions)
        End Sub
        
        Public Sub New(ByVal mgmtScope As System.Management.ManagementScope, ByVal path As System.Management.ManagementPath)
            MyBase.New
            Me.InitializeObject(mgmtScope, path, Nothing)
        End Sub
        
        Public Sub New(ByVal path As System.Management.ManagementPath)
            MyBase.New
            Me.InitializeObject(Nothing, path, Nothing)
        End Sub
        
        Public Sub New(ByVal mgmtScope As System.Management.ManagementScope, ByVal path As System.Management.ManagementPath, ByVal getOptions As System.Management.ObjectGetOptions)
            MyBase.New
            Me.InitializeObject(mgmtScope, path, getOptions)
        End Sub
        
        Public Sub New(ByVal theObject As System.Management.ManagementObject)
            MyBase.New
            Initialize
            If (CheckIfProperClass(theObject) = true) Then
                PrivateLateBoundObject = theObject
                PrivateSystemProperties = New ManagementSystemProperties(PrivateLateBoundObject)
                curObj = PrivateLateBoundObject
            Else
                Throw New System.ArgumentException("Class name does not match.")
            End If
        End Sub
        
        Public Sub New(ByVal theObject As System.Management.ManagementBaseObject)
            MyBase.New
            Initialize
            If (CheckIfProperClass(theObject) = true) Then
                embeddedObj = theObject
                PrivateSystemProperties = New ManagementSystemProperties(theObject)
                curObj = embeddedObj
                isEmbedded = true
            Else
                Throw New System.ArgumentException("Class name does not match.")
            End If
        End Sub
        
        'Property returns the namespace of the WMI class.
        <Browsable(true),  _
         DesignerSerializationVisibility(DesignerSerializationVisibility.Hidden)>  _
        Public ReadOnly Property OriginatingNamespace() As String
            Get
                Return "root\cimv2"
            End Get
        End Property
        
        <Browsable(true),  _
         DesignerSerializationVisibility(DesignerSerializationVisibility.Hidden)>  _
        Public ReadOnly Property ManagementClassName() As String
            Get
                Dim strRet As String = CreatedClassName
                If (Not (curObj) Is Nothing) Then
                    If (Not (curObj.ClassPath) Is Nothing) Then
                        strRet = CType(curObj("__CLASS"),String)
                        If ((strRet Is Nothing)  _
                                    OrElse (strRet Is String.Empty)) Then
                            strRet = CreatedClassName
                        End If
                    End If
                End If
                Return strRet
            End Get
        End Property
        
        'Property pointing to an embedded object to get System properties of the WMI object.
        <Browsable(true),  _
         DesignerSerializationVisibility(DesignerSerializationVisibility.Hidden)>  _
        Public ReadOnly Property SystemProperties() As ManagementSystemProperties
            Get
                Return PrivateSystemProperties
            End Get
        End Property
        
        'Property returning the underlying lateBound object.
        <Browsable(false),  _
         DesignerSerializationVisibility(DesignerSerializationVisibility.Hidden)>  _
        Public ReadOnly Property LateBoundObject() As System.Management.ManagementBaseObject
            Get
                Return curObj
            End Get
        End Property
        
        'ManagementScope of the object.
        <Browsable(true),  _
         DesignerSerializationVisibility(DesignerSerializationVisibility.Hidden)>  _
        Public Property Scope() As System.Management.ManagementScope
            Get
                If (isEmbedded = false) Then
                    Return PrivateLateBoundObject.Scope
                Else
                    Return Nothing
                End If
            End Get
            Set
                If (isEmbedded = false) Then
                    PrivateLateBoundObject.Scope = value
                End If
            End Set
        End Property
        
        'Property to show the commit behavior for the WMI object. If true, WMI object will be automatically saved after each property modification.(ie. Put() is called after modification of a property).
        <Browsable(false),  _
         DesignerSerializationVisibility(DesignerSerializationVisibility.Hidden)>  _
        Public Property AutoCommit() As Boolean
            Get
                Return AutoCommitProp
            End Get
            Set
                AutoCommitProp = value
            End Set
        End Property
        
        'The ManagementPath of the underlying WMI object.
        <Browsable(true)>  _
        Public Property Path() As System.Management.ManagementPath
            Get
                If (isEmbedded = false) Then
                    Return PrivateLateBoundObject.Path
                Else
                    Return Nothing
                End If
            End Get
            Set
                If (isEmbedded = false) Then
                    If (CheckIfProperClass(Nothing, value, Nothing) <> true) Then
                        Throw New System.ArgumentException("Class name does not match.")
                    End If
                    PrivateLateBoundObject.Path = value
                End If
            End Set
        End Property
        
        'Public static scope property which is used by the various methods.
        <Browsable(true),  _
         DesignerSerializationVisibility(DesignerSerializationVisibility.Hidden)>  _
        Public Shared Property StaticScope() As System.Management.ManagementScope
            Get
                Return statMgmtScope
            End Get
            Set
                statMgmtScope = value
            End Set
        End Property
        
        <Browsable(true),  _
         DesignerSerializationVisibility(DesignerSerializationVisibility.Hidden),  _
         Description("The Caption property is a short textual description (one-line string) of the obje"& _ 
            "ct.")>  _
        Public ReadOnly Property Caption() As String
            Get
                Return CType(curObj("Caption"),String)
            End Get
        End Property
        
        <Browsable(true),  _
         DesignerSerializationVisibility(DesignerSerializationVisibility.Hidden),  _
         Description("The CommandLine property specifies the command line used to start a particular pr"& _ 
            "ocess, if applicable.")>  _
        Public ReadOnly Property CommandLine() As String
            Get
                Return CType(curObj("CommandLine"),String)
            End Get
        End Property
        
        <Browsable(true),  _
         DesignerSerializationVisibility(DesignerSerializationVisibility.Hidden),  _
         Description("CreationClassName indicates the name of the class or the subclass used in the cre"& _ 
            "ation of an instance. When used with the other key properties of this class, thi"& _ 
            "s property allows all instances of this class and its subclasses to be uniquely "& _ 
            "identified.")>  _
        Public ReadOnly Property CreationClassName() As String
            Get
                Return CType(curObj("CreationClassName"),String)
            End Get
        End Property
        
        <Browsable(false),  _
         DesignerSerializationVisibility(DesignerSerializationVisibility.Hidden)>  _
        Public ReadOnly Property IsCreationDateNull() As Boolean
            Get
                If (curObj("CreationDate") Is Nothing) Then
                    Return true
                Else
                    Return false
                End If
            End Get
        End Property
        
        <Browsable(true),  _
         DesignerSerializationVisibility(DesignerSerializationVisibility.Hidden),  _
         Description("Time that the process began executing."),  _
         TypeConverter(GetType(WMIValueTypeConverter))>  _
        Public ReadOnly Property CreationDate() As Date
            Get
                If (Not (curObj("CreationDate")) Is Nothing) Then
                    Return ToDateTime(CType(curObj("CreationDate"),String))
                Else
                    Return Date.MinValue
                End If
            End Get
        End Property
        
        <Browsable(true),  _
         DesignerSerializationVisibility(DesignerSerializationVisibility.Hidden),  _
         Description("CSCreationClassName contains the scoping computer system's creation class name.")>  _
        Public ReadOnly Property CSCreationClassName() As String
            Get
                Return CType(curObj("CSCreationClassName"),String)
            End Get
        End Property
        
        <Browsable(true),  _
         DesignerSerializationVisibility(DesignerSerializationVisibility.Hidden),  _
         Description("The scoping computer system's name.")>  _
        Public ReadOnly Property CSName() As String
            Get
                Return CType(curObj("CSName"),String)
            End Get
        End Property
        
        <Browsable(true),  _
         DesignerSerializationVisibility(DesignerSerializationVisibility.Hidden),  _
         Description("The Description property provides a textual description of the object. ")>  _
        Public ReadOnly Property Description() As String
            Get
                Return CType(curObj("Description"),String)
            End Get
        End Property
        
        <Browsable(true),  _
         DesignerSerializationVisibility(DesignerSerializationVisibility.Hidden),  _
         Description("The ExecutablePath property indicates the path to the executable file of the proc"& _ 
            "ess."&Global.Microsoft.VisualBasic.ChrW(10)&"Example: C:\WINDOWS\EXPLORER.EXE")>  _
        Public ReadOnly Property ExecutablePath() As String
            Get
                Return CType(curObj("ExecutablePath"),String)
            End Get
        End Property
        
        <Browsable(false),  _
         DesignerSerializationVisibility(DesignerSerializationVisibility.Hidden)>  _
        Public ReadOnly Property IsExecutionStateNull() As Boolean
            Get
                If (curObj("ExecutionState") Is Nothing) Then
                    Return true
                Else
                    Return false
                End If
            End Get
        End Property
        
        <Browsable(true),  _
         DesignerSerializationVisibility(DesignerSerializationVisibility.Hidden),  _
         Description("Indicates the current operating condition of the process. Values include ready (2"& _ 
            "), running (3), and blocked (4), among others."),  _
         TypeConverter(GetType(WMIValueTypeConverter))>  _
        Public ReadOnly Property ExecutionState() As ExecutionStateValues
            Get
                If (curObj("ExecutionState") Is Nothing) Then
                    Return CType(System.Convert.ToInt32(10),ExecutionStateValues)
                End If
                Return CType(System.Convert.ToInt32(curObj("ExecutionState")),ExecutionStateValues)
            End Get
        End Property
        
        <Browsable(true),  _
         DesignerSerializationVisibility(DesignerSerializationVisibility.Hidden),  _
         Description("A string used to identify the process. A process ID is a kind of process handle.")>  _
        Public ReadOnly Property Handle() As String
            Get
                Return CType(curObj("Handle"),String)
            End Get
        End Property
        
        <Browsable(false),  _
         DesignerSerializationVisibility(DesignerSerializationVisibility.Hidden)>  _
        Public ReadOnly Property IsHandleCountNull() As Boolean
            Get
                If (curObj("HandleCount") Is Nothing) Then
                    Return true
                Else
                    Return false
                End If
            End Get
        End Property
        
        <Browsable(true),  _
         DesignerSerializationVisibility(DesignerSerializationVisibility.Hidden),  _
         Description("The HandleCount property specifies the total number of handles currently open by "& _ 
            "this process. This number is the sum of the handles currently open by each threa"& _ 
            "d in this process. A handle is used to examine or modify the system resources. E"& _ 
            "ach handle has an entry in an internally maintained table. These entries contain"& _ 
            " the addresses of the resources and the means to identify the resource type."),  _
         TypeConverter(GetType(WMIValueTypeConverter))>  _
        Public ReadOnly Property HandleCount() As UInteger
            Get
                If (curObj("HandleCount") Is Nothing) Then
                    Return System.Convert.ToUInt32(0)
                End If
                Return CType(curObj("HandleCount"),UInteger)
            End Get
        End Property
        
        <Browsable(false),  _
         DesignerSerializationVisibility(DesignerSerializationVisibility.Hidden)>  _
        Public ReadOnly Property IsInstallDateNull() As Boolean
            Get
                If (curObj("InstallDate") Is Nothing) Then
                    Return true
                Else
                    Return false
                End If
            End Get
        End Property
        
        <Browsable(true),  _
         DesignerSerializationVisibility(DesignerSerializationVisibility.Hidden),  _
         Description("The InstallDate property is datetime value indicating when the object was install"& _ 
            "ed. A lack of a value does not indicate that the object is not installed."),  _
         TypeConverter(GetType(WMIValueTypeConverter))>  _
        Public ReadOnly Property InstallDate() As Date
            Get
                If (Not (curObj("InstallDate")) Is Nothing) Then
                    Return ToDateTime(CType(curObj("InstallDate"),String))
                Else
                    Return Date.MinValue
                End If
            End Get
        End Property
        
        <Browsable(false),  _
         DesignerSerializationVisibility(DesignerSerializationVisibility.Hidden)>  _
        Public ReadOnly Property IsKernelModeTimeNull() As Boolean
            Get
                If (curObj("KernelModeTime") Is Nothing) Then
                    Return true
                Else
                    Return false
                End If
            End Get
        End Property
        
        <Browsable(true),  _
         DesignerSerializationVisibility(DesignerSerializationVisibility.Hidden),  _
         Description("Time in kernel mode, in 100 nanoseconds. If this information is not available, a "& _ 
            "value of 0 should be used."),  _
         TypeConverter(GetType(WMIValueTypeConverter))>  _
        Public ReadOnly Property KernelModeTime() As ULong
            Get
                If (curObj("KernelModeTime") Is Nothing) Then
                    Return System.Convert.ToUInt64(0)
                End If
                Return CType(curObj("KernelModeTime"),ULong)
            End Get
        End Property
        
        <Browsable(false),  _
         DesignerSerializationVisibility(DesignerSerializationVisibility.Hidden)>  _
        Public ReadOnly Property IsMaximumWorkingSetSizeNull() As Boolean
            Get
                If (curObj("MaximumWorkingSetSize") Is Nothing) Then
                    Return true
                Else
                    Return false
                End If
            End Get
        End Property
        
        <Browsable(true),  _
         DesignerSerializationVisibility(DesignerSerializationVisibility.Hidden),  _
         Description("The MaximumWorkingSetSize property indicates the maximum working set size of the "& _ 
            "process. The working set of a process is the set of memory pages currently visib"& _ 
            "le to the process in physical RAM. These pages are resident and available for an"& _ 
            " application to use without triggering a page fault."&Global.Microsoft.VisualBasic.ChrW(10)&"Example: 1413120."),  _
         TypeConverter(GetType(WMIValueTypeConverter))>  _
        Public ReadOnly Property MaximumWorkingSetSize() As UInteger
            Get
                If (curObj("MaximumWorkingSetSize") Is Nothing) Then
                    Return System.Convert.ToUInt32(0)
                End If
                Return CType(curObj("MaximumWorkingSetSize"),UInteger)
            End Get
        End Property
        
        <Browsable(false),  _
         DesignerSerializationVisibility(DesignerSerializationVisibility.Hidden)>  _
        Public ReadOnly Property IsMinimumWorkingSetSizeNull() As Boolean
            Get
                If (curObj("MinimumWorkingSetSize") Is Nothing) Then
                    Return true
                Else
                    Return false
                End If
            End Get
        End Property
        
        <Browsable(true),  _
         DesignerSerializationVisibility(DesignerSerializationVisibility.Hidden),  _
         Description("The MinimumWorkingSetSize property indicates the minimum working set size of the "& _ 
            "process. The working set of a process is the set of memory pages currently visib"& _ 
            "le to the process in physical RAM. These pages are resident and available for an"& _ 
            " application to use without triggering a page fault."&Global.Microsoft.VisualBasic.ChrW(10)&"Example: 20480."),  _
         TypeConverter(GetType(WMIValueTypeConverter))>  _
        Public ReadOnly Property MinimumWorkingSetSize() As UInteger
            Get
                If (curObj("MinimumWorkingSetSize") Is Nothing) Then
                    Return System.Convert.ToUInt32(0)
                End If
                Return CType(curObj("MinimumWorkingSetSize"),UInteger)
            End Get
        End Property
        
        <Browsable(true),  _
         DesignerSerializationVisibility(DesignerSerializationVisibility.Hidden),  _
         Description("The Name property defines the label by which the object is known. When subclassed"& _ 
            ", the Name property can be overridden to be a Key property.")>  _
        Public ReadOnly Property Name() As String
            Get
                Return CType(curObj("Name"),String)
            End Get
        End Property
        
        <Browsable(true),  _
         DesignerSerializationVisibility(DesignerSerializationVisibility.Hidden),  _
         Description("The scoping operating system's creation class name.")>  _
        Public ReadOnly Property OSCreationClassName() As String
            Get
                Return CType(curObj("OSCreationClassName"),String)
            End Get
        End Property
        
        <Browsable(true),  _
         DesignerSerializationVisibility(DesignerSerializationVisibility.Hidden),  _
         Description("The scoping operating system's name.")>  _
        Public ReadOnly Property OSName() As String
            Get
                Return CType(curObj("OSName"),String)
            End Get
        End Property
        
        <Browsable(false),  _
         DesignerSerializationVisibility(DesignerSerializationVisibility.Hidden)>  _
        Public ReadOnly Property IsOtherOperationCountNull() As Boolean
            Get
                If (curObj("OtherOperationCount") Is Nothing) Then
                    Return true
                Else
                    Return false
                End If
            End Get
        End Property
        
        <Browsable(true),  _
         DesignerSerializationVisibility(DesignerSerializationVisibility.Hidden),  _
         Description("The OtherOperationCount property specifies the number of I/O operations performed"& _ 
            ", other than read and write operations."),  _
         TypeConverter(GetType(WMIValueTypeConverter))>  _
        Public ReadOnly Property OtherOperationCount() As ULong
            Get
                If (curObj("OtherOperationCount") Is Nothing) Then
                    Return System.Convert.ToUInt64(0)
                End If
                Return CType(curObj("OtherOperationCount"),ULong)
            End Get
        End Property
        
        <Browsable(false),  _
         DesignerSerializationVisibility(DesignerSerializationVisibility.Hidden)>  _
        Public ReadOnly Property IsOtherTransferCountNull() As Boolean
            Get
                If (curObj("OtherTransferCount") Is Nothing) Then
                    Return true
                Else
                    Return false
                End If
            End Get
        End Property
        
        <Browsable(true),  _
         DesignerSerializationVisibility(DesignerSerializationVisibility.Hidden),  _
         Description("The OtherTransferCount property specifies the amount of data transferred during o"& _ 
            "perations other than read and write operations."),  _
         TypeConverter(GetType(WMIValueTypeConverter))>  _
        Public ReadOnly Property OtherTransferCount() As ULong
            Get
                If (curObj("OtherTransferCount") Is Nothing) Then
                    Return System.Convert.ToUInt64(0)
                End If
                Return CType(curObj("OtherTransferCount"),ULong)
            End Get
        End Property
        
        <Browsable(false),  _
         DesignerSerializationVisibility(DesignerSerializationVisibility.Hidden)>  _
        Public ReadOnly Property IsPageFaultsNull() As Boolean
            Get
                If (curObj("PageFaults") Is Nothing) Then
                    Return true
                Else
                    Return false
                End If
            End Get
        End Property
        
        <Browsable(true),  _
         DesignerSerializationVisibility(DesignerSerializationVisibility.Hidden),  _
         Description("The PageFaults property indicates the number of page faults generated by the proc"& _ 
            "ess."&Global.Microsoft.VisualBasic.ChrW(10)&"Example: 10"),  _
         TypeConverter(GetType(WMIValueTypeConverter))>  _
        Public ReadOnly Property PageFaults() As UInteger
            Get
                If (curObj("PageFaults") Is Nothing) Then
                    Return System.Convert.ToUInt32(0)
                End If
                Return CType(curObj("PageFaults"),UInteger)
            End Get
        End Property
        
        <Browsable(false),  _
         DesignerSerializationVisibility(DesignerSerializationVisibility.Hidden)>  _
        Public ReadOnly Property IsPageFileUsageNull() As Boolean
            Get
                If (curObj("PageFileUsage") Is Nothing) Then
                    Return true
                Else
                    Return false
                End If
            End Get
        End Property
        
        <Browsable(true),  _
         DesignerSerializationVisibility(DesignerSerializationVisibility.Hidden),  _
         Description("The PageFileUsage property indicates the amountof page file space currently being"& _ 
            " used by the process."&Global.Microsoft.VisualBasic.ChrW(10)&"Example: 102435"),  _
         TypeConverter(GetType(WMIValueTypeConverter))>  _
        Public ReadOnly Property PageFileUsage() As UInteger
            Get
                If (curObj("PageFileUsage") Is Nothing) Then
                    Return System.Convert.ToUInt32(0)
                End If
                Return CType(curObj("PageFileUsage"),UInteger)
            End Get
        End Property
        
        <Browsable(false),  _
         DesignerSerializationVisibility(DesignerSerializationVisibility.Hidden)>  _
        Public ReadOnly Property IsParentProcessIdNull() As Boolean
            Get
                If (curObj("ParentProcessId") Is Nothing) Then
                    Return true
                Else
                    Return false
                End If
            End Get
        End Property
        
        <Browsable(true),  _
         DesignerSerializationVisibility(DesignerSerializationVisibility.Hidden),  _
         Description("The ParentProcessId property specifies the unique identifier of the process that "& _ 
            "created this process. Process identifier numbers are reused, so they only identi"& _ 
            "fy a process for the lifetime of that process. It is possible that the process i"& _ 
            "dentified by ParentProcessId has terminated, so ParentProcessId may not refer to"& _ 
            " an running process. It is also possible that ParentProcessId incorrectly refers"& _ 
            " to a process which re-used that process identifier. The CreationDate property c"& _ 
            "an be used to determine whether the specified parent was created after this proc"& _ 
            "ess was created."),  _
         TypeConverter(GetType(WMIValueTypeConverter))>  _
        Public ReadOnly Property ParentProcessId() As UInteger
            Get
                If (curObj("ParentProcessId") Is Nothing) Then
                    Return System.Convert.ToUInt32(0)
                End If
                Return CType(curObj("ParentProcessId"),UInteger)
            End Get
        End Property
        
        <Browsable(false),  _
         DesignerSerializationVisibility(DesignerSerializationVisibility.Hidden)>  _
        Public ReadOnly Property IsPeakPageFileUsageNull() As Boolean
            Get
                If (curObj("PeakPageFileUsage") Is Nothing) Then
                    Return true
                Else
                    Return false
                End If
            End Get
        End Property
        
        <Browsable(true),  _
         DesignerSerializationVisibility(DesignerSerializationVisibility.Hidden),  _
         Description("The PeakPageFileUsage property indicates the maximum amount of page file space  u"& _ 
            "sed during the life of the process."&Global.Microsoft.VisualBasic.ChrW(10)&"Example: 102367"),  _
         TypeConverter(GetType(WMIValueTypeConverter))>  _
        Public ReadOnly Property PeakPageFileUsage() As UInteger
            Get
                If (curObj("PeakPageFileUsage") Is Nothing) Then
                    Return System.Convert.ToUInt32(0)
                End If
                Return CType(curObj("PeakPageFileUsage"),UInteger)
            End Get
        End Property
        
        <Browsable(false),  _
         DesignerSerializationVisibility(DesignerSerializationVisibility.Hidden)>  _
        Public ReadOnly Property IsPeakVirtualSizeNull() As Boolean
            Get
                If (curObj("PeakVirtualSize") Is Nothing) Then
                    Return true
                Else
                    Return false
                End If
            End Get
        End Property
        
        <Browsable(true),  _
         DesignerSerializationVisibility(DesignerSerializationVisibility.Hidden),  _
         Description("The PeakVirtualSize property specifies the maximum virtual address space the proc"& _ 
            "ess has used at any one time. Use of virtual address space does not necessarily "& _ 
            "imply corresponding use of either disk or main memory pages. However, virtual sp"& _ 
            "ace is finite, and by using too much, the process might limit its ability to loa"& _ 
            "d libraries."),  _
         TypeConverter(GetType(WMIValueTypeConverter))>  _
        Public ReadOnly Property PeakVirtualSize() As ULong
            Get
                If (curObj("PeakVirtualSize") Is Nothing) Then
                    Return System.Convert.ToUInt64(0)
                End If
                Return CType(curObj("PeakVirtualSize"),ULong)
            End Get
        End Property
        
        <Browsable(false),  _
         DesignerSerializationVisibility(DesignerSerializationVisibility.Hidden)>  _
        Public ReadOnly Property IsPeakWorkingSetSizeNull() As Boolean
            Get
                If (curObj("PeakWorkingSetSize") Is Nothing) Then
                    Return true
                Else
                    Return false
                End If
            End Get
        End Property
        
        <Browsable(true),  _
         DesignerSerializationVisibility(DesignerSerializationVisibility.Hidden),  _
         Description("The PeakWorkingSetSize property indicates the peak working set size of the proces"& _ 
            "s."&Global.Microsoft.VisualBasic.ChrW(10)&"Example: 1413120"),  _
         TypeConverter(GetType(WMIValueTypeConverter))>  _
        Public ReadOnly Property PeakWorkingSetSize() As UInteger
            Get
                If (curObj("PeakWorkingSetSize") Is Nothing) Then
                    Return System.Convert.ToUInt32(0)
                End If
                Return CType(curObj("PeakWorkingSetSize"),UInteger)
            End Get
        End Property
        
        <Browsable(false),  _
         DesignerSerializationVisibility(DesignerSerializationVisibility.Hidden)>  _
        Public ReadOnly Property IsPriorityNull() As Boolean
            Get
                If (curObj("Priority") Is Nothing) Then
                    Return true
                Else
                    Return false
                End If
            End Get
        End Property
        
        <Browsable(true),  _
         DesignerSerializationVisibility(DesignerSerializationVisibility.Hidden),  _
         Description("The Priority property indicates the scheduling priority of the process within the"& _ 
            " operating system. The higher the value, the higher priority the process receive"& _ 
            "s. Priority values can range from 0 (lowest priority) to 31 (highest priority)."&Global.Microsoft.VisualBasic.ChrW(10)& _ 
            "Example: 7."),  _
         TypeConverter(GetType(WMIValueTypeConverter))>  _
        Public ReadOnly Property Priority() As UInteger
            Get
                If (curObj("Priority") Is Nothing) Then
                    Return System.Convert.ToUInt32(0)
                End If
                Return CType(curObj("Priority"),UInteger)
            End Get
        End Property
        
        <Browsable(false),  _
         DesignerSerializationVisibility(DesignerSerializationVisibility.Hidden)>  _
        Public ReadOnly Property IsPrivatePageCountNull() As Boolean
            Get
                If (curObj("PrivatePageCount") Is Nothing) Then
                    Return true
                Else
                    Return false
                End If
            End Get
        End Property
        
        <Browsable(true),  _
         DesignerSerializationVisibility(DesignerSerializationVisibility.Hidden),  _
         Description("The PrivatePageCount property specifies the current number of pages allocated tha"& _ 
            "t are accessible only to this process "),  _
         TypeConverter(GetType(WMIValueTypeConverter))>  _
        Public ReadOnly Property PrivatePageCount() As ULong
            Get
                If (curObj("PrivatePageCount") Is Nothing) Then
                    Return System.Convert.ToUInt64(0)
                End If
                Return CType(curObj("PrivatePageCount"),ULong)
            End Get
        End Property
        
        <Browsable(false),  _
         DesignerSerializationVisibility(DesignerSerializationVisibility.Hidden)>  _
        Public ReadOnly Property IsProcessIdNull() As Boolean
            Get
                If (curObj("ProcessId") Is Nothing) Then
                    Return true
                Else
                    Return false
                End If
            End Get
        End Property
        
        <Browsable(true),  _
         DesignerSerializationVisibility(DesignerSerializationVisibility.Hidden),  _
         Description("The ProcessId property contains the global process identifier that can be used to"& _ 
            " identify a process. The value is valid from the creation of the process until t"& _ 
            "he process is terminated."),  _
         TypeConverter(GetType(WMIValueTypeConverter))>  _
        Public ReadOnly Property ProcessId() As UInteger
            Get
                If (curObj("ProcessId") Is Nothing) Then
                    Return System.Convert.ToUInt32(0)
                End If
                Return CType(curObj("ProcessId"),UInteger)
            End Get
        End Property
        
        <Browsable(false),  _
         DesignerSerializationVisibility(DesignerSerializationVisibility.Hidden)>  _
        Public ReadOnly Property IsQuotaNonPagedPoolUsageNull() As Boolean
            Get
                If (curObj("QuotaNonPagedPoolUsage") Is Nothing) Then
                    Return true
                Else
                    Return false
                End If
            End Get
        End Property
        
        <Browsable(true),  _
         DesignerSerializationVisibility(DesignerSerializationVisibility.Hidden),  _
         Description("The QuotaNonPagedPoolUsage property indicates the quota amount of non-paged pool "& _ 
            "usage for the process."&Global.Microsoft.VisualBasic.ChrW(10)&"Example: 15"),  _
         TypeConverter(GetType(WMIValueTypeConverter))>  _
        Public ReadOnly Property QuotaNonPagedPoolUsage() As UInteger
            Get
                If (curObj("QuotaNonPagedPoolUsage") Is Nothing) Then
                    Return System.Convert.ToUInt32(0)
                End If
                Return CType(curObj("QuotaNonPagedPoolUsage"),UInteger)
            End Get
        End Property
        
        <Browsable(false),  _
         DesignerSerializationVisibility(DesignerSerializationVisibility.Hidden)>  _
        Public ReadOnly Property IsQuotaPagedPoolUsageNull() As Boolean
            Get
                If (curObj("QuotaPagedPoolUsage") Is Nothing) Then
                    Return true
                Else
                    Return false
                End If
            End Get
        End Property
        
        <Browsable(true),  _
         DesignerSerializationVisibility(DesignerSerializationVisibility.Hidden),  _
         Description("The QuotaPagedPoolUsage property indicates the quota amount of paged pool usage f"& _ 
            "or the process."&Global.Microsoft.VisualBasic.ChrW(10)&"Example: 22"),  _
         TypeConverter(GetType(WMIValueTypeConverter))>  _
        Public ReadOnly Property QuotaPagedPoolUsage() As UInteger
            Get
                If (curObj("QuotaPagedPoolUsage") Is Nothing) Then
                    Return System.Convert.ToUInt32(0)
                End If
                Return CType(curObj("QuotaPagedPoolUsage"),UInteger)
            End Get
        End Property
        
        <Browsable(false),  _
         DesignerSerializationVisibility(DesignerSerializationVisibility.Hidden)>  _
        Public ReadOnly Property IsQuotaPeakNonPagedPoolUsageNull() As Boolean
            Get
                If (curObj("QuotaPeakNonPagedPoolUsage") Is Nothing) Then
                    Return true
                Else
                    Return false
                End If
            End Get
        End Property
        
        <Browsable(true),  _
         DesignerSerializationVisibility(DesignerSerializationVisibility.Hidden),  _
         Description("The QuotaPeakNonPagedPoolUsage property indicates the peak quota amount of non-pa"& _ 
            "ged pool usage for the process."&Global.Microsoft.VisualBasic.ChrW(10)&"Example: 31"),  _
         TypeConverter(GetType(WMIValueTypeConverter))>  _
        Public ReadOnly Property QuotaPeakNonPagedPoolUsage() As UInteger
            Get
                If (curObj("QuotaPeakNonPagedPoolUsage") Is Nothing) Then
                    Return System.Convert.ToUInt32(0)
                End If
                Return CType(curObj("QuotaPeakNonPagedPoolUsage"),UInteger)
            End Get
        End Property
        
        <Browsable(false),  _
         DesignerSerializationVisibility(DesignerSerializationVisibility.Hidden)>  _
        Public ReadOnly Property IsQuotaPeakPagedPoolUsageNull() As Boolean
            Get
                If (curObj("QuotaPeakPagedPoolUsage") Is Nothing) Then
                    Return true
                Else
                    Return false
                End If
            End Get
        End Property
        
        <Browsable(true),  _
         DesignerSerializationVisibility(DesignerSerializationVisibility.Hidden),  _
         Description("The QuotaPeakPagedPoolUsage property indicates the peak quota amount of paged poo"& _ 
            "l usage for the process."&Global.Microsoft.VisualBasic.ChrW(10)&" Example: 31"),  _
         TypeConverter(GetType(WMIValueTypeConverter))>  _
        Public ReadOnly Property QuotaPeakPagedPoolUsage() As UInteger
            Get
                If (curObj("QuotaPeakPagedPoolUsage") Is Nothing) Then
                    Return System.Convert.ToUInt32(0)
                End If
                Return CType(curObj("QuotaPeakPagedPoolUsage"),UInteger)
            End Get
        End Property
        
        <Browsable(false),  _
         DesignerSerializationVisibility(DesignerSerializationVisibility.Hidden)>  _
        Public ReadOnly Property IsReadOperationCountNull() As Boolean
            Get
                If (curObj("ReadOperationCount") Is Nothing) Then
                    Return true
                Else
                    Return false
                End If
            End Get
        End Property
        
        <Browsable(true),  _
         DesignerSerializationVisibility(DesignerSerializationVisibility.Hidden),  _
         Description("The ReadOperationCount property specifies the number of read operations performed"& _ 
            "."),  _
         TypeConverter(GetType(WMIValueTypeConverter))>  _
        Public ReadOnly Property ReadOperationCount() As ULong
            Get
                If (curObj("ReadOperationCount") Is Nothing) Then
                    Return System.Convert.ToUInt64(0)
                End If
                Return CType(curObj("ReadOperationCount"),ULong)
            End Get
        End Property
        
        <Browsable(false),  _
         DesignerSerializationVisibility(DesignerSerializationVisibility.Hidden)>  _
        Public ReadOnly Property IsReadTransferCountNull() As Boolean
            Get
                If (curObj("ReadTransferCount") Is Nothing) Then
                    Return true
                Else
                    Return false
                End If
            End Get
        End Property
        
        <Browsable(true),  _
         DesignerSerializationVisibility(DesignerSerializationVisibility.Hidden),  _
         Description("The ReadTransferCount property specifies the amount of data read."),  _
         TypeConverter(GetType(WMIValueTypeConverter))>  _
        Public ReadOnly Property ReadTransferCount() As ULong
            Get
                If (curObj("ReadTransferCount") Is Nothing) Then
                    Return System.Convert.ToUInt64(0)
                End If
                Return CType(curObj("ReadTransferCount"),ULong)
            End Get
        End Property
        
        <Browsable(false),  _
         DesignerSerializationVisibility(DesignerSerializationVisibility.Hidden)>  _
        Public ReadOnly Property IsSessionIdNull() As Boolean
            Get
                If (curObj("SessionId") Is Nothing) Then
                    Return true
                Else
                    Return false
                End If
            End Get
        End Property
        
        <Browsable(true),  _
         DesignerSerializationVisibility(DesignerSerializationVisibility.Hidden),  _
         Description("The SessionId property specifies the unique identifier that is generated by the o"& _ 
            "perating system when the session is created. A session spans a period of time fr"& _ 
            "om log in to log out on a particular system."),  _
         TypeConverter(GetType(WMIValueTypeConverter))>  _
        Public ReadOnly Property SessionId() As UInteger
            Get
                If (curObj("SessionId") Is Nothing) Then
                    Return System.Convert.ToUInt32(0)
                End If
                Return CType(curObj("SessionId"),UInteger)
            End Get
        End Property
        
        <Browsable(true),  _
         DesignerSerializationVisibility(DesignerSerializationVisibility.Hidden),  _
         Description("The Status property is a string indicating the current status of the object. Vari"& _ 
            "ous operational and non-operational statuses can be defined. Operational statuse"& _ 
            "s are ""OK"", ""Degraded"" and ""Pred Fail"". ""Pred Fail"" indicates that an element ma"& _ 
            "y be functioning properly but predicting a failure in the near future. An exampl"& _ 
            "e is a SMART-enabled hard drive. Non-operational statuses can also be specified."& _ 
            " These are ""Error"", ""Starting"", ""Stopping"" and ""Service"". The latter, ""Service"","& _ 
            " could apply during mirror-resilvering of a disk, reload of a user permissions l"& _ 
            "ist, or other administrative work. Not all such work is on-line, yet the managed"& _ 
            " element is neither ""OK"" nor in one of the other states.")>  _
        Public ReadOnly Property Status() As String
            Get
                Return CType(curObj("Status"),String)
            End Get
        End Property
        
        <Browsable(false),  _
         DesignerSerializationVisibility(DesignerSerializationVisibility.Hidden)>  _
        Public ReadOnly Property IsTerminationDateNull() As Boolean
            Get
                If (curObj("TerminationDate") Is Nothing) Then
                    Return true
                Else
                    Return false
                End If
            End Get
        End Property
        
        <Browsable(true),  _
         DesignerSerializationVisibility(DesignerSerializationVisibility.Hidden),  _
         Description("Time that the process was stopped or terminated."),  _
         TypeConverter(GetType(WMIValueTypeConverter))>  _
        Public ReadOnly Property TerminationDate() As Date
            Get
                If (Not (curObj("TerminationDate")) Is Nothing) Then
                    Return ToDateTime(CType(curObj("TerminationDate"),String))
                Else
                    Return Date.MinValue
                End If
            End Get
        End Property
        
        <Browsable(false),  _
         DesignerSerializationVisibility(DesignerSerializationVisibility.Hidden)>  _
        Public ReadOnly Property IsThreadCountNull() As Boolean
            Get
                If (curObj("ThreadCount") Is Nothing) Then
                    Return true
                Else
                    Return false
                End If
            End Get
        End Property
        
        <Browsable(true),  _
         DesignerSerializationVisibility(DesignerSerializationVisibility.Hidden),  _
         Description("The ThreadCount property specifies the number of active threads in this process. "& _ 
            "An instruction is the basic unit of execution in a processor, and a thread is th"& _ 
            "e object that executes instructions. Every running process has at least one thre"& _ 
            "ad. This property is for computers running Windows NT only."),  _
         TypeConverter(GetType(WMIValueTypeConverter))>  _
        Public ReadOnly Property ThreadCount() As UInteger
            Get
                If (curObj("ThreadCount") Is Nothing) Then
                    Return System.Convert.ToUInt32(0)
                End If
                Return CType(curObj("ThreadCount"),UInteger)
            End Get
        End Property
        
        <Browsable(false),  _
         DesignerSerializationVisibility(DesignerSerializationVisibility.Hidden)>  _
        Public ReadOnly Property IsUserModeTimeNull() As Boolean
            Get
                If (curObj("UserModeTime") Is Nothing) Then
                    Return true
                Else
                    Return false
                End If
            End Get
        End Property
        
        <Browsable(true),  _
         DesignerSerializationVisibility(DesignerSerializationVisibility.Hidden),  _
         Description("Time in user mode, in 100 nanoseconds. If this information is not available, a va"& _ 
            "lue of 0 should be used."),  _
         TypeConverter(GetType(WMIValueTypeConverter))>  _
        Public ReadOnly Property UserModeTime() As ULong
            Get
                If (curObj("UserModeTime") Is Nothing) Then
                    Return System.Convert.ToUInt64(0)
                End If
                Return CType(curObj("UserModeTime"),ULong)
            End Get
        End Property
        
        <Browsable(false),  _
         DesignerSerializationVisibility(DesignerSerializationVisibility.Hidden)>  _
        Public ReadOnly Property IsVirtualSizeNull() As Boolean
            Get
                If (curObj("VirtualSize") Is Nothing) Then
                    Return true
                Else
                    Return false
                End If
            End Get
        End Property
        
        <Browsable(true),  _
         DesignerSerializationVisibility(DesignerSerializationVisibility.Hidden),  _
         Description("The VirtualSize property specifies the current size in bytes of the virtual addre"& _ 
            "ss space the process is using. Use of virtual address space does not necessarily"& _ 
            " imply corresponding use of either disk or main memory pages. Virtual space is f"& _ 
            "inite, and by using too much, the process can limit its ability to load librarie"& _ 
            "s."),  _
         TypeConverter(GetType(WMIValueTypeConverter))>  _
        Public ReadOnly Property VirtualSize() As ULong
            Get
                If (curObj("VirtualSize") Is Nothing) Then
                    Return System.Convert.ToUInt64(0)
                End If
                Return CType(curObj("VirtualSize"),ULong)
            End Get
        End Property
        
        <Browsable(true),  _
         DesignerSerializationVisibility(DesignerSerializationVisibility.Hidden),  _
         Description("The WindowsVersion property indicates the version of Windows in which the process"& _ 
            " is running."&Global.Microsoft.VisualBasic.ChrW(10)&"Example: 4.0")>  _
        Public ReadOnly Property WindowsVersion() As String
            Get
                Return CType(curObj("WindowsVersion"),String)
            End Get
        End Property
        
        <Browsable(false),  _
         DesignerSerializationVisibility(DesignerSerializationVisibility.Hidden)>  _
        Public ReadOnly Property IsWorkingSetSizeNull() As Boolean
            Get
                If (curObj("WorkingSetSize") Is Nothing) Then
                    Return true
                Else
                    Return false
                End If
            End Get
        End Property
        
        <Browsable(true),  _
         DesignerSerializationVisibility(DesignerSerializationVisibility.Hidden),  _
         Description("The amount of memory in bytes that a process needs to execute efficiently, for an"& _ 
            " operating system that uses page-based memory management. If an insufficient amo"& _ 
            "unt of memory is available (< working set size), thrashing will occur. If this i"& _ 
            "nformation is not known, NULL or 0 should be entered.  If this data is provided,"& _ 
            " it could be monitored to understand a process' changing memory requirements as "& _ 
            "execution proceeds."),  _
         TypeConverter(GetType(WMIValueTypeConverter))>  _
        Public ReadOnly Property WorkingSetSize() As ULong
            Get
                If (curObj("WorkingSetSize") Is Nothing) Then
                    Return System.Convert.ToUInt64(0)
                End If
                Return CType(curObj("WorkingSetSize"),ULong)
            End Get
        End Property
        
        <Browsable(false),  _
         DesignerSerializationVisibility(DesignerSerializationVisibility.Hidden)>  _
        Public ReadOnly Property IsWriteOperationCountNull() As Boolean
            Get
                If (curObj("WriteOperationCount") Is Nothing) Then
                    Return true
                Else
                    Return false
                End If
            End Get
        End Property
        
        <Browsable(true),  _
         DesignerSerializationVisibility(DesignerSerializationVisibility.Hidden),  _
         Description("The WriteOperationCount property specifies the number of write operations perform"& _ 
            "ed."),  _
         TypeConverter(GetType(WMIValueTypeConverter))>  _
        Public ReadOnly Property WriteOperationCount() As ULong
            Get
                If (curObj("WriteOperationCount") Is Nothing) Then
                    Return System.Convert.ToUInt64(0)
                End If
                Return CType(curObj("WriteOperationCount"),ULong)
            End Get
        End Property
        
        <Browsable(false),  _
         DesignerSerializationVisibility(DesignerSerializationVisibility.Hidden)>  _
        Public ReadOnly Property IsWriteTransferCountNull() As Boolean
            Get
                If (curObj("WriteTransferCount") Is Nothing) Then
                    Return true
                Else
                    Return false
                End If
            End Get
        End Property
        
        <Browsable(true),  _
         DesignerSerializationVisibility(DesignerSerializationVisibility.Hidden),  _
         Description("The WriteTransferCount property specifies the amount of data written."),  _
         TypeConverter(GetType(WMIValueTypeConverter))>  _
        Public ReadOnly Property WriteTransferCount() As ULong
            Get
                If (curObj("WriteTransferCount") Is Nothing) Then
                    Return System.Convert.ToUInt64(0)
                End If
                Return CType(curObj("WriteTransferCount"),ULong)
            End Get
        End Property
        
        Private Overloads Function CheckIfProperClass(ByVal mgmtScope As System.Management.ManagementScope, ByVal path As System.Management.ManagementPath, ByVal OptionsParam As System.Management.ObjectGetOptions) As Boolean
            If ((Not (path) Is Nothing)  _
                        AndAlso (String.Compare(path.ClassName, Me.ManagementClassName, true, System.Globalization.CultureInfo.InvariantCulture) = 0)) Then
                Return true
            Else
                Return CheckIfProperClass(New System.Management.ManagementObject(mgmtScope, path, OptionsParam))
            End If
        End Function
        
        Private Overloads Function CheckIfProperClass(ByVal theObj As System.Management.ManagementBaseObject) As Boolean
            If ((Not (theObj) Is Nothing)  _
                        AndAlso (String.Compare(CType(theObj("__CLASS"),String), Me.ManagementClassName, true, System.Globalization.CultureInfo.InvariantCulture) = 0)) Then
                Return true
            Else
                Dim parentClasses As System.Array = CType(theObj("__DERIVATION"),System.Array)
                If (Not (parentClasses) Is Nothing) Then
                    Dim count As Integer = 0
                    count = 0
                    Do While (count < parentClasses.Length)
                        If (String.Compare(CType(parentClasses.GetValue(count),String), Me.ManagementClassName, true, System.Globalization.CultureInfo.InvariantCulture) = 0) Then
                            Return true
                        End If
                        count = (count + 1)
                    Loop
                End If
            End If
            Return false
        End Function
        
        'Converts a given datetime in DMTF format to System.DateTime object.
        Shared Function ToDateTime(ByVal dmtfDate As String) As Date
            Dim initializer As Date = Date.MinValue
            Dim year As Integer = initializer.Year
            Dim month As Integer = initializer.Month
            Dim day As Integer = initializer.Day
            Dim hour As Integer = initializer.Hour
            Dim minute As Integer = initializer.Minute
            Dim second As Integer = initializer.Second
            Dim ticks As Long = 0
            Dim dmtf As String = dmtfDate
            Dim datetime As Date = Date.MinValue
            Dim tempString As String = String.Empty
            If (dmtf Is Nothing) Then
                Throw New System.ArgumentOutOfRangeException
            End If
            If (dmtf.Length = 0) Then
                Throw New System.ArgumentOutOfRangeException
            End If
            If (dmtf.Length <> 25) Then
                Throw New System.ArgumentOutOfRangeException
            End If
            Try 
                tempString = dmtf.Substring(0, 4)
                If ("****" <> tempString) Then
                    year = Integer.Parse(tempString)
                End If
                tempString = dmtf.Substring(4, 2)
                If ("**" <> tempString) Then
                    month = Integer.Parse(tempString)
                End If
                tempString = dmtf.Substring(6, 2)
                If ("**" <> tempString) Then
                    day = Integer.Parse(tempString)
                End If
                tempString = dmtf.Substring(8, 2)
                If ("**" <> tempString) Then
                    hour = Integer.Parse(tempString)
                End If
                tempString = dmtf.Substring(10, 2)
                If ("**" <> tempString) Then
                    minute = Integer.Parse(tempString)
                End If
                tempString = dmtf.Substring(12, 2)
                If ("**" <> tempString) Then
                    second = Integer.Parse(tempString)
                End If
                tempString = dmtf.Substring(15, 6)
                If ("******" <> tempString) Then
                    ticks = (Long.Parse(tempString) * CType((System.TimeSpan.TicksPerMillisecond / 1000),Long))
                End If
                If ((((((((year < 0)  _
                            OrElse (month < 0))  _
                            OrElse (day < 0))  _
                            OrElse (hour < 0))  _
                            OrElse (minute < 0))  _
                            OrElse (minute < 0))  _
                            OrElse (second < 0))  _
                            OrElse (ticks < 0)) Then
                    Throw New System.ArgumentOutOfRangeException
                End If
            Catch e As System.Exception
                Throw New System.ArgumentOutOfRangeException(Nothing, e.Message)
            End Try
            datetime = New Date(year, month, day, hour, minute, second, 0)
            datetime = datetime.AddTicks(ticks)
            Dim tickOffset As System.TimeSpan = System.TimeZone.CurrentTimeZone.GetUtcOffset(datetime)
            Dim UTCOffset As Integer = 0
            Dim OffsetToBeAdjusted As Integer = 0
            Dim OffsetMins As Long = CType((tickOffset.Ticks / System.TimeSpan.TicksPerMinute),Long)
            tempString = dmtf.Substring(22, 3)
            If (tempString <> "******") Then
                tempString = dmtf.Substring(21, 4)
                Try 
                    UTCOffset = Integer.Parse(tempString)
                Catch e As System.Exception
                    Throw New System.ArgumentOutOfRangeException(Nothing, e.Message)
                End Try
                OffsetToBeAdjusted = CType((OffsetMins - UTCOffset),Integer)
                datetime = datetime.AddMinutes(CType(OffsetToBeAdjusted,Double))
            End If
            Return datetime
        End Function
        
        'Converts a given System.DateTime object to DMTF datetime format.
        Shared Function ToDmtfDateTime(ByVal [date] As Date) As String
            Dim utcString As String = String.Empty
            Dim tickOffset As System.TimeSpan = System.TimeZone.CurrentTimeZone.GetUtcOffset([date])
            Dim OffsetMins As Long = CType((tickOffset.Ticks / System.TimeSpan.TicksPerMinute),Long)
            If (System.Math.Abs(OffsetMins) > 999) Then
                [date] = [date].ToUniversalTime
                utcString = "+000"
            Else
                If (tickOffset.Ticks >= 0) Then
                    utcString = String.Concat("+", CType((tickOffset.Ticks / System.TimeSpan.TicksPerMinute),System.Int64 ).ToString.PadLeft(3, Global.Microsoft.VisualBasic.ChrW(48)))
                Else
                    Dim strTemp As String = CType(OffsetMins,System.Int64 ).ToString
                    utcString = String.Concat("-", strTemp.Substring(1, (strTemp.Length - 1)).PadLeft(3, Global.Microsoft.VisualBasic.ChrW(48)))
                End If
            End If
            Dim dmtfDateTime As String = CType([date].Year,System.Int32 ).ToString.PadLeft(4, Global.Microsoft.VisualBasic.ChrW(48))
            dmtfDateTime = String.Concat(dmtfDateTime, CType([date].Month,System.Int32 ).ToString.PadLeft(2, Global.Microsoft.VisualBasic.ChrW(48)))
            dmtfDateTime = String.Concat(dmtfDateTime, CType([date].Day,System.Int32 ).ToString.PadLeft(2, Global.Microsoft.VisualBasic.ChrW(48)))
            dmtfDateTime = String.Concat(dmtfDateTime, CType([date].Hour,System.Int32 ).ToString.PadLeft(2, Global.Microsoft.VisualBasic.ChrW(48)))
            dmtfDateTime = String.Concat(dmtfDateTime, CType([date].Minute,System.Int32 ).ToString.PadLeft(2, Global.Microsoft.VisualBasic.ChrW(48)))
            dmtfDateTime = String.Concat(dmtfDateTime, CType([date].Second,System.Int32 ).ToString.PadLeft(2, Global.Microsoft.VisualBasic.ChrW(48)))
            dmtfDateTime = String.Concat(dmtfDateTime, ".")
            Dim dtTemp As Date = New Date([date].Year, [date].Month, [date].Day, [date].Hour, [date].Minute, [date].Second, 0)
            Dim microsec As Long = CType(((([date].Ticks - dtTemp.Ticks)  _
                        * 1000)  _
                        / System.TimeSpan.TicksPerMillisecond),Long)
            Dim strMicrosec As String = CType(microsec,System.Int64 ).ToString
            If (strMicrosec.Length > 6) Then
                strMicrosec = strMicrosec.Substring(0, 6)
            End If
            dmtfDateTime = String.Concat(dmtfDateTime, strMicrosec.PadLeft(6, Global.Microsoft.VisualBasic.ChrW(48)))
            dmtfDateTime = String.Concat(dmtfDateTime, utcString)
            Return dmtfDateTime
        End Function
        
        Private Function ShouldSerializeCreationDate() As Boolean
            If (Me.IsCreationDateNull = false) Then
                Return true
            End If
            Return false
        End Function
        
        Private Function ShouldSerializeExecutionState() As Boolean
            If (Me.IsExecutionStateNull = false) Then
                Return true
            End If
            Return false
        End Function
        
        Private Function ShouldSerializeHandleCount() As Boolean
            If (Me.IsHandleCountNull = false) Then
                Return true
            End If
            Return false
        End Function
        
        Private Function ShouldSerializeInstallDate() As Boolean
            If (Me.IsInstallDateNull = false) Then
                Return true
            End If
            Return false
        End Function
        
        Private Function ShouldSerializeKernelModeTime() As Boolean
            If (Me.IsKernelModeTimeNull = false) Then
                Return true
            End If
            Return false
        End Function
        
        Private Function ShouldSerializeMaximumWorkingSetSize() As Boolean
            If (Me.IsMaximumWorkingSetSizeNull = false) Then
                Return true
            End If
            Return false
        End Function
        
        Private Function ShouldSerializeMinimumWorkingSetSize() As Boolean
            If (Me.IsMinimumWorkingSetSizeNull = false) Then
                Return true
            End If
            Return false
        End Function
        
        Private Function ShouldSerializeOtherOperationCount() As Boolean
            If (Me.IsOtherOperationCountNull = false) Then
                Return true
            End If
            Return false
        End Function
        
        Private Function ShouldSerializeOtherTransferCount() As Boolean
            If (Me.IsOtherTransferCountNull = false) Then
                Return true
            End If
            Return false
        End Function
        
        Private Function ShouldSerializePageFaults() As Boolean
            If (Me.IsPageFaultsNull = false) Then
                Return true
            End If
            Return false
        End Function
        
        Private Function ShouldSerializePageFileUsage() As Boolean
            If (Me.IsPageFileUsageNull = false) Then
                Return true
            End If
            Return false
        End Function
        
        Private Function ShouldSerializeParentProcessId() As Boolean
            If (Me.IsParentProcessIdNull = false) Then
                Return true
            End If
            Return false
        End Function
        
        Private Function ShouldSerializePeakPageFileUsage() As Boolean
            If (Me.IsPeakPageFileUsageNull = false) Then
                Return true
            End If
            Return false
        End Function
        
        Private Function ShouldSerializePeakVirtualSize() As Boolean
            If (Me.IsPeakVirtualSizeNull = false) Then
                Return true
            End If
            Return false
        End Function
        
        Private Function ShouldSerializePeakWorkingSetSize() As Boolean
            If (Me.IsPeakWorkingSetSizeNull = false) Then
                Return true
            End If
            Return false
        End Function
        
        Private Function ShouldSerializePriority() As Boolean
            If (Me.IsPriorityNull = false) Then
                Return true
            End If
            Return false
        End Function
        
        Private Function ShouldSerializePrivatePageCount() As Boolean
            If (Me.IsPrivatePageCountNull = false) Then
                Return true
            End If
            Return false
        End Function
        
        Private Function ShouldSerializeProcessId() As Boolean
            If (Me.IsProcessIdNull = false) Then
                Return true
            End If
            Return false
        End Function
        
        Private Function ShouldSerializeQuotaNonPagedPoolUsage() As Boolean
            If (Me.IsQuotaNonPagedPoolUsageNull = false) Then
                Return true
            End If
            Return false
        End Function
        
        Private Function ShouldSerializeQuotaPagedPoolUsage() As Boolean
            If (Me.IsQuotaPagedPoolUsageNull = false) Then
                Return true
            End If
            Return false
        End Function
        
        Private Function ShouldSerializeQuotaPeakNonPagedPoolUsage() As Boolean
            If (Me.IsQuotaPeakNonPagedPoolUsageNull = false) Then
                Return true
            End If
            Return false
        End Function
        
        Private Function ShouldSerializeQuotaPeakPagedPoolUsage() As Boolean
            If (Me.IsQuotaPeakPagedPoolUsageNull = false) Then
                Return true
            End If
            Return false
        End Function
        
        Private Function ShouldSerializeReadOperationCount() As Boolean
            If (Me.IsReadOperationCountNull = false) Then
                Return true
            End If
            Return false
        End Function
        
        Private Function ShouldSerializeReadTransferCount() As Boolean
            If (Me.IsReadTransferCountNull = false) Then
                Return true
            End If
            Return false
        End Function
        
        Private Function ShouldSerializeSessionId() As Boolean
            If (Me.IsSessionIdNull = false) Then
                Return true
            End If
            Return false
        End Function
        
        Private Function ShouldSerializeTerminationDate() As Boolean
            If (Me.IsTerminationDateNull = false) Then
                Return true
            End If
            Return false
        End Function
        
        Private Function ShouldSerializeThreadCount() As Boolean
            If (Me.IsThreadCountNull = false) Then
                Return true
            End If
            Return false
        End Function
        
        Private Function ShouldSerializeUserModeTime() As Boolean
            If (Me.IsUserModeTimeNull = false) Then
                Return true
            End If
            Return false
        End Function
        
        Private Function ShouldSerializeVirtualSize() As Boolean
            If (Me.IsVirtualSizeNull = false) Then
                Return true
            End If
            Return false
        End Function
        
        Private Function ShouldSerializeWorkingSetSize() As Boolean
            If (Me.IsWorkingSetSizeNull = false) Then
                Return true
            End If
            Return false
        End Function
        
        Private Function ShouldSerializeWriteOperationCount() As Boolean
            If (Me.IsWriteOperationCountNull = false) Then
                Return true
            End If
            Return false
        End Function
        
        Private Function ShouldSerializeWriteTransferCount() As Boolean
            If (Me.IsWriteTransferCountNull = false) Then
                Return true
            End If
            Return false
        End Function
        
        <Browsable(true)>  _
        Public Overloads Sub CommitObject()
            If (isEmbedded = false) Then
                PrivateLateBoundObject.Put
            End If
        End Sub
        
        <Browsable(true)>  _
        Public Overloads Sub CommitObject(ByVal putOptions As System.Management.PutOptions)
            If (isEmbedded = false) Then
                PrivateLateBoundObject.Put(putOptions)
            End If
        End Sub
        
        Private Sub Initialize()
            AutoCommitProp = true
            isEmbedded = false
        End Sub
        
        Private Shared Function ConstructPath(ByVal keyHandle As String) As String
            Dim strPath As String = "root\cimv2:Win32_Process"
            strPath = String.Concat(strPath, String.Concat(".Handle=", String.Concat("""", String.Concat(keyHandle, """"))))
            Return strPath
        End Function
        
        Private Sub InitializeObject(ByVal mgmtScope As System.Management.ManagementScope, ByVal path As System.Management.ManagementPath, ByVal getOptions As System.Management.ObjectGetOptions)
            Initialize
            If (Not (path) Is Nothing) Then
                If (CheckIfProperClass(mgmtScope, path, getOptions) <> true) Then
                    Throw New System.ArgumentException("Class name does not match.")
                End If
            End If
            PrivateLateBoundObject = New System.Management.ManagementObject(mgmtScope, path, getOptions)
            PrivateSystemProperties = New ManagementSystemProperties(PrivateLateBoundObject)
            curObj = PrivateLateBoundObject
        End Sub
        
        'Different overloads of GetInstances() help in enumerating instances of the WMI class.
        Public Overloads Shared Function GetInstances() As ProcessCollection
            Return GetInstances(Nothing, Nothing, Nothing)
        End Function
        
        Public Overloads Shared Function GetInstances(ByVal condition As String) As ProcessCollection
            Return GetInstances(Nothing, condition, Nothing)
        End Function
        
        Public Overloads Shared Function GetInstances(ByVal selectedProperties() As System.String ) As ProcessCollection
            Return GetInstances(Nothing, Nothing, selectedProperties)
        End Function
        
        Public Overloads Shared Function GetInstances(ByVal condition As String, ByVal selectedProperties() As System.String ) As ProcessCollection
            Return GetInstances(Nothing, condition, selectedProperties)
        End Function
        
        Public Overloads Shared Function GetInstances(ByVal mgmtScope As System.Management.ManagementScope, ByVal enumOptions As System.Management.EnumerationOptions) As ProcessCollection
            If (mgmtScope Is Nothing) Then
                If (statMgmtScope Is Nothing) Then
                    mgmtScope = New System.Management.ManagementScope
                    mgmtScope.Path.NamespacePath = "root\cimv2"
                Else
                    mgmtScope = statMgmtScope
                End If
            End If
            Dim pathObj As System.Management.ManagementPath = New System.Management.ManagementPath
            pathObj.ClassName = "Win32_Process"
            pathObj.NamespacePath = "root\cimv2"
            Dim clsObject As System.Management.ManagementClass = New System.Management.ManagementClass(mgmtScope, pathObj, Nothing)
            If (enumOptions Is Nothing) Then
                enumOptions = New System.Management.EnumerationOptions
                enumOptions.EnsureLocatable = true
            End If
            Return New ProcessCollection(clsObject.GetInstances(enumOptions))
        End Function
        
        Public Overloads Shared Function GetInstances(ByVal mgmtScope As System.Management.ManagementScope, ByVal condition As String) As ProcessCollection
            Return GetInstances(mgmtScope, condition, Nothing)
        End Function
        
        Public Overloads Shared Function GetInstances(ByVal mgmtScope As System.Management.ManagementScope, ByVal selectedProperties() As System.String ) As ProcessCollection
            Return GetInstances(mgmtScope, Nothing, selectedProperties)
        End Function
        
        Public Overloads Shared Function GetInstances(ByVal mgmtScope As System.Management.ManagementScope, ByVal condition As String, ByVal selectedProperties() As System.String ) As ProcessCollection
            If (mgmtScope Is Nothing) Then
                If (statMgmtScope Is Nothing) Then
                    mgmtScope = New System.Management.ManagementScope
                    mgmtScope.Path.NamespacePath = "root\cimv2"
                Else
                    mgmtScope = statMgmtScope
                End If
            End If
            Dim ObjectSearcher As System.Management.ManagementObjectSearcher = New System.Management.ManagementObjectSearcher(mgmtScope, New SelectQuery("Win32_Process", condition, selectedProperties))
            Dim enumOptions As System.Management.EnumerationOptions = New System.Management.EnumerationOptions
            enumOptions.EnsureLocatable = true
            ObjectSearcher.Options = enumOptions
            Return New ProcessCollection(ObjectSearcher.Get)
        End Function
        
        <Browsable(true)>  _
        Public Shared Function CreateInstance() As Process
            Dim mgmtScope As System.Management.ManagementScope = Nothing
            If (statMgmtScope Is Nothing) Then
                mgmtScope = New System.Management.ManagementScope
                mgmtScope.Path.NamespacePath = CreatedWmiNamespace
            Else
                mgmtScope = statMgmtScope
            End If
            Dim mgmtPath As System.Management.ManagementPath = New System.Management.ManagementPath(CreatedClassName)
            Dim tmpMgmtClass As System.Management.ManagementClass = New System.Management.ManagementClass(mgmtScope, mgmtPath, Nothing)
            Return New Process(tmpMgmtClass.CreateInstance)
        End Function
        
        <Browsable(true)>  _
        Public Sub Delete()
            PrivateLateBoundObject.Delete
        End Sub
        
        Public Function AttachDebugger() As UInteger
            If (isEmbedded = false) Then
                Dim inParams As System.Management.ManagementBaseObject = Nothing
                Dim outParams As System.Management.ManagementBaseObject = PrivateLateBoundObject.InvokeMethod("AttachDebugger", inParams, Nothing)
                Return System.Convert.ToUInt32(outParams.Properties("ReturnValue").Value)
            Else
                Return System.Convert.ToUInt32(0)
            End If
        End Function
        
        Public Shared Function Create(ByVal CommandLine As String, ByVal CurrentDirectory As String, ByVal ProcessStartupInformation As System.Management.ManagementBaseObject, ByRef ProcessId As UInteger) As UInteger
            Dim IsMethodStatic As Boolean = true
            If (IsMethodStatic = true) Then
                Dim inParams As System.Management.ManagementBaseObject = Nothing
                Dim mgmtPath As System.Management.ManagementPath = New System.Management.ManagementPath(CreatedClassName)
                Dim classObj As System.Management.ManagementClass = New System.Management.ManagementClass(statMgmtScope, mgmtPath, Nothing)
                Dim EnablePrivileges As Boolean = classObj.Scope.Options.EnablePrivileges
                classObj.Scope.Options.EnablePrivileges = true
                inParams = classObj.GetMethodParameters("Create")
                inParams("CommandLine") = CType(CommandLine,System.String )
                inParams("CurrentDirectory") = CType(CurrentDirectory,System.String )
                inParams("ProcessStartupInformation") = CType(ProcessStartupInformation,System.Management.ManagementBaseObject )
                Dim outParams As System.Management.ManagementBaseObject = classObj.InvokeMethod("Create", inParams, Nothing)
                ProcessId = System.Convert.ToUInt32(outParams.Properties("ProcessId").Value)
                classObj.Scope.Options.EnablePrivileges = EnablePrivileges
                Return System.Convert.ToUInt32(outParams.Properties("ReturnValue").Value)
            Else
                ProcessId = System.Convert.ToUInt32(0)
                Return System.Convert.ToUInt32(0)
            End If
        End Function
        
        Public Function GetOwner(ByRef Domain As String, ByRef User As String) As UInteger
            If (isEmbedded = false) Then
                Dim inParams As System.Management.ManagementBaseObject = Nothing
                Dim EnablePrivileges As Boolean = PrivateLateBoundObject.Scope.Options.EnablePrivileges
                PrivateLateBoundObject.Scope.Options.EnablePrivileges = true
                Dim outParams As System.Management.ManagementBaseObject = PrivateLateBoundObject.InvokeMethod("GetOwner", inParams, Nothing)
                Domain = System.Convert.ToString(outParams.Properties("Domain").Value)
                User = System.Convert.ToString(outParams.Properties("User").Value)
                PrivateLateBoundObject.Scope.Options.EnablePrivileges = EnablePrivileges
                Return System.Convert.ToUInt32(outParams.Properties("ReturnValue").Value)
            Else
                Domain = Nothing
                User = Nothing
                Return System.Convert.ToUInt32(0)
            End If
        End Function
        
        Public Function GetOwnerSid(ByRef Sid As String) As UInteger
            If (isEmbedded = false) Then
                Dim inParams As System.Management.ManagementBaseObject = Nothing
                Dim EnablePrivileges As Boolean = PrivateLateBoundObject.Scope.Options.EnablePrivileges
                PrivateLateBoundObject.Scope.Options.EnablePrivileges = true
                Dim outParams As System.Management.ManagementBaseObject = PrivateLateBoundObject.InvokeMethod("GetOwnerSid", inParams, Nothing)
                Sid = System.Convert.ToString(outParams.Properties("Sid").Value)
                PrivateLateBoundObject.Scope.Options.EnablePrivileges = EnablePrivileges
                Return System.Convert.ToUInt32(outParams.Properties("ReturnValue").Value)
            Else
                Sid = Nothing
                Return System.Convert.ToUInt32(0)
            End If
        End Function
        
        Public Function SetPriority(ByVal Priority As Integer) As UInteger
            If (isEmbedded = false) Then
                Dim inParams As System.Management.ManagementBaseObject = Nothing
                Dim EnablePrivileges As Boolean = PrivateLateBoundObject.Scope.Options.EnablePrivileges
                PrivateLateBoundObject.Scope.Options.EnablePrivileges = true
                inParams = PrivateLateBoundObject.GetMethodParameters("SetPriority")
                inParams("Priority") = CType(Priority,System.Int32 )
                Dim outParams As System.Management.ManagementBaseObject = PrivateLateBoundObject.InvokeMethod("SetPriority", inParams, Nothing)
                PrivateLateBoundObject.Scope.Options.EnablePrivileges = EnablePrivileges
                Return System.Convert.ToUInt32(outParams.Properties("ReturnValue").Value)
            Else
                Return System.Convert.ToUInt32(0)
            End If
        End Function
        
        Public Function Terminate(ByVal Reason As UInteger) As UInteger
            If (isEmbedded = false) Then
                Dim inParams As System.Management.ManagementBaseObject = Nothing
                Dim EnablePrivileges As Boolean = PrivateLateBoundObject.Scope.Options.EnablePrivileges
                PrivateLateBoundObject.Scope.Options.EnablePrivileges = true
                inParams = PrivateLateBoundObject.GetMethodParameters("Terminate")
                inParams("Reason") = CType(Reason,System.UInt32 )
                Dim outParams As System.Management.ManagementBaseObject = PrivateLateBoundObject.InvokeMethod("Terminate", inParams, Nothing)
                PrivateLateBoundObject.Scope.Options.EnablePrivileges = EnablePrivileges
                Return System.Convert.ToUInt32(outParams.Properties("ReturnValue").Value)
            Else
                Return System.Convert.ToUInt32(0)
            End If
        End Function
        
        Public Enum ExecutionStateValues
            
            Unknown0 = 0
            
            Other0 = 1
            
            Ready = 2
            
            Running = 3
            
            Blocked = 4
            
            Suspended_Blocked = 5
            
            Suspended_Ready = 6
            
            Terminated = 7
            
            Stopped = 8
            
            Growing = 9
            
            NULL_ENUM_VALUE = 10
        End Enum
        
        'Enumerator implementation for enumerating instances of the class.
        Public Class ProcessCollection
            Inherits Object
            Implements ICollection
            
            Private privColObj As ManagementObjectCollection
            
            Public Sub New(ByVal objCollection As ManagementObjectCollection)
                MyBase.New
                privColObj = objCollection
            End Sub
            
            Public Overridable ReadOnly Property Count() As Integer Implements System.Collections.ICollection.Count
                Get
                    Return privColObj.Count
                End Get
            End Property
            
            Public Overridable ReadOnly Property IsSynchronized() As Boolean Implements System.Collections.ICollection.IsSynchronized
                Get
                    Return privColObj.IsSynchronized
                End Get
            End Property
            
            Public Overridable ReadOnly Property SyncRoot() As Object Implements System.Collections.ICollection.SyncRoot
                Get
                    Return Me
                End Get
            End Property
            
            Public Overridable Sub CopyTo(ByVal array As System.Array, ByVal index As Integer) Implements System.Collections.ICollection.CopyTo
                privColObj.CopyTo(array, index)
                Dim nCtr As Integer
                nCtr = 0
                Do While (nCtr < array.Length)
                    array.SetValue(New Process(CType(array.GetValue(nCtr),System.Management.ManagementObject)), nCtr)
                    nCtr = (nCtr + 1)
                Loop
            End Sub
            
            Public Overridable Function GetEnumerator() As System.Collections.IEnumerator Implements System.Collections.IEnumerable.GetEnumerator
                Return New ProcessEnumerator(privColObj.GetEnumerator)
            End Function
            
            Public Class ProcessEnumerator
                Inherits Object
                Implements System.Collections.IEnumerator
                
                Private privObjEnum As ManagementObjectCollection.ManagementObjectEnumerator
                
                Public Sub New(ByVal objEnum As ManagementObjectCollection.ManagementObjectEnumerator)
                    MyBase.New
                    privObjEnum = objEnum
                End Sub
                
                Public Overridable ReadOnly Property Current() As Object Implements System.Collections.IEnumerator.Current
                    Get
                        Return New Process(CType(privObjEnum.Current,System.Management.ManagementObject))
                    End Get
                End Property
                
                Public Overridable Function MoveNext() As Boolean Implements System.Collections.IEnumerator.MoveNext
                    Return privObjEnum.MoveNext
                End Function
                
                Public Overridable Sub Reset() Implements System.Collections.IEnumerator.Reset
                    privObjEnum.Reset
                End Sub
            End Class
        End Class
        
        'TypeConverter to handle null values for ValueType properties
        Public Class WMIValueTypeConverter
            Inherits TypeConverter
            
            Private baseConverter As TypeConverter
            
            Private baseType As System.Type
            
            Public Sub New(ByVal inBaseType As System.Type)
                MyBase.New
                baseConverter = TypeDescriptor.GetConverter(inBaseType)
                baseType = inBaseType
            End Sub
            
            Public Overloads Overrides Function CanConvertFrom(ByVal context As System.ComponentModel.ITypeDescriptorContext, ByVal srcType As System.Type) As Boolean
                Return baseConverter.CanConvertFrom(context, srcType)
            End Function
            
            Public Overloads Overrides Function CanConvertTo(ByVal context As System.ComponentModel.ITypeDescriptorContext, ByVal destinationType As System.Type) As Boolean
                Return baseConverter.CanConvertTo(context, destinationType)
            End Function
            
            Public Overloads Overrides Function ConvertFrom(ByVal context As System.ComponentModel.ITypeDescriptorContext, ByVal culture As System.Globalization.CultureInfo, ByVal value As Object) As Object
                Return baseConverter.ConvertFrom(context, culture, value)
            End Function
            
            Public Overloads Overrides Function CreateInstance(ByVal context As System.ComponentModel.ITypeDescriptorContext, ByVal dictionary As System.Collections.IDictionary) As Object
                Return baseConverter.CreateInstance(context, dictionary)
            End Function
            
            Public Overloads Overrides Function GetCreateInstanceSupported(ByVal context As System.ComponentModel.ITypeDescriptorContext) As Boolean
                Return baseConverter.GetCreateInstanceSupported(context)
            End Function
            
            Public Overloads Overrides Function GetProperties(ByVal context As System.ComponentModel.ITypeDescriptorContext, ByVal value As Object, ByVal attributeVar() As System.Attribute) As PropertyDescriptorCollection
                Return baseConverter.GetProperties(context, value, attributeVar)
            End Function
            
            Public Overloads Overrides Function GetPropertiesSupported(ByVal context As System.ComponentModel.ITypeDescriptorContext) As Boolean
                Return baseConverter.GetPropertiesSupported(context)
            End Function
            
            Public Overloads Overrides Function GetStandardValues(ByVal context As System.ComponentModel.ITypeDescriptorContext) As System.ComponentModel.TypeConverter.StandardValuesCollection
                Return baseConverter.GetStandardValues(context)
            End Function
            
            Public Overloads Overrides Function GetStandardValuesExclusive(ByVal context As System.ComponentModel.ITypeDescriptorContext) As Boolean
                Return baseConverter.GetStandardValuesExclusive(context)
            End Function
            
            Public Overloads Overrides Function GetStandardValuesSupported(ByVal context As System.ComponentModel.ITypeDescriptorContext) As Boolean
                Return baseConverter.GetStandardValuesSupported(context)
            End Function
            
            Public Overloads Overrides Function ConvertTo(ByVal context As System.ComponentModel.ITypeDescriptorContext, ByVal culture As System.Globalization.CultureInfo, ByVal value As Object, ByVal destinationType As System.Type) As Object
                If (baseType.BaseType Is GetType(System.[Enum])) Then
                    If (value.GetType Is destinationType) Then
                        Return value
                    End If
                    If (((value = Nothing)  _
                                AndAlso (Not (context) Is Nothing))  _
                                AndAlso (context.PropertyDescriptor.ShouldSerializeValue(context.Instance) = false)) Then
                        Return  "NULL_ENUM_VALUE" 
                    End If
                    Return baseConverter.ConvertTo(context, culture, value, destinationType)
                End If
                If ((baseType Is GetType(Boolean))  _
                            AndAlso (baseType.BaseType Is GetType(System.ValueType))) Then
                    If (((value = Nothing)  _
                                AndAlso (Not (context) Is Nothing))  _
                                AndAlso (context.PropertyDescriptor.ShouldSerializeValue(context.Instance) = false)) Then
                        Return ""
                    End If
                    Return baseConverter.ConvertTo(context, culture, value, destinationType)
                End If
                If ((Not (context) Is Nothing)  _
                            AndAlso (context.PropertyDescriptor.ShouldSerializeValue(context.Instance) = false)) Then
                    Return ""
                End If
                Return baseConverter.ConvertTo(context, culture, value, destinationType)
            End Function
        End Class
        
        'Embedded class to represent WMI system Properties.
        <TypeConverter(GetType(System.ComponentModel.ExpandableObjectConverter))>  _
        Public Class ManagementSystemProperties
            
            Private PrivateLateBoundObject As System.Management.ManagementBaseObject
            
            Public Sub New(ByVal ManagedObject As System.Management.ManagementBaseObject)
                MyBase.New
                PrivateLateBoundObject = ManagedObject
            End Sub
            
            <Browsable(true)>  _
            Public ReadOnly Property GENUS() As Integer
                Get
                    Return CType(PrivateLateBoundObject("__GENUS"),Integer)
                End Get
            End Property
            
            <Browsable(true)>  _
            Public ReadOnly Property [CLASS]() As String
                Get
                    Return CType(PrivateLateBoundObject("__CLASS"),String)
                End Get
            End Property
            
            <Browsable(true)>  _
            Public ReadOnly Property SUPERCLASS() As String
                Get
                    Return CType(PrivateLateBoundObject("__SUPERCLASS"),String)
                End Get
            End Property
            
            <Browsable(true)>  _
            Public ReadOnly Property DYNASTY() As String
                Get
                    Return CType(PrivateLateBoundObject("__DYNASTY"),String)
                End Get
            End Property
            
            <Browsable(true)>  _
            Public ReadOnly Property RELPATH() As String
                Get
                    Return CType(PrivateLateBoundObject("__RELPATH"),String)
                End Get
            End Property
            
            <Browsable(true)>  _
            Public ReadOnly Property PROPERTY_COUNT() As Integer
                Get
                    Return CType(PrivateLateBoundObject("__PROPERTY_COUNT"),Integer)
                End Get
            End Property
            
            <Browsable(true)>  _
            Public ReadOnly Property DERIVATION() As String()
                Get
                    Return CType(PrivateLateBoundObject("__DERIVATION"),String())
                End Get
            End Property
            
            <Browsable(true)>  _
            Public ReadOnly Property SERVER() As String
                Get
                    Return CType(PrivateLateBoundObject("__SERVER"),String)
                End Get
            End Property
            
            <Browsable(true)>  _
            Public ReadOnly Property [NAMESPACE]() As String
                Get
                    Return CType(PrivateLateBoundObject("__NAMESPACE"),String)
                End Get
            End Property
            
            <Browsable(true)>  _
            Public ReadOnly Property PATH() As String
                Get
                    Return CType(PrivateLateBoundObject("__PATH"),String)
                End Get
            End Property
        End Class
    End Class
End Namespace
