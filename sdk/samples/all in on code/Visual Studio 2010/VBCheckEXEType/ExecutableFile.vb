'*************************** Module Header ******************************'
' Module Name:  ExecutableFile.vb
' Project:	    VBCheckEXEType
' Copyright (c) Microsoft Corporation.
' 
' This class represents an executable file. It could get the image file header,
' image optinal header and data directories from the image file. Form these 
' headers, we can get whether this is a console application, whether this is 
' a .Net application and whether this is a 32bit native application. For .NET
' application, it could generate the full display name like 
'  System, Version=4.0.0.0, Culture=neutral, PublicKeyToken=b77a5c561934e089, processorArchitecture=MSIL
'  
' This source is subject to the Microsoft Public License.
' See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
' All other rights reserved.
' 
' THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND, 
' EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED 
' WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
'*************************************************************************'

Imports System.IO
Imports System.Text

Public Class ExecutableFile
    ''' <summary>
    ''' The path of the executable file.
    ''' </summary>
    Private _exeFilePath As String
    Public Property ExeFilePath() As String
        Get
            Return _exeFilePath
        End Get
        Private Set(ByVal value As String)
            _exeFilePath = value
        End Set
    End Property

    Private _imageFileHeader As IMAGE.IMAGE_FILE_HEADER

    ''' <summary>
    ''' The image file header.
    ''' </summary>
    Public ReadOnly Property ImageFileHeader() As IMAGE.IMAGE_FILE_HEADER
        Get
            Return _imageFileHeader
        End Get
    End Property

    Private _optinalHeader32 As IMAGE.IMAGE_OPTIONAL_HEADER32

    ''' <summary>
    ''' The image optinal header for 32bit executable file.
    ''' </summary>
    Public ReadOnly Property OptinalHeader32() As IMAGE.IMAGE_OPTIONAL_HEADER32
        Get
            Return _optinalHeader32
        End Get
    End Property

    Private _optinalHeader64 As IMAGE.IMAGE_OPTIONAL_HEADER64

    ''' <summary>
    ''' The image optinal header for 64bit executable file.
    ''' </summary>
    Public ReadOnly Property OptinalHeader64() As IMAGE.IMAGE_OPTIONAL_HEADER64
        Get
            Return _optinalHeader64
        End Get
    End Property

    Private _directoryValues As IMAGE.IMAGE_DATA_DIRECTORY_Values

    ''' <summary>
    ''' The data directories.
    ''' </summary>
    Public ReadOnly Property DirectoryValues() As IMAGE.IMAGE_DATA_DIRECTORY_Values
        Get
            Return _directoryValues
        End Get
    End Property

    ''' <summary>
    ''' Specify whether this is a console application. 
    ''' </summary>
    Public ReadOnly Property IsConsoleApplication() As Boolean
        Get
            If Is32bitImage Then
                Return _optinalHeader32.Subsystem = 3
            Else
                Return _optinalHeader64.Subsystem = 3
            End If
        End Get
    End Property

    ''' <summary>
    ''' Specify whether this is a .Net application. 
    ''' </summary>
    Public ReadOnly Property IsDotNetAssembly() As Boolean
        Get
            Return _directoryValues.Values(IMAGE.IMAGE_DATA_DIRECTORY_Values.IMAGE_DIRECTORY_ENTRY_COM_DESCRIPTOR).VirtualAddress <> 0
        End Get
    End Property

    ''' <summary>
    ''' Specify whether this is a 32bit application. 
    ''' </summary>
    Private _is32bitImage As Boolean
    Public Property Is32bitImage() As Boolean
        Get
            Return _is32bitImage
        End Get
        Private Set(ByVal value As Boolean)
            _is32bitImage = value
        End Set
    End Property



    Public Sub New(ByVal filePath As String)
        If Not File.Exists(filePath) Then
            Throw New ArgumentException("Process could not be found.")
        End If

        Me.ExeFilePath = filePath

        Me.ReadHeaders()
    End Sub

    ''' <summary>
    ''' Read the headers from the image file.
    ''' </summary>
    Private Sub ReadHeaders()

        Dim fs As FileStream = Nothing
        Dim binReader As BinaryReader = Nothing

        Try
            fs = New FileStream(ExeFilePath, FileMode.Open, FileAccess.Read)
            binReader = New BinaryReader(fs)

            ' Read the PE Header start offset.
            fs.Position = &H3C
            Dim headerOffset As UInt32 = binReader.ReadUInt32()

            ' Check if the offset is invalid
            If headerOffset > fs.Length - 5 Then
                Throw New ApplicationException("Invalid Image Format")
            End If

            ' Read the PE file signature
            fs.Position = headerOffset
            Dim signature As UInt32 = binReader.ReadUInt32()

            ' 0x00004550 is the letters "PE" followed by two terminating zeroes.
            If signature <> &H4550 Then
                Throw New ApplicationException("Invalid Image Format")
            End If

            ' Read the image file header. 
            Me._imageFileHeader.Machine = binReader.ReadUInt16()
            Me._imageFileHeader.NumberOfSections = binReader.ReadUInt16()
            Me._imageFileHeader.TimeDateStamp = binReader.ReadUInt32()
            Me._imageFileHeader.PointerToSymbolTable = binReader.ReadUInt32()
            Me._imageFileHeader.NumberOfSymbols = binReader.ReadUInt32()
            Me._imageFileHeader.SizeOfOptionalHeader = binReader.ReadUInt16()
            Me._imageFileHeader.Characteristics = CType(binReader.ReadUInt16(), IMAGE.IMAGE_FILE_Flag)

            ' Determine whether this is a 32bit assembly.
            Dim magic As UInt16 = binReader.ReadUInt16()
            If magic <> &H10B AndAlso magic <> &H20B Then
                Throw New ApplicationException("Invalid Image Format")
            End If

            ' Read the IMAGE_OPTIONAL_HEADER32 for 32bit application.
            If magic = &H10B Then
                Me.Is32bitImage = True

                Me._optinalHeader32.Magic = magic
                Me._optinalHeader32.MajorLinkerVersion = binReader.ReadByte()
                Me._optinalHeader32.MinorImageVersion = binReader.ReadByte()
                Me._optinalHeader32.SizeOfCode = binReader.ReadUInt32()
                Me._optinalHeader32.SizeOfInitializedData = binReader.ReadUInt32()
                Me._optinalHeader32.SizeOfUninitializedData = binReader.ReadUInt32()
                Me._optinalHeader32.AddressOfEntryPoint = binReader.ReadUInt32()
                Me._optinalHeader32.BaseOfCode = binReader.ReadUInt32()
                Me._optinalHeader32.BaseOfData = binReader.ReadUInt32()
                Me._optinalHeader32.ImageBase = binReader.ReadUInt32()
                Me._optinalHeader32.SectionAlignment = binReader.ReadUInt32()
                Me._optinalHeader32.FileAlignment = binReader.ReadUInt32()
                Me._optinalHeader32.MajorOperatingSystemVersion = binReader.ReadUInt16()
                Me._optinalHeader32.MinorOperatingSystemVersion = binReader.ReadUInt16()
                Me._optinalHeader32.MajorImageVersion = binReader.ReadUInt16()
                Me._optinalHeader32.MinorImageVersion = binReader.ReadUInt16()
                Me._optinalHeader32.MajorSubsystemVersion = binReader.ReadUInt16()
                Me._optinalHeader32.MinorSubsystemVersion = binReader.ReadUInt16()
                Me._optinalHeader32.Win32VersionValue = binReader.ReadUInt32()
                Me._optinalHeader32.SizeOfImage = binReader.ReadUInt32()
                Me._optinalHeader32.SizeOfHeaders = binReader.ReadUInt32()
                Me._optinalHeader32.CheckSum = binReader.ReadUInt32()
                Me._optinalHeader32.Subsystem = binReader.ReadUInt16()
                Me._optinalHeader32.DllCharacteristics = binReader.ReadUInt16()
                Me._optinalHeader32.SizeOfStackReserve = binReader.ReadUInt32()
                Me._optinalHeader32.SizeOfStackCommit = binReader.ReadUInt32()
                Me._optinalHeader32.SizeOfHeapReserve = binReader.ReadUInt32()
                Me._optinalHeader32.SizeOfHeapCommit = binReader.ReadUInt32()
                Me._optinalHeader32.LoaderFlags = binReader.ReadUInt32()
                Me._optinalHeader32.NumberOfRvaAndSizes = binReader.ReadUInt32()

                ' Read the IMAGE_OPTIONAL_HEADER64 for 64bit application.
            Else
                Me.Is32bitImage = False

                Me._optinalHeader64.Magic = magic
                Me._optinalHeader64.MajorLinkerVersion = binReader.ReadByte()
                Me._optinalHeader64.MinorImageVersion = binReader.ReadByte()
                Me._optinalHeader64.SizeOfCode = binReader.ReadUInt32()
                Me._optinalHeader64.SizeOfInitializedData = binReader.ReadUInt32()
                Me._optinalHeader64.SizeOfUninitializedData = binReader.ReadUInt32()
                Me._optinalHeader64.AddressOfEntryPoint = binReader.ReadUInt32()
                Me._optinalHeader64.BaseOfCode = binReader.ReadUInt32()
                Me._optinalHeader64.ImageBase = binReader.ReadUInt64()
                Me._optinalHeader64.SectionAlignment = binReader.ReadUInt32()
                Me._optinalHeader64.FileAlignment = binReader.ReadUInt32()
                Me._optinalHeader64.MajorOperatingSystemVersion = binReader.ReadUInt16()
                Me._optinalHeader64.MinorOperatingSystemVersion = binReader.ReadUInt16()
                Me._optinalHeader64.MajorImageVersion = binReader.ReadUInt16()
                Me._optinalHeader64.MinorImageVersion = binReader.ReadUInt16()
                Me._optinalHeader64.MajorSubsystemVersion = binReader.ReadUInt16()
                Me._optinalHeader64.MinorSubsystemVersion = binReader.ReadUInt16()
                Me._optinalHeader64.Win32VersionValue = binReader.ReadUInt32()
                Me._optinalHeader64.SizeOfImage = binReader.ReadUInt32()
                Me._optinalHeader64.SizeOfHeaders = binReader.ReadUInt32()
                Me._optinalHeader64.CheckSum = binReader.ReadUInt32()
                Me._optinalHeader64.Subsystem = binReader.ReadUInt16()
                Me._optinalHeader64.DllCharacteristics = binReader.ReadUInt16()
                Me._optinalHeader64.SizeOfStackReserve = binReader.ReadUInt64()
                Me._optinalHeader64.SizeOfStackCommit = binReader.ReadUInt64()
                Me._optinalHeader64.SizeOfHeapReserve = binReader.ReadUInt64()
                Me._optinalHeader64.SizeOfHeapCommit = binReader.ReadUInt64()
                Me._optinalHeader64.LoaderFlags = binReader.ReadUInt32()
                Me._optinalHeader64.NumberOfRvaAndSizes = binReader.ReadUInt32()
            End If

            ' Read the data directories.
            Me._directoryValues = New IMAGE.IMAGE_DATA_DIRECTORY_Values()
            For i As Integer = 0 To 15
                _directoryValues.Values(i).VirtualAddress = binReader.ReadUInt32()
                _directoryValues.Values(i).Size = binReader.ReadUInt32()
            Next i
        Finally

            ' Release the IO resource.
            If binReader IsNot Nothing Then
                binReader.Close()
            End If

            If fs IsNot Nothing Then
                fs.Close()
            End If

        End Try
    End Sub

    ''' <summary>
    ''' Get the full name of the .Net application.
    ''' </summary>
    ''' <returns></returns>
    Public Function GetFullDisplayName() As String
        If Not IsDotNetAssembly Then
            Return ExeFilePath
        End If

        Dim buffer(1024) As Char

        ' Get the IReferenceIdentity interface.
        Dim referenceIdentity As Fusion.IReferenceIdentity =
            TryCast(Fusion.NativeMethods.GetAssemblyIdentityFromFile(
                    ExeFilePath, Fusion.NativeMethods.ReferenceIdentityGuid), 
                Fusion.IReferenceIdentity)

        Dim IdentityAuthority As Fusion.IIdentityAuthority =
            Fusion.NativeMethods.GetIdentityAuthority()

        Dim fullName As String =
            IdentityAuthority.ReferenceToText(0, referenceIdentity)

        Return fullName
    End Function


    Public Function GetAttributes() As Dictionary(Of String, String)
        If Not IsDotNetAssembly Then
            Return Nothing
        End If

        Dim attributeDictionary = New Dictionary(Of String, String)()

        ' Get the IReferenceIdentity interface.
        Dim referenceIdentity As Fusion.IReferenceIdentity =
            TryCast(Fusion.NativeMethods.GetAssemblyIdentityFromFile(ExeFilePath, Fusion.NativeMethods.ReferenceIdentityGuid), Fusion.IReferenceIdentity)

        Dim enumAttributes = referenceIdentity.EnumAttributes()
        Dim IDENTITY_ATTRIBUTEs(1024) As Fusion.IDENTITY_ATTRIBUTE

        enumAttributes.Next(1024, IDENTITY_ATTRIBUTEs)

        For Each IDENTITY_ATTRIBUTE In IDENTITY_ATTRIBUTEs
            If Not String.IsNullOrEmpty(IDENTITY_ATTRIBUTE.Name) Then
                attributeDictionary.Add(IDENTITY_ATTRIBUTE.Name,
                                        IDENTITY_ATTRIBUTE.Value)
            End If
        Next IDENTITY_ATTRIBUTE

        Return attributeDictionary
    End Function

    ''' <summary>
    ''' Gets the assembly's original .NET Framework compilation version 
    ''' (stored in the metadata), given its file path. 
    ''' </summary>
    Public Function GetCompiledRuntimeVersion() As String
        Dim metahostInterface As Object = Nothing
        Hosting.NativeMethods.CLRCreateInstance(Hosting.NativeMethods.CLSID_CLRMetaHost,
                                                Hosting.NativeMethods.IID_ICLRMetaHost, metahostInterface)

        If metahostInterface Is Nothing OrElse Not (TypeOf metahostInterface Is Hosting.IClrMetaHost) Then
            Throw New ApplicationException("Can not get IClrMetaHost interface.")
        End If

        Dim ClrMetaHost As Hosting.IClrMetaHost = TryCast(metahostInterface, Hosting.IClrMetaHost)
        Dim buffer As New StringBuilder(1024)
        Dim bufferLength As UInteger = 1024
        ClrMetaHost.GetVersionFromFile(Me.ExeFilePath, buffer, bufferLength)
        Dim runtimeVersion As String = buffer.ToString()

        Return runtimeVersion
    End Function

End Class
