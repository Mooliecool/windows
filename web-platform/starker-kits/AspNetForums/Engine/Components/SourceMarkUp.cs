using System;
using System.Web.Mail;
using System.ComponentModel;
using System.Collections;
using System.Collections.Specialized;
using System.Text.RegularExpressions;
using System.IO;

namespace AspNetForums.Components {

    public class SourceMarkUp {

/*
        string GetLangFromLine(string source, string defLang) {
            if ( source == null ) {
                return defLang;
            }

            Match langMatch = Regex.Match(source, "(?i)<%@\\s*Page\\s*.*Language\\s*=\\s*\"(?<lang>[^\"]+)\"");
            if ( langMatch.Success ) {
                return langMatch.Groups["lang"].ToString();
            }

            langMatch = Regex.Match(source, "(?i)(?=.*runat\\s*=\\s*\"?server\"?)<script.*language\\s*=\\s*\"(?<lang>[^\"]+)\".*>");
            if ( langMatch.Success ) {
                return langMatch.Groups["lang"].ToString();
            }

            langMatch = Regex.Match(source, "(?i)<%@\\s*WebService\\s*.*Language\\s*=\\s*\"?(?<lang>[^\"]+)\"?");
            if ( langMatch.Success ) {
                return langMatch.Groups["lang"].ToString();
            }

            return defLang;
        }
*/

/*
        string FixAspxLine(string source ) {
            string searchExpr;      // search string
            string replaceExpr;     // replace string

            if ((source == null) || (source.Length == 0))
                return source;

            // Search for \t and replace it with 4 spaces.
            source = Regex.Replace(source, "(?i)(\\t)", "    ");
            source = HttpUtility.HtmlEncode(source);


            // Single line comment or #include references.
            searchExpr = "(?i)(?<a>(^.*))(?<b>(&lt;!--))(?<c>(.*))(?<d>(--&gt;))(?<e>(.*))";
            replaceExpr = "${a}" + TAG_FNTGRN + "${b}${c}${d}" + TAG_EFONT + "${e}";

            if (Regex.IsMatch(source, searchExpr))
                return Regex.Replace(source, searchExpr, replaceExpr);

            // Colorize <%@ <type>
            searchExpr = "(?i)" + "(?<a>(&lt;%@))" + "(?<b>(.*))" + "(?<c>(%&gt;))";
            replaceExpr = "<font color=blue><b>${a}${b}${c}</b></font>";

            if (Regex.IsMatch(source, searchExpr))
                source = Regex.Replace(source, searchExpr, replaceExpr);

            // Colorize <%# <type>
            searchExpr = "(?i)" + "(?<a>(&lt;%#))" + "(?<b>(.*))" + "(?<c>(%&gt;))";
            replaceExpr = "${a}" + "<font color=red><b>" + "${b}" + "</b></font>" + "${c}";

            if (Regex.IsMatch(source, searchExpr))
                source = Regex.Replace(source, searchExpr, replaceExpr);

            // Colorize tag <type>
            searchExpr = "(?i)" + "(?<a>(&lt;)(?!%)(?!/?asp:)(?!/?template)(?!/?property)(?!/?ibuyspy:)(/|!)?)" + "(?<b>[^;\\s&]+)" + "(?<c>(\\s|&gt;|\\Z))";
            replaceExpr = "${a}" + TAG_FNTMRN + "${b}" + TAG_EFONT + "${c}";

            if (Regex.IsMatch(source, searchExpr))
                source = Regex.Replace(source, searchExpr, replaceExpr);

            // Colorize asp:|template for runat=server tags <type>
            searchExpr = "(?i)(?<a>&lt;/?)(?<b>(asp:|template|property|IBuySpy:).*)(?<c>&gt;)?";
            replaceExpr = "${a}<font color=blue><b>${b}</b></font>${c}";

            if (Regex.IsMatch(source, searchExpr))
                source = Regex.Replace(source, searchExpr, replaceExpr);

            //colorize begin of tag char(s) "<","</","<%"
            searchExpr = "(?i)(?<a>(&lt;)(/|!|%)?)";
            replaceExpr = TAG_FNTBLUE + "${a}" + TAG_EFONT;

            if (Regex.IsMatch(source, searchExpr))
                source = Regex.Replace(source, searchExpr, replaceExpr);

            // Colorize end of tag char(s) ">","/>"
            searchExpr = "(?i)(?<a>(/|%)?(&gt;))";
            replaceExpr = TAG_FNTBLUE + "${a}" + TAG_EFONT;

            if (Regex.IsMatch(source, searchExpr))
                source = Regex.Replace(source, searchExpr, replaceExpr);

            return source;
        }

        bool IsScriptBlockTagStart(String source) {
            if (Regex.IsMatch(source, "<script.*runat=\"?server\"?.*>")) {
                return true;
            }
            if (Regex.IsMatch(source, "(?i)<%@\\s*WebService")) {
                return true;
            }
            return false;
        }

        bool IsScriptBlockTagEnd(String source) {
            if (Regex.IsMatch(source, "</script.*>")) {
                return true;
            }
            return false;
        }

        bool IsMultiLineTagStart(String source) {
            String searchExpr = "(?i)(?!.*&gt;)(?<a>&lt;/?)(?<b>(asp:|template|property|IBuySpy:).*)";

            source = HttpUtility.HtmlEncode(source);
            if ( Regex.IsMatch(source, searchExpr) ) {
                return true;
            }
            return false;
        }

        bool IsMultiLineTagEnd(String source) {
            String searchExpr = "(?i)&gt;";

            source = HttpUtility.HtmlEncode(source);
            if ( Regex.IsMatch(source, searchExpr) ) {
                return true;
            }
            return false;
        }
*/
    }
}