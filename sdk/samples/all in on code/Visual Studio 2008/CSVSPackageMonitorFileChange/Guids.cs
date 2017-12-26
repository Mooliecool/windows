// Guids.cs
// MUST match guids.h
using System;

namespace CSVSPackageMonitorFileChange
{
    static class GuidList
    {
        public const string guidCSVSMonitorFileChangePkgString = "a34f57d1-6c8e-42e5-9059-ce4e985e36ba";
        public const string guidCSVSMonitorFileChangeCmdSetString = "7eb50ee7-0a1d-49dd-8315-1dc9487ea814";

        public static readonly Guid guidCSVSMonitorFileChangeCmdSet = new Guid(guidCSVSMonitorFileChangeCmdSetString);
    };
}