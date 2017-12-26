//  Copyright (c) Microsoft Corporation.  All Rights Reserved.

using System.ServiceModel.Channels;

namespace Microsoft.Ajax.Samples
{
    public class JsonContentTypeMapper : WebContentTypeMapper
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
