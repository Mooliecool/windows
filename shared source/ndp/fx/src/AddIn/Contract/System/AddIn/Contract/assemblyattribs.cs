//------------------------------------------------------------------------------
/// <copyright from='2005' to='2005' company='Microsoft Corporation'>
///    Copyright (c) Microsoft Corporation. All Rights Reserved.
///    Information Contained Herein is Proprietary and Confidential.
/// </copyright>
//------------------------------------------------------------------------------

using System.Security;
using System.Security.Policy;
using System.Security.Permissions;
using System.Diagnostics.CodeAnalysis;
using System.Runtime.ConstrainedExecution;

#pragma warning disable 618
[assembly:SecurityPermission(SecurityAction.RequestMinimum, Unrestricted = true)]
#pragma warning restore 618

[assembly:SuppressMessage("Microsoft.Design", "CA2210:AssembliesShouldHaveValidStrongNames", Justification="Assembly is delay signed.")]
[assembly:SuppressMessage("Microsoft.MSInternal", "CA905:SystemAndMicrosoftNamespacesRequireApproval", Scope="namespace", Target="System.AddIn.Contract")]
[assembly:SuppressMessage("Microsoft.MSInternal", "CA905:SystemAndMicrosoftNamespacesRequireApproval", Scope="namespace", Target="System.AddIn.Contract.Collections")]
[assembly:SuppressMessage("Microsoft.MSInternal", "CA905:SystemAndMicrosoftNamespacesRequireApproval", Scope="namespace", Target="System.AddIn.Contract.Automation")]

[assembly:ReliabilityContract(Consistency.MayCorruptProcess, Cer.None)]

[assembly:System.Security.SecurityTransparent()]
[assembly:AllowPartiallyTrustedCallers()]

