<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform">

	<xsl:template match="/">
	<root>
		<xsl:apply-templates/>
	</root>
	</xsl:template>

	<xsl:template match="bookstore">
	<!-- Prices and books -->
		<bookstore>
			<xsl:apply-templates select="book"/>
		</bookstore>
	</xsl:template>

	<xsl:template match="book">
		<book>
			<xsl:attribute name="ISBN">
				<xsl:value-of select="@ISBN"/>
			</xsl:attribute>
			<price><xsl:value-of select="price"/></price><xsl:text>
			</xsl:text>
		</book>
	</xsl:template>

</xsl:stylesheet>
