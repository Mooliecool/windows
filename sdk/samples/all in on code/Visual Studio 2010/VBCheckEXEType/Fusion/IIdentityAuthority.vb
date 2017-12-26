'************************** Module Header ******************************'
' Module Name:  IIdentityAuthority.vb
' Project:	    VBCheckEXEType
' Copyright (c) Microsoft Corporation.
' 
' Manages identity keys for code objects.
' 
' 
' This source is subject to the Microsoft Public License.
' See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
' All other rights reserved.
' 
' THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND, 
' EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED 
' WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
'************************************************************************'

Imports System.Runtime.InteropServices
Imports System.Security

Namespace Fusion

    <ComImport()>
    <InterfaceType(ComInterfaceType.InterfaceIsIUnknown)>
    <Guid("261a6983-c35d-4d0d-aa5b-7867259e77bc")>
    Friend Interface IIdentityAuthority

        <SecurityCritical()>
        Function TextToDefinition(<[In]()> ByVal Flags As UInt32,
                                  <[In](), MarshalAs(UnmanagedType.LPWStr)> ByVal Identity As String) _
                              As IDefinitionIdentity

        <SecurityCritical()>
        Function TextToReference(<[In]()> ByVal Flags As UInt32,
                                 <[In](), MarshalAs(UnmanagedType.LPWStr)> ByVal Identity As String) _
                             As IReferenceIdentity

        <SecurityCritical()>
        Function DefinitionToText(<[In]()> ByVal Flags As UInt32,
                                  <[In]()> ByVal DefinitionIdentity As IDefinitionIdentity) _
                              As <MarshalAs(UnmanagedType.LPWStr)> String

        <SecurityCritical()>
        Function DefinitionToTextBuffer(<[In]()> ByVal Flags As UInt32,
                                        <[In]()> ByVal DefinitionIdentity As IDefinitionIdentity,
                                        <[In]()> ByVal BufferSize As UInt32,
                                        <Out(), MarshalAs(UnmanagedType.LPArray)> ByVal Buffer As Char()) _
                                    As UInt32

        <SecurityCritical()>
        Function ReferenceToText(<[In]()> ByVal Flags As UInt32,
                                 <[In]()> ByVal ReferenceIdentity As IReferenceIdentity) _
                             As <MarshalAs(UnmanagedType.LPWStr)> String

        <SecurityCritical()>
        Function ReferenceToTextBuffer(<[In]()> ByVal Flags As UInt32,
                                       <[In]()> ByVal ReferenceIdentity As IReferenceIdentity,
                                       <[In]()> ByVal BufferSize As UInt32,
                                       <Out(), MarshalAs(UnmanagedType.LPArray)> ByVal Buffer As Char()) As UInt32

        <SecurityCritical()>
        Function AreDefinitionsEqual(<[In]()> ByVal Flags As UInt32,
                                     <[In]()> ByVal Definition1 As IDefinitionIdentity,
                                     <[In]()> ByVal Definition2 As IDefinitionIdentity) _
                                 As <MarshalAs(UnmanagedType.Bool)> Boolean

        <SecurityCritical()>
        Function AreReferencesEqual(<[In]()> ByVal Flags As UInt32,
                                    <[In]()> ByVal Reference1 As IReferenceIdentity,
                                    <[In]()> ByVal Reference2 As IReferenceIdentity) _
                                As <MarshalAs(UnmanagedType.Bool)> Boolean

        <SecurityCritical()>
        Function AreTextualDefinitionsEqual(<[In]()> ByVal Flags As UInt32,
                                            <[In](), MarshalAs(UnmanagedType.LPWStr)> ByVal IdentityLeft As String,
                                            <[In](), MarshalAs(UnmanagedType.LPWStr)> ByVal IdentityRight As String) _
                                        As <MarshalAs(UnmanagedType.Bool)> Boolean

        <SecurityCritical()>
        Function AreTextualReferencesEqual(<[In]()> ByVal Flags As UInt32,
                                           <[In](), MarshalAs(UnmanagedType.LPWStr)> ByVal IdentityLeft As String,
                                           <[In](), MarshalAs(UnmanagedType.LPWStr)> ByVal IdentityRight As String) _
                                       As <MarshalAs(UnmanagedType.Bool)> Boolean

        <SecurityCritical()>
        Function DoesDefinitionMatchReference(<[In]()> ByVal Flags As UInt32,
                                              <[In]()> ByVal DefinitionIdentity As IDefinitionIdentity,
                                              <[In]()> ByVal ReferenceIdentity As IReferenceIdentity) _
                                          As <MarshalAs(UnmanagedType.Bool)> Boolean

        <SecurityCritical()>
        Function DoesTextualDefinitionMatchTextualReference(<[In]()> ByVal Flags As UInt32,
                                                            <[In](), MarshalAs(UnmanagedType.LPWStr)> ByVal Definition As String,
                                                            <[In](), MarshalAs(UnmanagedType.LPWStr)> ByVal Reference As String) _
                                                        As <MarshalAs(UnmanagedType.Bool)> Boolean

        <SecurityCritical()>
        Function HashReference(<[In]()> ByVal Flags As UInt32,
                               <[In]()> ByVal ReferenceIdentity As IReferenceIdentity) As UInt64

        <SecurityCritical()>
        Function HashDefinition(<[In]()> ByVal Flags As UInt32,
                                <[In]()> ByVal DefinitionIdentity As IDefinitionIdentity) As UInt64

        <SecurityCritical()>
        Function GenerateDefinitionKey(<[In]()> ByVal Flags As UInt32,
                                       <[In]()> ByVal DefinitionIdentity As IDefinitionIdentity) _
                                   As <MarshalAs(UnmanagedType.LPWStr)> String

        <SecurityCritical()>
        Function GenerateReferenceKey(<[In]()> ByVal Flags As UInt32,
                                      <[In]()> ByVal ReferenceIdentity As IReferenceIdentity) _
                                  As <MarshalAs(UnmanagedType.LPWStr)> String

        <SecurityCritical()>
        Function CreateDefinition() As IDefinitionIdentity

        <SecurityCritical()>
        Function CreateReference() As IReferenceIdentity

    End Interface
End Namespace
