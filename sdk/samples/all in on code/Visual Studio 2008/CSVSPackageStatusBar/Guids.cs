// Guids.cs
// MUST match guids.h
using System;

namespace Company.CSVSPackageStatusBar
{
    static class GuidList
    {
        public const string guidCSVSPackageStatusBarPkgString = "b66a4cfe-5fa4-425a-a9f2-64d17909f821";
        public const string guidCSVSPackageStatusBarCmdSetString = "db10635b-1075-4a5d-a147-e51f6d26734f";
        public const string guidToolWindowPersistanceString = "174276fa-19e9-43c1-a05f-13ac5e2981c9";

        public static readonly Guid guidCSVSPackageStatusBarCmdSet = new Guid(guidCSVSPackageStatusBarCmdSetString);
    };
}