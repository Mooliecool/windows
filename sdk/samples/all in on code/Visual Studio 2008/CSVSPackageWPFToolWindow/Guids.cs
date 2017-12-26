// Guids.cs
// MUST match guids.h
using System;

namespace Company.VSPackageWPFToolWindow
{
    static class GuidList
    {
        public const string guidVSPackageWPFToolWindowPkgString = "31921cf1-2895-4725-aec7-a9c6f994018b";
        public const string guidVSPackageWPFToolWindowCmdSetString = "43ef812a-a984-4b42-b80a-98151b688722";
        public const string guidToolWindowPersistanceString = "1c0ed058-d9bf-44f0-97fa-18ae474a30f5";

        public static readonly Guid guidVSPackageWPFToolWindowCmdSet = new Guid(guidVSPackageWPFToolWindowCmdSetString);
    };
}