using System;
using System.Collections;
using System.Collections.Specialized;
using System.Web;
using System.IO;
using System.Web.Caching;
using System.Text.RegularExpressions;
using System.Text;

namespace AspNetForums.Components {

    public class Transforms {

        // *********************************************************************
        //  TranformString
        //
        /// <summary>
        /// Method used to transform contents of string based on settings of forums
        /// </summary>
        /// 
        // ********************************************************************/
        public static string TransformString(string stringToTransform) {
            string transformedString;
            ArrayList userDefinedTransforms;

            // Load the transform table
            userDefinedTransforms = LoadUserDefinedTransforms();

            // Html Encode the contents
            stringToTransform = HttpContext.Current.Server.HtmlEncode(stringToTransform);

            // Perform user defined transforms
            transformedString = PerformUserTransforms(stringToTransform, userDefinedTransforms);

            // Peform specialized transforms
//            transformedString = PerformSpecializedTransforms(transformedString);

            // Ensure we have safe anchors
            transformedString = EnsureSafeAnchors(transformedString);

            // Look for edit notes
//            transformedString = FormatEditNotes(transformedString);

            // Return the new string
            return transformedString.Replace("\n", "\n" + Globals.HtmlNewLine + "\n");
        }


        private static string EnsureSafeAnchors(string stringToTransform) {
            MatchCollection matchs;
                        
            // Ensure we have safe anchors
            matchs = Regex.Matches(stringToTransform, "&lt;a.href=&quot;(?<url>http://((.|\\n)*?))&quot;&gt;(?<target>((.|\\n)*?))&lt;/a&gt;", RegexOptions.IgnoreCase | RegexOptions.Compiled);

            foreach (Match m in matchs) {
                stringToTransform = stringToTransform.Replace(m.ToString(), "<a target=\"_new\" href=\"" + m.Groups["url"].ToString() + "\">" + m.Groups["target"].ToString() + "</a>");
            }

            return stringToTransform;
        }


        // *********************************************************************
        //  PerformUserTransforms
        //
        /// <summary>
        /// Performs the user defined transforms
        /// </summary>
        /// 
        // ********************************************************************/
        private static string PerformUserTransforms(string stringToTransform, ArrayList userDefinedTransforms) {
            int iLoop = 0;			

            while (iLoop < userDefinedTransforms.Count) {		
		        
                // Special work for anchors
                stringToTransform = Regex.Replace(stringToTransform, userDefinedTransforms[iLoop].ToString(), userDefinedTransforms[iLoop+1].ToString(), RegexOptions.IgnoreCase | RegexOptions.Compiled | RegexOptions.Multiline);

                iLoop += 2;
            }

            return stringToTransform;
        }


        // *********************************************************************
        //  LoadUserDefinedTransforms
        //
        /// <summary>
        /// Returns a array list containing transforms that the user defined. Usually
        /// in transforms.txt
        /// </summary>
        /// 
        // ********************************************************************/
        private static ArrayList LoadUserDefinedTransforms() {
            ArrayList tranforms;
            string filenameOfTransformFile;

            // read the transformation hashtable from the cache
            tranforms = (ArrayList) HttpContext.Current.Cache.Get("transformTable");
            if (tranforms == null) {
                tranforms = new ArrayList();

                // build up the hashtable and store it in the cache
                // start by opening the text file
                filenameOfTransformFile = Globals.PhysicalPathToTransformationFile;
                if (filenameOfTransformFile.Length > 0) {
                    StreamReader sr = File.OpenText(filenameOfTransformFile);

                    // now, read through each set of lines in the text file
                    string line = sr.ReadLine(); 
                    string replaceLine = "";

                    while (line != null) {
                        line = Regex.Escape(line);
                        replaceLine = sr.ReadLine();

                        // make sure replaceLine != null
                        if (replaceLine == null) 
                            break;
					
                        line = line.Replace("<CONTENTS>", "((.|\n)*?)");
                        line = line.Replace("<WORDBOUNDARY>", "\\b");
                        line = line.Replace("<", "&lt;");
                        line = line.Replace(">", "&gt;");
                        line = line.Replace("\"", "&quot;");

                        replaceLine = replaceLine.Replace("<CONTENTS>", "$1");					
					
                        tranforms.Add(line);
                        tranforms.Add(replaceLine);

                        line = sr.ReadLine();

                    }

                    // close the streamreader
                    sr.Close();		

                    // slap the ArrayList into the cache and set its dependency to the transform file.
                    HttpContext.Current.Cache.Insert("transformTable", tranforms, new CacheDependency(filenameOfTransformFile));
                }
            }
  
            return (ArrayList) HttpContext.Current.Cache["transformTable"];
        }

