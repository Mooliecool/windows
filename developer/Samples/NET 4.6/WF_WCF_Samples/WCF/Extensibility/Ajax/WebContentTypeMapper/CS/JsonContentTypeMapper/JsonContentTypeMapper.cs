//----------------------------------------------------------------
// Copyright (c) Microsoft Corporation.  All rights reserved.
//----------------------------------------------------------------

using System.ServiceModel.Channels;

namespace Microsoft.Samples.WebContentTypeMapper
{
    public class JsonContentTypeMapper : System.ServiceModel.Channels.WebContentTypeMapper
    {
        public override WebContentFormat GetMessageFormatForContentType(string contentType)
        {
            if (contentType == "text/javascript")
            {
                return WebContentFormat.Json;
            }
            else
            {
                return WebContentFormat.Default;
            }
        }
    }
}
