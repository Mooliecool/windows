<?xml version="1.0"?>
<xsl:transform version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform">
  <xsl:output method="xml" indent="yes"/>

  <xsl:template match="/ProcessParameters">
     <xsl:element name="xsl:stylesheet" namespace="http://www.w3.org/1999/XSL/Transform">
	     <xsl:attribute name="version">1.0</xsl:attribute>
       <xsl:apply-templates/>
     </xsl:element>
  </xsl:template>

  <xsl:template match="SystemA">
     <xsl:element name="xsl:template">
        <xsl:attribute name="match">/SomeList</xsl:attribute>
        <xsl:element name="html">
           <xsl:element name="head"/>
           <xsl:element name="body">
 	           <xsl:element name="UL">
  	          <xsl:element name="xsl:for-each">
	              <xsl:attribute name="select">item</xsl:attribute>
		            <xsl:element name="LI">
	                <xsl:element name="xsl:value-of">
	                  <xsl:attribute name="select">.</xsl:attribute>
		              </xsl:element>
		            </xsl:element>
	            </xsl:element>
	           </xsl:element>
           </xsl:element>
        </xsl:element>
     </xsl:element>
  </xsl:template>

  <xsl:template match="SystemB">
    <xsl:element name="xsl:template">
      <xsl:attribute name="match">/SomeTable</xsl:attribute>
      <xsl:element name="html">
        <xsl:element name="head"/>
        <xsl:element name="body">
	        <xsl:element name="Table">
		        <xsl:attribute name="border">1</xsl:attribute>
	          <xsl:element name="xsl:for-each">
	            <xsl:attribute name="select">row</xsl:attribute>
		          <xsl:element name="TR">
	              <xsl:element name="xsl:for-each">
	                <xsl:attribute name="select">column</xsl:attribute>
			            <xsl:element name="TD">
	            	    <xsl:element name="xsl:value-of">
	                    <xsl:attribute name="select">.</xsl:attribute>
 	                  </xsl:element>
			            </xsl:element>
			          </xsl:element>
		          </xsl:element>
	          </xsl:element>
	        </xsl:element>
	      </xsl:element>
	    </xsl:element>
    </xsl:element>
  </xsl:template>

</xsl:transform>