        private static string FormatEditNotes(string stringToTransform) {

            Match match;
            StringBuilder editTable = null;

            match = Regex.Match(stringToTransform, ".Edit by=&quot;(?<Editor>(.|\\n)*?)&quot;.(?<Notes>(.|\\n)*?)./Edit.", RegexOptions.IgnoreCase | RegexOptions.Compiled);

            if (match.Captures.Count > 0) {

                editTable = new StringBuilder();

                editTable.Append( "<table width=\"100%\">" );
                editTable.Append( "<tr>" );
                editTable.Append( "<td>" );
                editTable.Append( match.Groups["Editor"].ToString() );
                editTable.Append( "</td>" );
                editTable.Append( "</tr>" );
                editTable.Append( "<tr>" );
                editTable.Append( "<td>" );
                editTable.Append( match.Groups["Notes"].ToString() );
                editTable.Append( "</td>" );
                editTable.Append( "</tr>" );
                editTable.Append( "</table>" );
            }
            if (editTable != null)
                return stringToTransform.Replace(match.ToString(), editTable.ToString());
            else
                return stringToTransform;
        }

        private static string PerformSpecializedTransforms(string stringToTransform) {
            StringBuilder stringToReturn = new StringBuilder();
            MatchCollection matchs;
            int currentIndexPosition = 0;

            // First we need to crack the string into segments to be transformed
            // there is only 1 special marker we care about: [code language="xxx"][/code]
            matchs = Regex.Matches(stringToTransform, "(?<content>(.|\\n)*?).code language=\"(?<lang>(.|\\n)*?)\".(?<code>(.|\\n)*?)./code.", RegexOptions.IgnoreCase | RegexOptions.Multiline | RegexOptions.Compiled);

            foreach (Match match in matchs) {
                string literalContent;
                string codeContent;

                // Literal content
                literalContent = match.Groups["content"].ToString();
                literalContent = literalContent.Replace("\n", "\n" + Globals.HtmlNewLine + "\n");
                stringToReturn.Append(literalContent);

                // Code/Markup content
                codeContent = match.Groups["code"].ToString(); //stringToTransform.Substring(match.Index, match.Length);
                stringToReturn.Append( FormatSource(codeContent, GetLanguageType(match.Groups["lang"].ToString())));

                // Reset the currentIndexPosition pointer
                currentIndexPosition = (match.Index + match.Length) + 1;
            }

            // Return the content
            if (stringToReturn.Length == 0) {
                return stringToTransform.Replace("\n", "\n" + Globals.HtmlNewLine + "\n");
            } else {

                // Pick up any other characters in the string
                if (currentIndexPosition < stringToTransform.Length) {
    
                    string remainderOfMessage = stringToTransform.Substring(currentIndexPosition);
                    remainderOfMessage = remainderOfMessage.Replace("\n", "\n" + Globals.HtmlNewLine + "\n");

                    stringToReturn.Append(remainderOfMessage);
                }

                return stringToReturn.ToString();
            }

        }

        static Language GetLanguageType(string language) {
            
            switch (language.ToUpper()) {
                case "VB" :
                    return Language.VB;
                case "JS" :
                    return Language.JScript;
                case "JScript" :
                    return Language.JScript;
                default :
                    return Language.CS;
            }
        }

        public enum Language {
            CS,
            VB,
            JScript
        }

        const int _fontsize = 3;
        const string TAG_FNTRED  = "<font color= \"red\">";
        const string TAG_FNTBLUE = "<font color= \"blue\">" ;
        const string TAG_FNTGRN  = "<font color= \"green\">" ;
        const string TAG_FNTMRN  = "<font color=\"maroon\">" ;
        const string TAG_EFONT   = "</font>" ;


        public static string FormatSource(string htmlEncodedSource, Language language) {

            StringWriter textBuffer = new StringWriter();

            textBuffer.Write("<font size=\"" + _fontsize + "\">\r\n");

            textBuffer.Write("<pre>");
            if(language == Language.CS) {
                textBuffer.Write(FixCSLine(htmlEncodedSource)) ;
            } else if(language == Language.JScript) {
                textBuffer.Write(FixJSLine(htmlEncodedSource)) ;
            } else if(language == Language.VB) {
                textBuffer.Write(FixVBLine(htmlEncodedSource)) ;
            }
            textBuffer.Write("</pre>");

            textBuffer.Write("</font>");

            return textBuffer.ToString();
        }

