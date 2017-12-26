// Guids.cs
// MUST match guids.h
using System;

namespace AllInOne.CSVSPackageInvokeCoreEditor
{
    static class GuidList
    {
        public const string guidCSVSPackageInvokeCoreEditorPkgString = "0b1f870b-9054-48e0-b8a2-356383f878a2";
        public const string guidCSVSPackageInvokeCoreEditorCmdSetString = "c4f153af-ec4f-4001-a5d6-ca18ca13c862";

        public const string guidVBLangSvcString = "{E34ACDC0-BAAE-11D0-88BF-00A0C9110049}";
        public const string guidCSharpLangSvcString = "{694DD9B6-B865-4C5B-AD85-86356E9C88DC}";
        public const string guidXmlLangSvcString = "{f6819a78-a205-47b5-be1c-675b3c7f0b8e}";

        public static readonly Guid guidCSVSPackageInvokeCoreEditorCmdSet = new Guid(guidCSVSPackageInvokeCoreEditorCmdSetString);

        public static Guid guidVBLangSvc = new Guid(guidVBLangSvcString);
        public static Guid guidCSharpLangSvc = new Guid(guidCSharpLangSvcString);
        public static Guid guidXmlLangSvc = new Guid(guidXmlLangSvcString);
    };
}