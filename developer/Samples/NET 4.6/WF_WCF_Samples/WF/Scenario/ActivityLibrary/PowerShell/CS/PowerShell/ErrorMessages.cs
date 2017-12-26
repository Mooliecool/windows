//----------------------------------------------------------------
// Copyright (c) Microsoft Corporation.  All rights reserved.
//----------------------------------------------------------------

namespace Microsoft.Samples.Activities.PowerShell
{
    public static class ErrorMessages
    {
        public const string ErrorOccursWhenInvokePowerShellCommand = "Errors occurred during the execution of PowerShell command. Check the FailReason and ErrorRecords properties of PowerShellExecutionException for more details.";
        public const string PowerShellRequiresCommand = "CommandText must be set before InvokePowerShell activity '{0}' can be used.";
        public const string PowerShellNotInstalled = "Windows PowerShell 1.0 is not installed, which is required by InvokePowerShell Activity.";
    }
}
