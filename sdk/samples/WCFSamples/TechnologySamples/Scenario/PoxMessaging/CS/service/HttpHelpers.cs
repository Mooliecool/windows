using System;
using System.Collections.Generic;
using System.Text;

namespace Microsoft.ServiceModel.Samples
{
    class HttpHelpers
    {
        public static Dictionary<string, string> ParseQueryString(string queryString)
        {
            Dictionary<string, string> results = new Dictionary<string, string>();

            queryString = queryString.TrimStart('?');
            string[] keyValPairs = queryString.Split('&');
            foreach (string keyValPair in keyValPairs)
            {
                string[] keyAndVal = keyValPair.Split('=');
                results.Add(keyAndVal[0], keyAndVal[1]);
            }

            return results;

        }
    }
}
