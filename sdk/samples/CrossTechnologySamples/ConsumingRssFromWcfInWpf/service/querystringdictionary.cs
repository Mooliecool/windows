using System;
using System.Collections.Specialized;
using System.Net.Mime;
using System.ServiceModel.Channels;
using System.Text;
using System.Web;

namespace Microsoft.ServiceModel.Samples
{ 
    public class QueryStringDictionary : NameValueCollection
    {
        public QueryStringDictionary( HttpRequestMessageProperty requestProperty )
        {
            string query = requestProperty.QueryString;
            string contentType = requestProperty.Headers["Content-Type"];
            Encoding encoding = Encoding.UTF8;

            if( !String.IsNullOrEmpty(contentType) )
            {      
                ContentType ct = new ContentType(contentType);
                string charSet = ct.CharSet;

                try
                {
                    if( charSet != null )
                        encoding = Encoding.GetEncoding(ct.CharSet);
                }
                catch { }
            }

            FillFromString(query, true, encoding);
        }

        public static string Name
        {
            get { return "QueryStringDictionary"; }
        }

        private void FillFromString( string s, bool urlencoded, Encoding encoding )
        {
            int length = (s != null) ? s.Length : 0;
            for( int i = 0; i < length; i++ )
            {
                int tokenStart = i;
                int tokenEnd = -1;

                while( i < length )
                {
                    char c = s[i];
                    if( c == '=' )
                    {
                        if( tokenEnd < 0 )
                        {
                            tokenEnd = i;
                        }
                    }
                    else if( c == '&' )
                    {
                        break;
                    }
                    i++;
                }

                string name = null;
                string value = null;
                
                if( tokenEnd >= 0 )
                {
                    name = s.Substring(tokenStart, tokenEnd - tokenStart);
                    value = s.Substring(tokenEnd + 1, (i - tokenEnd) - 1);
                }
                else
                {
                    value = s.Substring(tokenStart, i - tokenStart);
                }
                if( urlencoded )
                {
                    base.Add(HttpUtility.UrlDecode(name, encoding).ToLowerInvariant(), HttpUtility.UrlDecode(value, encoding));
                }
                else
                {
                    base.Add(name, value);
                }
                if( (i == (length - 1)) && (s[i] == '&') )
                {
                    base.Add(null, string.Empty);
                }
            }
        }


    }
}