        static string FixCSLine(string source) {

            if (source == null)
                return null;

            source = Regex.Replace(source, "(?i)(\\t)", "    ");

            String[] keywords = new String[] {
                                                 "private", "protected", "public", "namespace", "class", "break",
                                                 "for", "if", "else", "while", "switch", "case", "using",
                                                 "return", "null", "void", "int", "bool", "string", "float",
                                                 "this", "new", "true", "false", "const", "static", "base",
                                                 "foreach", "in", "try", "catch", "finally", "get", "set", "char", "default"};

            String CombinedKeywords = "(?<keyword>" + String.Join("|", keywords) + ")";

            source = Regex.Replace(source, "\\b" + CombinedKeywords + "\\b(?<!//.*)", TAG_FNTBLUE + "${keyword}" + TAG_EFONT);
            source = Regex.Replace(source, "(?<comment>//.*$)", TAG_FNTGRN + "${comment}" + TAG_EFONT);

            return source;
        }

        static string FixJSLine(string source) {
            if (source == null)
                return null;

            source = Regex.Replace(source, "(?i)(\\t)", "    ");

            String[] keywords = new String[] {
                                                 "private", "protected", "public", "namespace", "class", "var",
                                                 "for", "if", "else", "while", "switch", "case", "using", "get",
                                                 "return", "null", "void", "int", "string", "float", "this", "set",
                                                 "new", "true", "false", "const", "static", "package", "function",
                                                 "internal", "extends", "super", "import", "default", "break", "try",
                                                 "catch", "finally" };

            String CombinedKeywords = "(?<keyword>" + String.Join("|", keywords) + ")";

            source = Regex.Replace(source, "\\b" + CombinedKeywords + "\\b(?<!//.*)", TAG_FNTBLUE + "${keyword}" + TAG_EFONT);
            source = Regex.Replace(source, "(?<comment>//.*$)", TAG_FNTGRN + "${comment}" + TAG_EFONT);

            return source;
        }

        static string FixVBLine(string source) {
            if (source == null)
                return null;

            source = Regex.Replace(source, "(?i)(\\t)", "    ");

            String[] keywords = new String[] {
                                                 "Private", "Protected", "Public", "End Namespace", "Namespace",
                                                 "End Class", "Exit", "Class", "Goto", "Try", "Catch", "End Try",
                                                 "For", "End If", "If", "Else", "ElseIf", "Next", "While", "And",
                                                 "Do", "Loop", "Dim", "As", "End Select", "Select", "Case", "Or",
                                                 "Imports", "Then", "Integer", "Long", "String", "Overloads", "True",
                                                 "Overrides", "End Property", "End Sub", "End Function", "Sub", "Me",
                                                 "Function", "End Get", "End Set", "Get", "Friend", "Inherits",
                                                 "Implements","Return", "Not", "New", "Shared", "Nothing", "Finally",
                                                 "False", "Me", "My", "MyBase" };


            String CombinedKeywords = "(?<keyword>" + String.Join("|", keywords) + ")";

            source = Regex.Replace(source, "(?i)\\b" + CombinedKeywords + "\\b(?<!'.*)", TAG_FNTBLUE + "${keyword}" + TAG_EFONT);
            source = Regex.Replace(source, "(?<comment>'(?![^']*&quot;).*$)", TAG_FNTGRN + "${comment}" + TAG_EFONT);

            return source;
        }
    }
}

/*


            // replace all carraige returns with <br> tags
            strBody = strBody.Replace("\n", "\n" + Globals.HtmlNewLine + "\n");

            // Ensure we have safe anchors
            Match m = Regex.Match(strBody, "href=\"((.|\\n)*?)\"");
            foreach(Capture capture in m.Captures) {

                if ( (!capture.ToString().StartsWith("href=\"http://")) )
                  strBody = strBody.Replace(capture.ToString(), "");

//      TODO          if ( (!capture.ToString().StartsWith("href=\"http://")) && (!capture.ToString().StartsWith("href=\"/")) )
//                    strBody = strBody.Replace(capture.ToString(), "");
            }

            // Create mailto links with any words containing @
//            strBody = Regex.Replace(strBody, "\\b((?<!<a\\s+href\\s*=\\s*\\S+)\\w+@([\\w\\-\\.,@?^=%&:/~\\+#]*[\\w\\-\\@?^=%&/~\\+#])?)", "$1", RegexOptions.IgnoreCase | RegexOptions.Multiline);

            // replace all whitespace with &nbsp;
            //strBody = strBody.Replace(" ", "&nbsp;");

            return strBody;
            */
