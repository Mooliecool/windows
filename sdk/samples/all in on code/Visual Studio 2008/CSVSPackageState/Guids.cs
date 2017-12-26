// Guids.cs
// MUST match guids.h
using System;

namespace AllInOne.CSVSPackageState
{
    static class GuidList
    {
        public const string guidCSVSPackageStatePkgString = "5130c574-1b8d-43b2-9955-823f4515e75c";
        public const string guidCSVSPackageStateCmdSetString = "77e2c0f7-2eec-467a-aa95-866fb4a97b07";
        public const string guidToolWindowPersistanceString = "0e2dd62e-a87a-44d3-9b70-c3b5de51338c";

        public static readonly Guid guidCSVSPackageStateCmdSet = new Guid(guidCSVSPackageStateCmdSetString);
    };
}