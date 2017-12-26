//------------------------------------------------------------------------------
// <copyright file="XmlComplianceUtil.cs" company="Microsoft">
//     
//      Copyright (c) 2006 Microsoft Corporation.  All rights reserved.
//     
//      The use and distribution terms for this software are contained in the file
//      named license.txt, which can be found in the root of this distribution.
//      By using this software in any fashion, you are agreeing to be bound by the
//      terms of this license.
//     
//      You must not remove this notice, or any other, from this software.
//     
// </copyright>
//------------------------------------------------------------------------------

using System;
using System.IO;

namespace System.Xml {
    using System.Text;

    internal static class XmlComplianceUtil {

        // Replaces \r\n, \n, \r and \t with single space (0x20) and then removes spaces
        // at the beggining and and the end of the string and replaces sequences of spaces
        // with a single space.
        public static string NonCDataNormalize( string value ) {
            int len = value.Length;
            if ( len <= 0 ) {
               return string.Empty;
            }

            int startPos = 0;
            StringBuilder norValue = null;
            XmlCharType xmlCharType = XmlCharType.Instance;
            while ( xmlCharType.IsWhiteSpace( value[startPos] ) ) {
                startPos++;
                if ( startPos == len ) {
                    return " ";
                }
            }

            int i = startPos;
            while ( i < len ) {
                if ( !xmlCharType.IsWhiteSpace( value[i] ) ) {
                    i++;
                    continue;
                }

                int j = i + 1;
                while ( j < len && xmlCharType.IsWhiteSpace( value[j] ) ) {
                    j++;
                }
                if ( j == len ) {
                    if ( norValue == null ) {
                        return value.Substring( startPos, i - startPos );
                    }
                    else {
                        norValue.Append( value, startPos, i - startPos );
                        return norValue.ToString();
                    }
                }
                if (j > i + 1 || value[i] != 0x20 ) {
                    if ( norValue == null ) {
                        norValue = new StringBuilder( len );
                    }
                    norValue.Append( value, startPos, i - startPos );
                    norValue.Append( (char)0x20 );
                    startPos = j;
                    i = j;
                }
                else {
                    i++;
                }
            }

            if ( norValue != null ) {
                if ( startPos < i ) {
                    norValue.Append( value, startPos, i - startPos );
                }
                return norValue.ToString();
            }
            else {
                if ( startPos > 0 ) {
                    return value.Substring( startPos, len - startPos );
                }
                else {
                    return value;
                }
            }
        }

        // Replaces \r\n, \n, \r and \t with single space (0x20) 
        public static string CDataNormalize( string value ) {
            int len = value.Length;

            if ( len <= 0 ) {
               return string.Empty;
            }

            int i = 0;
            int startPos = 0;
            StringBuilder norValue = null;

            while ( i < len ) {
                char ch = value[i];
                if ( ch >= 0x20 || ( ch != 0x9 && ch != 0xA && ch != 0xD ) ) {
                    i++;
                    continue;
                }

                if ( norValue == null ) {
                    norValue = new StringBuilder( len );
                }
                if ( startPos < i ) {
                    norValue.Append( value, startPos, i - startPos );
                }
                norValue.Append( (char)0x20 );

                if ( ch == 0xD && ( i+1 < len && value[i+1] == 0xA ) ) {
                    i += 2;
                }
                else {
                    i++;
                }
                startPos = i;
            }

            if ( norValue == null ) {
                return value;
            }
            else {
               if ( i > startPos ) {
                   norValue.Append(value, startPos, i-startPos);
               }
               return norValue.ToString();
            }
        }

        // StripSpaces removes spaces at the beginning and at the end of the value and replaces sequences of spaces with a single space
        public static string StripSpaces( string value ) {
            int len = value.Length;
            if ( len <= 0 ) {
               return string.Empty;
            }

            int startPos = 0;
            StringBuilder norValue = null;

            while ( value[startPos] == 0x20 ) {
                startPos++;
                if ( startPos == len ) {
                    return " ";
                }
            }

            int i;
            for ( i = startPos; i < len; i++ ) {
                if ( value[i] == 0x20 ) {
                    int j = i + 1;
                    while ( j < len && value[j] == 0x20 ) {
                        j++;
                    }
                    if ( j == len ) {
                        if ( norValue == null ) {
                            return value.Substring( startPos, i - startPos );
                        }
                        else {
                            norValue.Append( value, startPos, i - startPos );
                            return norValue.ToString();
                        }
                   }
                   if ( j > i + 1 ) {
                        if ( norValue == null ) {
                            norValue = new StringBuilder( len );
                        }
                        norValue.Append( value, startPos, i - startPos + 1 );
                        startPos = j;
                        i = j - 1;
                    }
                }
            }
            if ( norValue == null ) {
                return ( startPos == 0 ) ? value : value.Substring( startPos, len - startPos );
            }
            else {
                if ( i > startPos ) {
                   norValue.Append( value, startPos, i - startPos );
               }
                return norValue.ToString();
            }
        }

        // StripSpaces removes spaces at the beginning and at the end of the value and replaces sequences of spaces with a single space
        public static void StripSpaces( char[] value, int index, ref int len ) {
            if ( len <= 0 ) {
               return;
            }

            int startPos = index;
            int endPos = index + len;

            while ( value[startPos] == 0x20 ) {
                startPos++;
                if ( startPos == endPos ) {
                    len = 1;
                    return;
                }
            }

            int offset = startPos - index;
            int i;
            for ( i = startPos; i < endPos; i++ ) {
                char ch;
                if ( ( ch = value[i] ) == 0x20 ) {
                    int j = i + 1;
                    while ( j < endPos && value[j] == 0x20 ) {
                        j++;
                    }
                    if ( j == endPos ) {
                        offset += ( j - i );
                        break;
                    }
                    if ( j > i+1 ) {
                        offset += ( j - i - 1 );
                        i = j - 1;
                    }
                }
                value[i-offset] = ch;
            }
            len -= offset;
        }

        public static bool IsValidLanguageID( char[] value, int startPos, int length ) {
            int len = length;
            if ( len < 2 ) {
                return false;
            }

            bool fSeenLetter = false;
            int i = startPos;
            XmlCharType xmlCharType = XmlCharType.Instance;

            char ch = value[i];
            if ( xmlCharType.IsLetter(ch) ) {
                if ( xmlCharType.IsLetter( value[++i] ) ) {
                    if ( len == 2 ) {
                        return true;
                    }
                    len--;
                    i++;
                }
                else if ( ('I' != ch && 'i' != ch ) && ( 'X' != ch && 'x' != ch ) ) {  //IANA or custom Code
                    return false;
                }

                if ( value[i] != '-' ) {
                    return false;
                }

                len -= 2;
                while ( len-- > 0 ) {
                    ch = value[++i];
                    if ( xmlCharType.IsLetter( ch ) ) {
                        fSeenLetter = true;
                    }
                    else if ( ch == '-' && fSeenLetter ) {
                        fSeenLetter = false;
                    }
                    else {
                        return false;
                    }
                }
                if ( fSeenLetter ) {
                    return true;
                }
            }
            return false;
        }
    }
}
