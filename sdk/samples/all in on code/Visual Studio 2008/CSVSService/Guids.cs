// Guids.cs
// MUST match guids.h
using System;

namespace VSX.CSVSService
{
    static class GuidList
    {
        public const string guidCSVSServiceProviderPackageString        = "a8ac9a14-aabe-41ba-a063-4ca22539f45d";
        public const string guidCSVSServiceConsumerPackageString        = "f3c7a11b-cc28-4ff9-a9f5-2d50f844d65a";
        public const string guidCSVSServiceGlobalServiceInterfaceString = "39f27a3b-3a61-4cc7-b08b-4cc6fdf6bcd4";
        public const string guidCSVSServiceLocalServiceInterfaceString  = "db69f85a-5156-4374-b142-f4f4f9e3d4f3";
        public const string guidCSVSServiceLocalServiceString           = "cf7e7d78-de38-4eff-a93e-60ad2b6aad2e";
        public const string guidCSVSServiceGlobalServiceString          = "81099ee5-d61c-4cd7-bf44-cb5c48667b49";
        public const string guidCSVSServiceCmdSetString                 = "65ed95ea-e66d-469c-b713-24729ac8568f";

        public static readonly Guid guidCSVSServiceCmdSet               = new Guid(guidCSVSServiceCmdSetString);
    };
}