//------------------------------------------------------------------------------
// <copyright file="NewLineHandling.cs" company="Microsoft">
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

namespace System.Xml
{
    // NewLineHandling speficies what will XmlWriter do with new line characters. The options are:
    //  Replace  = Replaces all new line characters with XmlWriterSettings.NewLineChars so all new lines are the same; by default NewLineChars are "\r\n"
    //  Entitize = Replaces all new line characters that would be normalized away by a normalizing XmlReader with character entities
    //  None     = Does not change the new line characters in input
    //
    // Following table shows what will happen with new line characters in detail:
    //
    //										|      In text node value       |    In attribute value               |
    // input to XmlWriter.WriteString()		| \r\n		\n		\r		\t	|	\r\n		\n		\r		\t    |
    // ------------------------------------------------------------------------------------------------------------
    // NewLineHandling.Replace (default)	| \r\n		\r\n	\r\n	\t	|	&#D;&#A;	&#A;	&#D;	&#9;  |
    // NewLineHandling.Entitize			    | &#D;		\n		&#D;	\t	|	&#D;&#A;	&#A;	&#D;	&#9;  |
    // NewLineHandling.None				    | \r\n		\n		\r		\t	|	\r\n		\n		\r		\t    |
    // ------------------------------------------------------------------------------------------------------------

    // Specifies how end of line is handled in XmlWriter.
    public enum NewLineHandling {
        Replace = 0,
        Entitize = 1,
        None = 2
    }
}
