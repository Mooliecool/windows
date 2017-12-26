// Guids.cs
// MUST match guids.h
using System;

namespace Microsoft.CSVSXSaveProject
{
    static class GuidList
    {
        public const string guidCSVSXSaveProjectPkgString = "78f74c84-6262-4828-a0a3-49639de84066";
        public const string guidCSVSXSaveProjectCmdSetString = "3a929003-5d71-4994-8101-736f12c868be";

        // Create a Guid correspond with new command string.
        public const string guidCSVSXSaveProjectContextCmdSetString = "48F743BB-19DB-499A-A760-8AC746A638C9";

        public static readonly Guid guidCSVSXSaveProjectCmdSet = new Guid(guidCSVSXSaveProjectCmdSetString);

        // Add the Guid of the new command so that you can use.
        public static readonly Guid guidCSVSXSaveProjectContextCmdSet = new Guid(guidCSVSXSaveProjectContextCmdSetString);
    };
}