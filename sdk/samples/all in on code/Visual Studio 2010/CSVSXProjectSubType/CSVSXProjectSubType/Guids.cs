// Guids.cs
// MUST match guids.h
using System;

namespace CSVSXProjectSubType
{
    static class GuidList
    {
        public const string guidCSVSXProjectSubTypePkgString = "72f0306a-8c12-4ee5-94ce-b76d693786cc";
        public const string guidCSVSXProjectSubTypeCmdSetString = "8840751c-5810-4024-b628-5fdfe5f32149";

        public static readonly Guid guidCSVSXProjectSubTypeCmdSet = new Guid(guidCSVSXProjectSubTypeCmdSetString);
    };
}