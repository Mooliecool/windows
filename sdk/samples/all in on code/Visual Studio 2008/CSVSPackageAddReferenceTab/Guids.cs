// Guids.cs
// MUST match guids.h
using System;

namespace Microsoft.CSVSPackageAddReferenceTab
{
    static class GuidList
    {
        public const string guidReferencePageString = "252CB87A-98E8-4361-81D2-DBC70EC712D7";
        public const string guidCSVSPackageAddReferenceTabPkgString = "d7ded286-965e-430d-8015-72e721e211bc";
        public const string guidCSVSPackageAddReferenceTabCmdSetString = "e2e49589-3410-467b-a97f-112736652cde";

        public static readonly Guid guidCSVSPackageAddReferenceTabCmdSet = new Guid(guidCSVSPackageAddReferenceTabCmdSetString);
        public static readonly Guid guidReferencePage = new Guid(guidReferencePageString);
    };
}