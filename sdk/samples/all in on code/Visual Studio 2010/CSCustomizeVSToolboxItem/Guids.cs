// Guids.cs
// MUST match guids.h
using System;

namespace Microsoft.CSCustomizeVSToolboxItem
{
    static class GuidList
    {
        public const string guidCSCustomizeVSToolboxItemPkgString = "da61c11b-dbf4-4d35-9217-12b7163d75c9";
        public const string guidCSCustomizeVSToolboxItemCmdSetString = "2e52ac82-5adc-444c-b727-598fcfd312d8";

        public static readonly Guid guidCSCustomizeVSToolboxItemCmdSet = new Guid(guidCSCustomizeVSToolboxItemCmdSetString);
    };
}