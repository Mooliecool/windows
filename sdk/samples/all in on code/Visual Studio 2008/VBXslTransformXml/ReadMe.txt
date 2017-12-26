========================================================================
    CONSOLE APPLICATION : VBXslTransformXml Project Overview
========================================================================

/////////////////////////////////////////////////////////////////////////////
General:

This sample project shows how to use XslCompiledTransform to transform an XML 
data file to .csv file using an XSLT style sheet.


/////////////////////////////////////////////////////////////////////////////
Code Logic:

1. Initialize an XslCompiledTransform instance

2. Call transform.Load() to load the XSLT style sheet Books.xslt

3. Call transform.Transform() to transform the source XML file to a csv file
using the XSLT style sheet.


/////////////////////////////////////////////////////////////////////////////
XSLT usage:

1.    <xsl:text>&#34;</xsl:text>
      <xsl:value-of select ="@id"/>
      <xsl:text>&#34;</xsl:text>
&#34 means the quote mark. Use quote mark above each value to avoid internal
comma's effect.Use @ to select the attribute instead of child node.

2.    <xsl:for-each select ="*">
		<xsl:text>&#34;</xsl:text>
		<xsl:value-of select ="."/>
		<xsl:text>&#34;</xsl:text>
		<xsl:if test ="position() != last()">
			<xsl:value-of select ="','"/>
		</xsl:if>
      </xsl:for-each>
Use xsl:for-each to loop all child nodes. <xsl:value-of select="."> means to 
select the node's content.

3.    <xsl:text>&#xD;&#xa;</xsl:text>
&#xD;&#xa; refers to the new line(\r\n). At end of row, we should start
a new line with a new book.


/////////////////////////////////////////////////////////////////////////////
References:

XslCompiledTransform document,
http://msdn.microsoft.com/en-us/library/system.xml.xsl.xslcompiledtransform.aspx

XSLT Reference:
http://msdn.microsoft.com/en-us/library/ms256069.aspx


/////////////////////////////////////////////////////////////////////////////
