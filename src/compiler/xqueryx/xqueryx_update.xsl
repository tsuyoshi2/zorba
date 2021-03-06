<?xml version="1.0"?>
<xsl:stylesheet version="1.0"
    xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
    xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
    xmlns:xqxuf="http://www.w3.org/2007/xquery-update-10"
    xmlns:xqx="http://www.w3.org/2005/XQueryX">

  <!-- Note that this stylesheet frequently invokes templates for
     specified elements, even when there are no templates in the
     stylesheet whose match="" attribute identifies those elements.
     In such case, the default template's element matching template
     is invoked, which merely invokes xsl:apply-templates -->

  <xsl:output method="text"/>
  <xsl:strip-space elements="*"/>
  <xsl:preserve-space elements="xqx:value xqx:attributeValue xqx:pragmaContents
                                xqx:optionContents xqx:xquery"/>

  <xsl:variable name="DOT" select="'.'"/>
  <xsl:variable name="SPACE" select="' '"/>
  <xsl:variable name="SLASH" select="'/'"/>
  <xsl:variable name="SLASH_SLASH" select="'//'"/>
  <xsl:variable name="LESSTHAN" select="'&lt;'"/>
  <xsl:variable name="GREATERTHAN" select="'&gt;'"/>
  <xsl:variable name="LPAREN" select="'('"/>
  <xsl:variable name="RPAREN" select="')'"/>
  <xsl:variable name="NEWLINE">
    <xsl:text>
</xsl:text>
  </xsl:variable>
  <xsl:variable name="COMMA" select="','"/>
  <xsl:variable name="COMMA_SPACE" select="', '"/>
  <xsl:variable name="COMMA_NEWLINE">
    <xsl:text>,
</xsl:text>
  </xsl:variable>
  <xsl:variable name="QUOTE">
    <xsl:text>'</xsl:text>
  </xsl:variable>
  <xsl:variable name="DOUBLEQUOTE">
    <xsl:text>"</xsl:text>
  </xsl:variable>
  <xsl:variable name="TO" select="' to '"/>
  <xsl:variable name="LBRACE" select="'{'"/>
  <xsl:variable name="RBRACE" select="'}'"/>
  <xsl:variable name="LBRACKET" select="'['"/>
  <xsl:variable name="RBRACKET" select="']'"/>
  <xsl:variable name="DOLLAR" select="'$'"/>
  <xsl:variable name="MINUS" select="'-'"/>
  <xsl:variable name="PLUS" select="'+'"/>
  <xsl:variable name="EQUAL" select="'='"/>
  <xsl:variable name="COLON" select="':'"/>
  <xsl:variable name="DOUBLE_COLON" select="'::'"/>
  <xsl:variable name="SEMICOLON" select="';'"/>
  <xsl:variable name="AT" select="'@'"/>
  <xsl:variable name="STAR" select="'*'"/>
  <xsl:variable name="QUESTIONMARK" select="'?'"/>
  <xsl:variable name="ASSIGN" select="':='"/>
  <xsl:variable name="SEPARATOR" select="';'"/>
  <xsl:variable name="PRAGMA_BEGIN" select="'(# '"/>
  <xsl:variable name="PRAGMA_END" select="' #)'"/>


  <xsl:template name="delimitedList">
    <xsl:param name="delimiter" />
    <xsl:param name="leftEncloser"/>
    <xsl:param name="rightEncloser" />
    <xsl:param name="selector"/>

    <xsl:value-of select="$leftEncloser"/>
    <xsl:for-each select="*">
      <xsl:apply-templates select="."/>
      <xsl:if test="not (position()=last())">
        <xsl:value-of select="$delimiter"/>
      </xsl:if>
    </xsl:for-each>
    <xsl:value-of select="$rightEncloser"/>
  </xsl:template>


  <xsl:template name="parenthesizedList">
    <xsl:param name="delimiter" select="$COMMA_SPACE"/>
    <xsl:call-template name="delimitedList">
      <xsl:with-param name="delimiter" select="$delimiter" />
      <xsl:with-param name="leftEncloser" select="$LPAREN"/>
      <xsl:with-param name="rightEncloser" select="$RPAREN"/>
    </xsl:call-template>
  </xsl:template>


  <xsl:template name="commaSeparatedList">
    <xsl:call-template name="delimitedList">
      <xsl:with-param name="delimiter">
        <xsl:value-of select="$COMMA_SPACE"/>
      </xsl:with-param>
    </xsl:call-template>
  </xsl:template>


  <!-- To resolve Bugzilla bug #3446, we now escape CR (#xD), NEL (#x85),
       and LINE SEPARATOR (#x2028) characters in text nodes and attribute values.
         Note that this template is invoked for a number of other purposes (e.g.,
       xqx:collation, xqx:namespaceDeclaration) where the presence of such
       characters would be invalid and thus are highly unlikely to appear. 
       If they do, then this template will happily escape them, deferring the
       error until the resulting XQuery expression is processed. -->
  <xsl:template name="quote">
    <xsl:param name="item"/>
    <xsl:value-of select="$DOUBLEQUOTE"/>
    <xsl:call-template name="globalReplace">
      <xsl:with-param name="stringToBeFixed">
        <xsl:call-template name="globalReplace">
          <xsl:with-param name="stringToBeFixed">
            <xsl:call-template name="globalReplace">
              <xsl:with-param name="stringToBeFixed">
                <xsl:call-template name="globalReplace">
                  <xsl:with-param name="stringToBeFixed">
                    <xsl:call-template name="globalReplace">
                      <xsl:with-param name="stringToBeFixed">
                        <xsl:call-template name="globalReplace">
                          <xsl:with-param name="stringToBeFixed">
                            <xsl:value-of select="$item"/>
                          </xsl:with-param>
                          <xsl:with-param name="toBeReplaced">&amp;</xsl:with-param>
                          <xsl:with-param name="replacement">&amp;amp;</xsl:with-param>
                        </xsl:call-template>
                      </xsl:with-param>
                      <xsl:with-param name="toBeReplaced">&lt;</xsl:with-param>
                      <xsl:with-param name="replacement">&amp;lt;</xsl:with-param>
                    </xsl:call-template>
                  </xsl:with-param>
                  <xsl:with-param name="toBeReplaced" select="'&#x85;'"/>
                  <xsl:with-param name="replacement">&amp;#x85;</xsl:with-param>
                </xsl:call-template>
              </xsl:with-param>
              <xsl:with-param name="toBeReplaced" select="'&#xD;'"/>
              <xsl:with-param name="replacement">&amp;#xD;</xsl:with-param>
            </xsl:call-template>
          </xsl:with-param>
          <xsl:with-param name="toBeReplaced" select="'&#x2028;'"/>
          <xsl:with-param name="replacement">&amp;#x2028;</xsl:with-param>
        </xsl:call-template>
      </xsl:with-param>
      <xsl:with-param name="toBeReplaced">
        <xsl:text>"</xsl:text>
      </xsl:with-param>
      <xsl:with-param name="replacement">
        <xsl:text>""</xsl:text>
      </xsl:with-param>
    </xsl:call-template>
    <xsl:value-of select="$DOUBLEQUOTE"/>
  </xsl:template>


  <xsl:template name="globalReplace">
    <xsl:param name="stringToBeFixed"/>
    <xsl:param name="toBeReplaced"/>
    <xsl:param name="replacement"/>
    <xsl:choose>
      <xsl:when test="contains($stringToBeFixed, $toBeReplaced)">
        <xsl:value-of select="concat(substring-before($stringToBeFixed, $toBeReplaced), $replacement)"/>
        <xsl:call-template name="globalReplace">
          <xsl:with-param name="stringToBeFixed" select="substring-after($stringToBeFixed, $toBeReplaced)"/>
          <xsl:with-param name="toBeReplaced" select="$toBeReplaced"/>
          <xsl:with-param name="replacement" select="$replacement"/>
        </xsl:call-template>
      </xsl:when>
      <xsl:otherwise>
        <xsl:value-of select="$stringToBeFixed"/>
      </xsl:otherwise>
    </xsl:choose>
  </xsl:template>


  <xsl:template match="xqx:QName | xqx:pragmaName | xqx:typeName | xqx:varName |
                       xqx:functionName | xqx:optionName |
                       xqx:atomicType | xqx:tagName">
    <xsl:if test="@xqx:prefix">
      <xsl:value-of select="@xqx:prefix"/>
      <xsl:value-of select="$COLON"/>
    </xsl:if>
    <xsl:value-of select="."/>
  </xsl:template>


  <xsl:template match="xqx:attributeName">
    <xsl:choose>
      <xsl:when test="@xqx:prefix='xmlns'">
        <xsl:message terminate="yes">Incorrect XQueryX: Attribute names are not permitted to have prefix 'xmlns'; use xqx:namespaceDeclaration to declare namespaces</xsl:message>
      </xsl:when>
      <xsl:when test="@xqx:prefix">
        <xsl:value-of select="@xqx:prefix"/>
        <xsl:value-of select="$COLON"/>
        <xsl:value-of select="."/>
      </xsl:when>
      <xsl:when test=". = 'xmlns'">
        <xsl:message terminate="yes">Incorrect XQueryX: Attribute names are not permitted to be 'xmlns'; use xqx:namespaceDeclaration to declare namespaces</xsl:message>
      </xsl:when>
      <xsl:otherwise>
        <xsl:value-of select="."/>
      </xsl:otherwise>
    </xsl:choose>
  </xsl:template>


  <xsl:template match="xqx:NCName">
    <xsl:value-of select="."/>
  </xsl:template>


  <xsl:template match="xqx:rootExpr">
    <xsl:value-of select="$SLASH"/>
  </xsl:template>


  <xsl:template match="xqx:contextItemExpr">
    <xsl:value-of select="$DOT"/>
  </xsl:template>


  <xsl:template match="xqx:stringConstantExpr">
    <xsl:call-template name="quote">
      <xsl:with-param name="item" select="xqx:value"/>
    </xsl:call-template>
  </xsl:template>


  <xsl:template match="xqx:integerConstantExpr
      | xqx:decimalConstantExpr
      | xqx:doubleConstantExpr">
    <xsl:value-of select="xqx:value"/>
  </xsl:template>


  <xsl:template match="xqx:varRef">
    <xsl:value-of select="$DOLLAR"/>
    <xsl:if test="xqx:name/@xqx:prefix">
      <xsl:value-of select="xqx:name/@xqx:prefix"/>
      <xsl:value-of select="$COLON"/>
    </xsl:if>
    <xsl:value-of select="xqx:name"/>
  </xsl:template>


  <xsl:template match="xqx:pragma">
    <xsl:value-of select="$PRAGMA_BEGIN"/>
    <xsl:apply-templates select="xqx:pragmaName"/>
    <xsl:value-of select="$SPACE"/>
    <xsl:value-of select="xqx:pragmaContents"/>
    <xsl:value-of select="$PRAGMA_END"/>
  </xsl:template>


  <xsl:template match="xqx:extensionExpr">
    <xsl:apply-templates select="xqx:pragma"/>
    <xsl:value-of select="$LBRACE"/>
    <xsl:apply-templates select="xqx:argExpr"/>
    <xsl:value-of select="$RBRACE"/>
  </xsl:template>

  <!-- Response to Bugzilla bug #2528 -->
  <xsl:template match="xqx:functionCallExpr">
    <xsl:choose>
      <xsl:when test="xqx:functionName/@xqx:prefix">
        <xsl:value-of select="xqx:functionName/@xqx:prefix"/>
        <xsl:value-of select="$COLON"/>
      </xsl:when>
      <xsl:otherwise>
        <xsl:if test="xqx:functionName = 'node' or
                       xqx:functionName = 'document-node' or
                       xqx:functionName = 'element' or
                       xqx:functionName = 'attribute' or
                       xqx:functionName = 'schema-element' or
                       xqx:functionName = 'schema-attribute' or
                       xqx:functionName = 'processing-instruction' or
                       xqx:functionName = 'comment' or
                       xqx:functionName = 'text' or
                       xqx:functionName = 'item' or
                       xqx:functionName = 'if' or
                       xqx:functionName = 'typeswitch' or
                       xqx:functionName = 'empty-sequence'">
          <xsl:variable name="message">
            <xsl:text>Incorrect XQueryX: function calls must not use unqualified "reserved" name "</xsl:text>
            <xsl:value-of select="xqx:functionName"/>
            <xsl:text>"</xsl:text>
          </xsl:variable>
          <xsl:message terminate="yes">
            <xsl:value-of select="$message"/>
          </xsl:message>
        </xsl:if>
      </xsl:otherwise>
    </xsl:choose>
    <xsl:value-of select="xqx:functionName"/>
    <xsl:choose>
      <xsl:when test="xqx:arguments">
        <xsl:for-each select="xqx:arguments">
          <xsl:call-template name="parenthesizedList"/>
        </xsl:for-each>
      </xsl:when>
      <xsl:otherwise>
        <xsl:value-of select="$LPAREN"/>
        <xsl:value-of select="$RPAREN"/>
      </xsl:otherwise>
    </xsl:choose>
  </xsl:template>


  <xsl:template match="xqx:constructorFunctionExpr">
    <xsl:apply-templates select="xqx:typeName"/>
    <xsl:for-each select="xqx:argExpr">
      <xsl:call-template name="parenthesizedList"/>
    </xsl:for-each>
  </xsl:template>


  <xsl:template match="xqx:unaryMinusOp | xqx:unaryPlusOp">
    <xsl:value-of select="$LPAREN"/>
    <xsl:choose>
      <xsl:when test="self::xqx:unaryPlusOp">
        <xsl:value-of select="$PLUS"/>
      </xsl:when>
      <xsl:when test="self::xqx:unaryMinusOp">
        <xsl:value-of select="$MINUS"/>
      </xsl:when>
    </xsl:choose>
    <xsl:apply-templates select="xqx:operand"/>
    <xsl:value-of select="$RPAREN"/>
  </xsl:template>


  <xsl:template match="xqx:addOp | xqx:subtractOp | xqx:multiplyOp
                     | xqx:divOp | xqx:idivOp | xqx:modOp">
    <xsl:value-of select="$LPAREN"/>
    <xsl:apply-templates select="xqx:firstOperand"/>
    <xsl:choose>
      <xsl:when test="self::xqx:addOp">
        <xsl:value-of select="$PLUS"/>
      </xsl:when>
      <xsl:when test="self::xqx:subtractOp">
        <xsl:text> </xsl:text>
        <xsl:value-of select="$MINUS"/>
        <xsl:text> </xsl:text>
      </xsl:when>
      <xsl:when test="self::xqx:multiplyOp">
        <xsl:value-of select="$STAR"/>
      </xsl:when>
      <xsl:when test="self::xqx:divOp">
        <xsl:text> div </xsl:text>
      </xsl:when>
      <xsl:when test="self::xqx:idivOp">
        <xsl:text> idiv </xsl:text>
      </xsl:when>
      <xsl:when test="self::xqx:modOp">
        <xsl:text> mod </xsl:text>
      </xsl:when>
    </xsl:choose>
    <xsl:apply-templates select="xqx:secondOperand"/>
    <xsl:value-of select="$RPAREN"/>
  </xsl:template>


  <xsl:template match="xqx:eqOp | xqx:neOp | xqx:ltOp
                     | xqx:gtOp | xqx:leOp | xqx:geOp">
    <xsl:value-of select="$LPAREN"/>
    <xsl:apply-templates select="xqx:firstOperand"/>
    <xsl:choose>
      <xsl:when test="self::xqx:eqOp">
        <xsl:text> eq </xsl:text>
      </xsl:when>
      <xsl:when test="self::xqx:neOp">
        <xsl:text> ne </xsl:text>
      </xsl:when>
      <xsl:when test="self::xqx:ltOp">
        <xsl:text> lt </xsl:text>
      </xsl:when>
      <xsl:when test="self::xqx:gtOp">
        <xsl:text> gt </xsl:text>
      </xsl:when>
      <xsl:when test="self::xqx:leOp">
        <xsl:text> le </xsl:text>
      </xsl:when>
      <xsl:when test="self::xqx:geOp">
        <xsl:text> ge </xsl:text>
      </xsl:when>
    </xsl:choose>
    <xsl:apply-templates select="xqx:secondOperand"/>
    <xsl:value-of select="$RPAREN"/>
  </xsl:template>


  <xsl:template match="xqx:equalOp | xqx:notEqualOp | xqx:lessThanOp
                     | xqx:greaterThanOp | xqx:lessThanOrEqualOp | xqx:greaterThanOrEqualOp">
    <xsl:value-of select="$LPAREN"/>
    <xsl:apply-templates select="xqx:firstOperand"/>
    <xsl:choose>
      <xsl:when test="self::xqx:equalOp">
        <xsl:text> </xsl:text>
        <xsl:value-of select="$EQUAL"/>
        <xsl:text> </xsl:text>
      </xsl:when>
      <xsl:when test="self::xqx:notEqualOp">
        <xsl:text> !</xsl:text>
        <xsl:value-of select="$EQUAL"/>
        <xsl:text> </xsl:text>
      </xsl:when>
      <xsl:when test="self::xqx:lessThanOp">
        <xsl:text> </xsl:text>
        <xsl:value-of select="$LESSTHAN"/>
        <xsl:text> </xsl:text>
      </xsl:when>
      <xsl:when test="self::xqx:greaterThanOp">
        <xsl:text> </xsl:text>
        <xsl:value-of select="$GREATERTHAN"/>
        <xsl:text> </xsl:text>
      </xsl:when>
      <xsl:when test="self::xqx:lessThanOrEqualOp">
        <xsl:text> </xsl:text>
        <xsl:value-of select="$LESSTHAN"/>
        <xsl:value-of select="$EQUAL"/>
        <xsl:text> </xsl:text>
      </xsl:when>
      <xsl:when test="self::xqx:greaterThanOrEqualOp">
        <xsl:text> </xsl:text>
        <xsl:value-of select="$GREATERTHAN"/>
        <xsl:value-of select="$EQUAL"/>
        <xsl:text> </xsl:text>
      </xsl:when>
    </xsl:choose>
    <xsl:apply-templates select="xqx:secondOperand"/>
    <xsl:value-of select="$RPAREN"/>
  </xsl:template>


  <xsl:template match="xqx:isOp | xqx:nodeBeforeOp | xqx:nodeAfterOp">
    <xsl:value-of select="$LPAREN"/>
    <xsl:apply-templates select="xqx:firstOperand"/>
    <xsl:choose>
      <xsl:when test="self::xqx:isOp">
        <xsl:text> is </xsl:text>
      </xsl:when>
      <xsl:when test="self::xqx:nodeBeforeOp">
        <xsl:text> </xsl:text>
        <xsl:value-of select="$LESSTHAN"/>
        <xsl:value-of select="$LESSTHAN"/>
        <xsl:text> </xsl:text>
      </xsl:when>
      <xsl:when test="self::xqx:nodeAfterOp">
        <xsl:text> </xsl:text>
        <xsl:value-of select="$GREATERTHAN"/>
        <xsl:value-of select="$GREATERTHAN"/>
        <xsl:text> </xsl:text>
      </xsl:when>
    </xsl:choose>
    <xsl:apply-templates select="xqx:secondOperand"/>
    <xsl:value-of select="$RPAREN"/>
  </xsl:template>


  <xsl:template match="xqx:andOp | xqx:orOp">
    <xsl:value-of select="$LPAREN"/>
    <xsl:apply-templates select="xqx:firstOperand"/>
    <xsl:choose>
      <xsl:when test="self::xqx:andOp">
        <xsl:text> and </xsl:text>
      </xsl:when>
      <xsl:when test="self::xqx:orOp">
        <xsl:text> or </xsl:text>
      </xsl:when>
    </xsl:choose>
    <xsl:apply-templates select="xqx:secondOperand"/>
    <xsl:value-of select="$RPAREN"/>
  </xsl:template>


  <xsl:template match="xqx:unionOp | xqx:intersectOp | xqx:exceptOp">
    <xsl:value-of select="$LPAREN"/>
    <xsl:apply-templates select="xqx:firstOperand"/>
    <xsl:choose>
      <xsl:when test="self::xqx:unionOp">
        <xsl:text> union </xsl:text>
      </xsl:when>
      <xsl:when test="self::xqx:intersectOp">
        <xsl:text> intersect </xsl:text>
      </xsl:when>
      <xsl:when test="self::xqx:exceptOp">
        <xsl:text> except </xsl:text>
      </xsl:when>
    </xsl:choose>
    <xsl:apply-templates select="xqx:secondOperand"/>
    <xsl:value-of select="$RPAREN"/>
  </xsl:template>


  <xsl:template match="xqx:sequenceExpr">
    <xsl:for-each select=".">
      <xsl:call-template name="parenthesizedList">
        <xsl:with-param name="delimiter" select="$COMMA_NEWLINE"/>
      </xsl:call-template>
    </xsl:for-each>
  </xsl:template>


  <xsl:template match="xqx:rangeSequenceExpr">
    <xsl:value-of select="$LPAREN"/>
    <xsl:apply-templates select="xqx:startExpr"/>
    <xsl:value-of select="$TO"/>
    <xsl:apply-templates select="xqx:endExpr"/>
    <xsl:value-of select="$RPAREN"/>
  </xsl:template>


  <xsl:template match="xqx:forClause">
    <xsl:text> for </xsl:text>
    <xsl:call-template name="commaSeparatedList"/>
    <xsl:value-of select="$NEWLINE"/>
  </xsl:template>

  <xsl:template match="xqx:forClauseItem">
    <xsl:apply-templates/>
  </xsl:template>

  <xsl:template match="xqx:forExpr">
    <xsl:value-of select="$NEWLINE"/>
    <xsl:text>    in </xsl:text>
    <xsl:apply-templates/>
  </xsl:template>


  <xsl:template match="xqx:letClause">
    <xsl:text> let </xsl:text>
    <xsl:call-template name="commaSeparatedList"/>
    <xsl:value-of select="$NEWLINE"/>
  </xsl:template>

  <xsl:template match="xqx:letClauseItem">
    <xsl:apply-templates/>
  </xsl:template>

  <xsl:template match="xqx:letExpr">
    <xsl:value-of select="$ASSIGN"/>
    <xsl:apply-templates/>
  </xsl:template>


  <xsl:template match="xqx:returnClause">
    <xsl:text> return </xsl:text>
    <xsl:apply-templates select="*"/>
    <xsl:value-of select="$NEWLINE"/>
  </xsl:template>


  <xsl:template match="xqx:whereClause">
    <xsl:text> where </xsl:text>
    <xsl:apply-templates select="*"/>
    <xsl:value-of select="$NEWLINE"/>
  </xsl:template>


  <xsl:template match="xqx:collation">
    <xsl:text> collation </xsl:text>
    <xsl:call-template name="quote">
      <xsl:with-param name="item">
        <xsl:value-of select="."/>
      </xsl:with-param>
    </xsl:call-template>
  </xsl:template>


  <xsl:template match="xqx:emptyOrderingMode">
    <xsl:value-of select="$SPACE"/>
    <xsl:value-of select="."/>
  </xsl:template>


  <xsl:template match="xqx:orderingKind">
    <xsl:value-of select="$SPACE"/>
    <xsl:value-of select="."/>
  </xsl:template>


  <xsl:template match="xqx:orderModifier">
    <xsl:apply-templates select="*"/>
  </xsl:template>


  <xsl:template match="xqx:orderBySpec">
    <xsl:apply-templates select="xqx:orderByExpr"/>
    <xsl:value-of select="$SPACE"/>
    <xsl:apply-templates select="xqx:orderModifier"/>
  </xsl:template>


  <xsl:template match="xqx:orderByClause">
    <xsl:if test="xqx:stable">
      <xsl:text> stable</xsl:text>
    </xsl:if>
    <xsl:text> order by </xsl:text>
    <xsl:apply-templates select="xqx:orderBySpec[1]"/>
    <xsl:for-each select="xqx:orderBySpec[position() > 1]">
      <xsl:value-of select="$COMMA_SPACE"/>
      <xsl:apply-templates select="."/>
    </xsl:for-each>
    <xsl:value-of select="$NEWLINE"/>
  </xsl:template>


  <!-- Surrounding FLWOR expressions with parentheses completes the set -->
  <xsl:template match="xqx:flworExpr">
    <xsl:value-of select="$NEWLINE"/>
    <xsl:value-of select="$LPAREN"/>
    <xsl:apply-templates select="*"/>
    <xsl:value-of select="$RPAREN"/>
  </xsl:template>


  <xsl:template match="xqx:ifThenElseExpr">
    <xsl:value-of select="$LPAREN"/>
    <xsl:text> if </xsl:text>
    <xsl:value-of select="$LPAREN"/>
    <xsl:apply-templates select="xqx:ifClause"/>
    <xsl:value-of select="$RPAREN"/>
    <xsl:text> then </xsl:text>
    <xsl:apply-templates select="xqx:thenClause"/>
    <xsl:text> else </xsl:text>
    <xsl:apply-templates select="xqx:elseClause"/>
    <xsl:value-of select="$RPAREN"/>
  </xsl:template>


  <xsl:template match="xqx:positionalVariableBinding">
    <xsl:text> at </xsl:text>
    <xsl:value-of select="$DOLLAR"/>
    <xsl:if test="@xqx:prefix">
      <xsl:value-of select="@xqx:prefix"/>
      <xsl:value-of select="$COLON"/>
    </xsl:if>
    <xsl:value-of select="."/>
  </xsl:template>


  <xsl:template match="xqx:variableBinding">
    <xsl:value-of select="$DOLLAR"/>
    <xsl:if test="@xqx:prefix">
      <xsl:value-of select="@xqx:prefix"/>
      <xsl:value-of select="$COLON"/>
    </xsl:if>
    <xsl:value-of select="."/>
    <xsl:if test="parent::xqx:typeswitchExprCaseClause">
      <xsl:text> as </xsl:text>
    </xsl:if>
  </xsl:template>


  <xsl:template match="xqx:typedVariableBinding" name="typedVariableBinding">
    <xsl:value-of select="$DOLLAR"/>
    <xsl:apply-templates select="xqx:varName"/>
    <xsl:apply-templates select="xqx:typeDeclaration"/>
  </xsl:template>


  <xsl:template match="xqx:quantifiedExprInClause">
    <xsl:apply-templates select="xqx:typedVariableBinding"/>
    <xsl:text> in </xsl:text>
    <xsl:apply-templates select="xqx:sourceExpr"/>
  </xsl:template>


  <xsl:template match="xqx:quantifiedExpr">
    <xsl:value-of select="$LPAREN"/>
    <xsl:value-of select="xqx:quantifier"/>
    <xsl:value-of select="$SPACE"/>
    <xsl:apply-templates select="xqx:quantifiedExprInClause[1]"/>
    <xsl:for-each select="xqx:quantifiedExprInClause[position() > 1]">
      <xsl:value-of select="$COMMA_SPACE"/>
      <xsl:apply-templates select="."/>
    </xsl:for-each>
    <xsl:text> satisfies </xsl:text>
    <xsl:apply-templates select="xqx:predicateExpr"/>
    <xsl:value-of select="$RPAREN"/>
  </xsl:template>


  <xsl:template match="xqx:instanceOfExpr">
    <xsl:value-of select="$LPAREN"/>
    <xsl:apply-templates select="xqx:argExpr"/>
    <xsl:text> instance of </xsl:text>
    <xsl:apply-templates select="xqx:sequenceType"/>
    <xsl:value-of select="$RPAREN"/>
  </xsl:template>


  <xsl:template match="xqx:castExpr">
    <xsl:value-of select="$LPAREN"/>
    <xsl:apply-templates select="xqx:argExpr"/>
    <xsl:text> cast as </xsl:text>
    <xsl:apply-templates select="xqx:singleType"/>
    <xsl:value-of select="$RPAREN"/>
  </xsl:template>


  <xsl:template match="xqx:castableExpr">
    <xsl:value-of select="$LPAREN"/>
    <xsl:apply-templates select="xqx:argExpr"/>
    <xsl:text> castable as </xsl:text>
    <xsl:apply-templates select="xqx:singleType"/>
    <xsl:value-of select="$RPAREN"/>
  </xsl:template>


  <xsl:template match="xqx:treatExpr">
    <xsl:value-of select="$LPAREN"/>
    <xsl:apply-templates select="xqx:argExpr"/>
    <xsl:text> treat as </xsl:text>
    <xsl:apply-templates select="xqx:sequenceType"/>
    <xsl:value-of select="$RPAREN"/>
  </xsl:template>


  <xsl:template match="xqx:typeswitchExprCaseClause">
    <xsl:text> case </xsl:text>
    <xsl:apply-templates select="xqx:variableBinding"/>
    <xsl:apply-templates select="xqx:sequenceType"/>
    <xsl:text> return </xsl:text>
    <xsl:apply-templates select="xqx:resultExpr"/>
  </xsl:template>


  <xsl:template match="xqx:typeswitchExprDefaultClause">
    <xsl:text> default </xsl:text>
    <xsl:apply-templates select="xqx:variableBinding"/>
    <xsl:text> return </xsl:text>
    <xsl:apply-templates select="xqx:resultExpr"/>
  </xsl:template>


  <xsl:template match="xqx:typeswitchExpr">
    <xsl:value-of select="$LPAREN"/>
    <xsl:text>typeswitch</xsl:text>
    <xsl:value-of select="$LPAREN"/>
    <xsl:apply-templates select="xqx:argExpr"/>
    <xsl:value-of select="$RPAREN"/>
    <xsl:apply-templates select="xqx:typeswitchExprCaseClause"/>
    <xsl:apply-templates select="xqx:typeswitchExprDefaultClause"/>
    <xsl:value-of select="$RPAREN"/>
  </xsl:template>


  <xsl:template match="xqx:validateExpr">
    <xsl:value-of select="$LPAREN"/>
    <xsl:text> validate </xsl:text>
    <xsl:value-of select="xqx:validationMode"/>
    <xsl:value-of select="$LBRACE"/>
    <xsl:apply-templates select="xqx:argExpr"/>
    <xsl:value-of select="$RBRACE"/>
    <xsl:value-of select="$RPAREN"/>
  </xsl:template>


  <xsl:template match="xqx:xpathAxis">
    <xsl:value-of select="."/>
    <xsl:value-of select="$DOUBLE_COLON"/>
  </xsl:template>


  <xsl:template match="xqx:predicates">
    <xsl:for-each select="*">
      <xsl:value-of select="$LBRACKET"/>
      <xsl:apply-templates select="."/>
      <xsl:value-of select="$RBRACKET"/>
    </xsl:for-each>
  </xsl:template>


  <xsl:template match="xqx:star">
    <xsl:value-of select="$STAR"/>
  </xsl:template>


  <xsl:template match="xqx:Wildcard[*]">
    <xsl:apply-templates select="*[1]"/>
    <xsl:if test="*[2]">
      <xsl:value-of select="$COLON"/>
      <xsl:apply-templates select="*[2]"/>
    </xsl:if>
  </xsl:template>


  <xsl:template match="xqx:Wildcard[not(*)]">
    <xsl:value-of select="$STAR"/>
  </xsl:template>


  <xsl:template name="simpleWildcard" match="xqx:simpleWildcard">
    <xsl:apply-templates select="xqx:star"/>
    <xsl:apply-templates select="xqx:QName"/>
  </xsl:template>


  <xsl:template match="xqx:textTest">
    <xsl:text>text()</xsl:text>
  </xsl:template>


  <xsl:template match="xqx:commentTest">
    <xsl:text>comment()</xsl:text>
  </xsl:template>


  <xsl:template match="xqx:anyKindTest">
    <xsl:text>node()</xsl:text>
  </xsl:template>


  <xsl:template match="xqx:piTest">
    <xsl:text>processing-instruction</xsl:text>
    <xsl:value-of select="$LPAREN"/>
    <xsl:value-of select="*"/>
    <xsl:value-of select="$RPAREN"/>
  </xsl:template>


  <xsl:template match="xqx:documentTest">
    <xsl:text>document-node</xsl:text>
    <xsl:value-of select="$LPAREN"/>
    <xsl:apply-templates select="*"/>
    <xsl:value-of select="$RPAREN"/>
  </xsl:template>


  <xsl:template match="xqx:nameTest">
    <xsl:if test="@xqx:prefix">
      <xsl:value-of select="@xqx:prefix"/>
      <xsl:value-of select="$COLON"/>
    </xsl:if>
    <xsl:value-of select="."/>
  </xsl:template>


  <xsl:template match="xqx:attributeTest">
    <xsl:text>attribute</xsl:text>
    <xsl:value-of select="$LPAREN"/>
    <xsl:for-each select="xqx:attributeName">
      <xsl:call-template name="simpleWildcard"/>
    </xsl:for-each>
    <xsl:if test="xqx:typeName">
      <xsl:value-of select="$COMMA"/>
      <xsl:apply-templates select="xqx:typeName"/>
    </xsl:if>
    <xsl:value-of select="$RPAREN"/>
  </xsl:template>


  <xsl:template match="xqx:elementTest">
    <xsl:text>element</xsl:text>
    <xsl:value-of select="$LPAREN"/>
    <xsl:for-each select="xqx:elementName">
      <xsl:call-template name="simpleWildcard"/>
    </xsl:for-each>
    <xsl:if test="xqx:typeName">
      <xsl:value-of select="$COMMA"/>
      <xsl:apply-templates select="xqx:typeName"/>
    </xsl:if>
    <xsl:if test="xqx:nillable">
      <xsl:value-of select="$QUESTIONMARK"/>
    </xsl:if>
    <xsl:value-of select="$RPAREN"/>
  </xsl:template>


  <xsl:template match="xqx:schemaElementTest">
    <xsl:text>schema-element</xsl:text>
    <xsl:value-of select="$LPAREN"/>
    <xsl:if test="@xqx:prefix">
      <xsl:value-of select="@xqx:prefix"/>
      <xsl:value-of select="$COLON"/>
    </xsl:if>
    <xsl:value-of select="."/>
    <xsl:value-of select="$RPAREN"/>
  </xsl:template>


  <xsl:template match="xqx:schemaAttributeTest">
    <xsl:text>schema-attribute</xsl:text>
    <xsl:value-of select="$LPAREN"/>
    <xsl:if test="@xqx:prefix">
      <xsl:value-of select="@xqx:prefix"/>
      <xsl:value-of select="$COLON"/>
    </xsl:if>
    <xsl:value-of select="."/>
    <xsl:value-of select="$RPAREN"/>
  </xsl:template>


  <!-- rewrote test expression per Bugzilla Bug #2523 -->
  <xsl:template match="xqx:stepExpr">
    <xsl:if test="preceding-sibling::xqx:stepExpr">
      <xsl:value-of select="$SLASH"/>
    </xsl:if>
    <xsl:apply-templates select="*"/>
  </xsl:template>


  <xsl:template match="xqx:parenthesizedExpr">
    <xsl:value-of select="$LPAREN"/>
    <xsl:apply-templates/>
    <xsl:value-of select="$RPAREN"/>
  </xsl:template>


  <xsl:template match="xqx:filterExpr">
    <xsl:apply-templates/>
  </xsl:template>


  <!-- rewrote pathExpr template per Bugzilla Bug #2523 -->
  <xsl:template match="xqx:pathExpr">
    <xsl:apply-templates select="xqx:rootExpr | xqx:stepExpr"/>
  </xsl:template>


  <!-- To resolve Bugzilla bug #3446, we now escape NL (#xA) and TAB (#x9)
       characters in attribute values -->
  <xsl:template match="xqx:attributeConstructor">
    <xsl:value-of select="$SPACE"/>
    <xsl:apply-templates select="xqx:attributeName"/>
    <xsl:value-of select="$EQUAL"/>
    <xsl:choose>
      <xsl:when test="xqx:attributeValue">
        <xsl:call-template name="globalReplace">
          <xsl:with-param name="stringToBeFixed">
            <xsl:call-template name="globalReplace">
              <xsl:with-param name="stringToBeFixed">
                <xsl:call-template name="quote">
                  <xsl:with-param name="item">
                    <xsl:call-template name="globalReplace">
                      <xsl:with-param name="stringToBeFixed">
                        <xsl:call-template name="globalReplace">
                          <xsl:with-param name="stringToBeFixed">
                            <xsl:value-of select="xqx:attributeValue"/>
                          </xsl:with-param>
                          <xsl:with-param name="toBeReplaced">
                            <xsl:text>{</xsl:text>
                          </xsl:with-param>
                          <xsl:with-param name="replacement">
                            <xsl:text>{{</xsl:text>
                          </xsl:with-param>
                        </xsl:call-template>
                      </xsl:with-param>
                      <xsl:with-param name="toBeReplaced">
                        <xsl:text>}</xsl:text>
                      </xsl:with-param>
                      <xsl:with-param name="replacement">
                        <xsl:text>}}</xsl:text>
                      </xsl:with-param>
                    </xsl:call-template>
                  </xsl:with-param>
                </xsl:call-template>
              </xsl:with-param>
              <xsl:with-param name="toBeReplaced" select="'&#xA;'"/>
              <xsl:with-param name="replacement">&amp;#xA;</xsl:with-param>
            </xsl:call-template>
          </xsl:with-param>
          <xsl:with-param name="toBeReplaced" select="'&#x9;'"/>
          <xsl:with-param name="replacement">&amp;#x9;</xsl:with-param>
        </xsl:call-template>
      </xsl:when>
      <xsl:otherwise>
        <xsl:value-of select="$DOUBLEQUOTE"/>
        <xsl:for-each select="./xqx:attributeValueExpr/xqx:*">
          <xsl:value-of select="$LBRACE"/>
          <xsl:apply-templates select="."/>
          <xsl:value-of select="$RBRACE"/>
        </xsl:for-each>
        <xsl:value-of select="$DOUBLEQUOTE"/>
      </xsl:otherwise>
    </xsl:choose>
  </xsl:template>


  <xsl:template match="xqx:namespaceDeclaration">
    <xsl:text> xmlns</xsl:text>
    <xsl:if test="xqx:prefix">
      <xsl:text>:</xsl:text>
      <xsl:value-of select="xqx:prefix"/>
    </xsl:if>
    <xsl:value-of select="$EQUAL"/>
    <xsl:call-template name="quote">
      <xsl:with-param name="item" select="xqx:uri"/>
    </xsl:call-template>
  </xsl:template>

  <xsl:template match="xqx:attributeList">
    <xsl:apply-templates select="*"/>
  </xsl:template>


  <xsl:template match="xqx:elementContent">
    <xsl:for-each select="*">
      <xsl:if test="not(self::xqx:elementConstructor)">
        <xsl:value-of select="$LBRACE"/>
      </xsl:if>
      <xsl:apply-templates select="."/>
      <xsl:if test="not(self::xqx:elementConstructor)">
        <xsl:value-of select="$RBRACE"/>
      </xsl:if>
    </xsl:for-each>
  </xsl:template>


  <xsl:template match="xqx:elementConstructor">
    <xsl:value-of select="$LESSTHAN"/>
    <xsl:apply-templates select="xqx:tagName"/>
    <xsl:apply-templates select="xqx:attributeList"/>
    <xsl:value-of select="$GREATERTHAN"/>
    <xsl:apply-templates select="xqx:elementContent"/>
    <xsl:value-of select="$LESSTHAN"/>
    <xsl:value-of select="$SLASH"/>
    <xsl:apply-templates select="xqx:tagName"/>
    <xsl:value-of select="$GREATERTHAN"/>
  </xsl:template>


  <xsl:template match="xqx:tagNameExpr">
    <xsl:value-of select="$LBRACE"/>
    <xsl:apply-templates select="*"/>
    <xsl:value-of select="$RBRACE"/>
  </xsl:template>


  <xsl:template match="xqx:computedElementConstructor">
    <xsl:text> element </xsl:text>
    <xsl:apply-templates select="xqx:tagName"/>
    <xsl:apply-templates select="xqx:tagNameExpr"/>
    <xsl:value-of select="$LBRACE"/>
    <xsl:apply-templates select="xqx:contentExpr"/>
    <xsl:value-of select="$RBRACE"/>
  </xsl:template>


  <xsl:template match="xqx:computedAttributeConstructor">
    <xsl:text> attribute </xsl:text>
    <xsl:apply-templates select="xqx:tagName"/>
    <xsl:apply-templates select="xqx:tagNameExpr"/>
    <xsl:value-of select="$LBRACE"/>
    <xsl:apply-templates select="xqx:valueExpr"/>
    <xsl:value-of select="$RBRACE"/>
  </xsl:template>


  <xsl:template match="xqx:computedDocumentConstructor">
    <xsl:text> document {</xsl:text>
    <xsl:apply-templates select="*"/>
    <xsl:text> }</xsl:text>
  </xsl:template>


  <xsl:template match="xqx:computedTextConstructor">
    <xsl:text> text</xsl:text>
    <xsl:value-of select="$LBRACE"/>
    <xsl:apply-templates select="*"/>
    <xsl:value-of select="$RBRACE"/>
  </xsl:template>


  <xsl:template match="xqx:computedCommentConstructor">
    <xsl:text> comment</xsl:text>
    <xsl:value-of select="$LBRACE"/>
    <xsl:apply-templates select="*"/>
    <xsl:value-of select="$RBRACE"/>
  </xsl:template>


  <xsl:template match="xqx:piTargetExpr">
    <xsl:value-of select="$LBRACE"/>
    <xsl:apply-templates select="*"/>
    <xsl:value-of select="$RBRACE"/>
  </xsl:template>


  <!-- Move value braces into computedPIConstructor template from this template, Bugzilla bug #3442 -->
  <xsl:template match="xqx:piValueExpr">
    <xsl:apply-templates select="*"/>
  </xsl:template>


  <!-- Move value braces into this template from piValueExpr template, Bugzilla bug #3442 -->
  <xsl:template match="xqx:computedPIConstructor">
    <xsl:text> processing-instruction </xsl:text>
    <xsl:value-of select="xqx:piTarget"/>
    <xsl:apply-templates select="xqx:piTargetExpr"/>
    <xsl:value-of select="$LBRACE"/>
    <xsl:apply-templates select="xqx:piValueExpr"/>
    <xsl:value-of select="$RBRACE"/>
  </xsl:template>


  <xsl:template match="xqx:unorderedExpr">
    <xsl:text> unordered</xsl:text>
    <xsl:value-of select="$LBRACE"/>
    <xsl:text> </xsl:text>
    <xsl:apply-templates select="*"/>
    <xsl:text> </xsl:text>
    <xsl:value-of select="$RBRACE"/>
  </xsl:template>


  <xsl:template match="xqx:orderedExpr">
    <xsl:text> ordered</xsl:text>
    <xsl:value-of select="$LBRACE"/>
    <xsl:text> </xsl:text>
    <xsl:apply-templates select="*"/>
    <xsl:text> </xsl:text>
    <xsl:value-of select="$RBRACE"/>
  </xsl:template>


  <xsl:template match="xqx:versionDecl">
    <xsl:text> xquery version </xsl:text>
    <xsl:call-template name="quote">
      <xsl:with-param name="item" select="xqx:version"/>
    </xsl:call-template>
    <xsl:value-of select="$SEPARATOR"/>
    <xsl:value-of select="$NEWLINE"/>
  </xsl:template>


  <xsl:template match="xqx:namespaceDecl">
    <xsl:text>declare namespace </xsl:text>
    <xsl:value-of select="xqx:prefix"/>
    <xsl:value-of select="$EQUAL"/>
    <xsl:call-template name="quote">
      <xsl:with-param name="item" select="xqx:uri"/>
    </xsl:call-template>
  </xsl:template>


  <xsl:template match="xqx:defaultNamespaceDecl">
    <xsl:text>declare default </xsl:text>
    <xsl:value-of select="xqx:defaultNamespaceCategory"/>
    <xsl:text> namespace </xsl:text>
    <xsl:call-template name="quote">
      <xsl:with-param name="item" select="xqx:uri"/>
    </xsl:call-template>
  </xsl:template>


  <xsl:template match="xqx:defaultCollationDecl">
    <xsl:text>declare default collation </xsl:text>
    <xsl:call-template name="quote">
      <xsl:with-param name="item" select="."/>
    </xsl:call-template>
  </xsl:template>


  <xsl:template match="xqx:baseUriDecl">
    <xsl:text>declare base-uri </xsl:text>
    <xsl:call-template name="quote">
      <xsl:with-param name="item" select="."/>
    </xsl:call-template>
  </xsl:template>

  <xsl:template match="xqx:constructionDecl">
    <xsl:text>declare construction </xsl:text>
    <xsl:value-of select="."/>
  </xsl:template>


  <xsl:template match="xqx:orderingModeDecl">
    <xsl:text>declare ordering </xsl:text>
    <xsl:value-of select="."/>
  </xsl:template>


  <xsl:template match="xqx:emptyOrderingDecl">
    <xsl:text>declare default order </xsl:text>
    <xsl:value-of select="."/>
  </xsl:template>


  <xsl:template match="xqx:copyNamespacesDecl">
    <xsl:text>declare copy-namespaces </xsl:text>
    <xsl:value-of select="xqx:preserveMode"/>
    <xsl:value-of select="$COMMA"/>
    <xsl:value-of select="xqx:inheritMode"/>
  </xsl:template>


  <xsl:template match="xqx:optionDecl">
    <xsl:text>declare option </xsl:text>
    <xsl:apply-templates select="xqx:optionName"/>
    <xsl:value-of select="$SPACE"/>
    <xsl:call-template name="quote">
      <xsl:with-param name="item" select="xqx:optionContents"/>
    </xsl:call-template>
  </xsl:template>


  <xsl:template match="xqx:voidSequenceType">
    <xsl:text>empty-sequence()</xsl:text>
  </xsl:template>


  <xsl:template match="xqx:occurrenceIndicator">
    <xsl:value-of select="."/>
  </xsl:template>


  <xsl:template match="xqx:anyItemType">
    <xsl:text>item()</xsl:text>
  </xsl:template>


  <xsl:template match="xqx:sequenceType">
    <xsl:apply-templates select="*"/>
  </xsl:template>


  <xsl:template match="xqx:singleType">
    <xsl:apply-templates select="xqx:atomicType"/>
    <xsl:if test="xqx:optional">
      <xsl:text>?</xsl:text>
    </xsl:if>
  </xsl:template>


  <xsl:template match="xqx:typeDeclaration">
    <xsl:text> as </xsl:text>
    <xsl:apply-templates select="*"/>
  </xsl:template>


  <xsl:template match="xqx:varDecl">
    <xsl:text>declare variable </xsl:text>
    <xsl:value-of select="$DOLLAR"/>
    <xsl:apply-templates select="xqx:varName"/>
    <xsl:apply-templates select="xqx:typeDeclaration"/>
    <xsl:if test="xqx:external">
      <xsl:text> external </xsl:text>
    </xsl:if>
    <xsl:if test="xqx:varValue">
      <xsl:value-of select="$ASSIGN"/>
      <xsl:apply-templates select="xqx:varValue"/>
    </xsl:if>
  </xsl:template>


  <!-- Part of fix for Bugzilla bug #3520 -->
  <xsl:template match="xqx:targetLocation">
    <xsl:choose>
      <xsl:when test="position()=1"> at </xsl:when>
      <xsl:otherwise>,&#xD;  </xsl:otherwise>
    </xsl:choose>
    <xsl:call-template name="quote">
      <xsl:with-param name="item" select="."/>
    </xsl:call-template>
  </xsl:template>


  <!-- Modified to fix Bugzilla bug #3520 -->
  <xsl:template match="xqx:schemaImport">
    <xsl:text> import schema </xsl:text>
    <xsl:if test="xqx:defaultElementNamespace">
      <xsl:text> default element namespace </xsl:text>
    </xsl:if>
    <xsl:if test="xqx:namespacePrefix">
      <xsl:text> namespace </xsl:text>
      <xsl:value-of select="xqx:namespacePrefix"/>
      <xsl:value-of select="$EQUAL"/>
    </xsl:if>
    <xsl:call-template name="quote">
      <xsl:with-param name="item" select="xqx:targetNamespace"/>
    </xsl:call-template>
    <xsl:apply-templates select="xqx:targetLocation"/>
  </xsl:template>


  <!-- Modified to fix Bugzilla bug #3520 -->
  <xsl:template match="xqx:moduleImport">
    <xsl:text> import module </xsl:text>
    <xsl:if test="xqx:namespacePrefix">
      <xsl:text> namespace </xsl:text>
      <xsl:value-of select="xqx:namespacePrefix"/>
      <xsl:value-of select="$EQUAL"/>
    </xsl:if>
    <xsl:call-template name="quote">
      <xsl:with-param name="item" select="xqx:targetNamespace"/>
    </xsl:call-template>
    <xsl:apply-templates select="xqx:targetLocation"/>
  </xsl:template>


  <xsl:template match="xqx:param">
    <xsl:value-of select="$DOLLAR"/>
    <xsl:apply-templates select="xqx:varName"/>
    <xsl:apply-templates select="xqx:typeDeclaration"/>
  </xsl:template>


  <xsl:template match="xqx:paramList">
    <xsl:call-template name="parenthesizedList"/>
  </xsl:template>


  <xsl:template match="xqx:functionBody">
    <xsl:value-of select="$NEWLINE"/>
    <xsl:value-of select="$LBRACE"/>
    <xsl:value-of select="$NEWLINE"/>
    <xsl:apply-templates/>
    <xsl:value-of select="$NEWLINE"/>
    <xsl:value-of select="$RBRACE"/>
  </xsl:template>


  <xsl:template match="xqx:functionDecl">
    <xsl:text>declare function </xsl:text>
    <xsl:apply-templates select="xqx:functionName"/>
    <xsl:apply-templates select="xqx:paramList"/>
    <xsl:apply-templates select="xqx:typeDeclaration"/>
    <xsl:apply-templates select="xqx:functionBody"/>
    <xsl:if test="xqx:externalDefinition">
      <xsl:text> external </xsl:text>
    </xsl:if>
  </xsl:template>


  <xsl:template match="xqx:queryBody">
    <xsl:apply-templates select="*"/>
    <xsl:value-of select="$NEWLINE"/>
  </xsl:template>

  <xsl:template match="xqx:moduleDecl">
    <xsl:text> module namespace </xsl:text>
    <xsl:value-of select="xqx:prefix"/>
    <xsl:value-of select="$EQUAL"/>
    <xsl:call-template name="quote">
      <xsl:with-param name="item" select="xqx:uri" />
    </xsl:call-template>
    <xsl:value-of select="$SEPARATOR"/>
    <xsl:value-of select="$NEWLINE"/>
  </xsl:template>


  <xsl:template match="xqx:prolog">
    <xsl:for-each select="*">
      <xsl:apply-templates select="."/>
      <xsl:value-of select="$SEPARATOR"/>
      <xsl:value-of select="$NEWLINE"/>
    </xsl:for-each>
  </xsl:template>


  <xsl:template match="xqx:libraryModule">
    <xsl:apply-templates select="xqx:moduleDecl"/>
    <xsl:apply-templates select="xqx:prolog"/>
  </xsl:template>


  <xsl:template match="xqx:mainModule">
    <xsl:apply-templates select="xqx:prolog"/>
    <xsl:apply-templates select="xqx:queryBody"/>
  </xsl:template>


  <xsl:template match="xqx:module" priority="2">
    <xsl:apply-templates select="*"/>
  </xsl:template>


  <xsl:template match="/xqx:*">
    <xsl:message terminate="yes">Incorrect XQueryX: The only top-level element permitted is xqx:module</xsl:message>
  </xsl:template>







  <!-- XQueryX Update XQueryX Update XQueryX Update XQueryX Update XQueryX Update -->




  <!-- Initial creation                  2006-08-17: Jim Melton -->
  <!-- Added revalidationDecl            2006-08-21: Jim Melton -->
  <!-- Bring up to date with spec        2007-08-07: Jim Melton -->
  <!-- Surround updating exprs w/parens  2007-09-13: Jim Melton -->

  <!-- revalidationDecl                                         -->
  <xsl:template match="xqxuf:revalidationDecl">
    <xsl:text>declare revalidation </xsl:text>
    <xsl:apply-templates/>
  </xsl:template>


  <!-- insertExpr                                               -->
  <xsl:template match="xqxuf:insertExpr">
    <xsl:value-of select="$LPAREN"/>
    <xsl:text>insert nodes </xsl:text>
    <xsl:value-of select="$NEWLINE"/>
    <xsl:apply-templates select="xqxuf:sourceExpr"/>
    <xsl:value-of select="$NEWLINE"/>
    <xsl:apply-templates select="xqxuf:insertInto |
                               xqxuf:insertBefore |
                               xqxuf:insertAfter"/>
    <xsl:value-of select="$NEWLINE"/>
    <xsl:apply-templates select="xqxuf:targetExpr"/>
    <xsl:value-of select="$RPAREN"/>
  </xsl:template>


  <!-- sourceExpr                                               -->
  <xsl:template match="xqxuf:sourceExpr">
    <xsl:apply-templates/>
  </xsl:template>


  <!-- insertInto                                               -->
  <xsl:template match="xqxuf:insertInto">
    <xsl:if test="child::node()">
      <xsl:text>as </xsl:text>
    </xsl:if>
    <xsl:apply-templates/>
    <xsl:text>into </xsl:text>
  </xsl:template>


  <!-- insertAsFirst                                            -->
  <xsl:template match="xqxuf:insertAsFirst">
    <xsl:text>first </xsl:text>
  </xsl:template>


  <!-- insertAsLast                                             -->
  <xsl:template match="xqxuf:insertAsLast">
    <xsl:text>last </xsl:text>
  </xsl:template>


  <!-- insertAfter                                              -->
  <xsl:template match="xqxuf:insertAfter">
    <xsl:text>after </xsl:text>
  </xsl:template>


  <!-- insertBefore                                             -->
  <xsl:template match="xqxuf:insertBefore">
    <xsl:text>before </xsl:text>
  </xsl:template>


  <!-- targetExpr                                               -->
  <xsl:template match="xqxuf:targetExpr">
    <xsl:apply-templates/>
  </xsl:template>


  <!-- deleteExpr                                               -->
  <xsl:template match="xqxuf:deleteExpr">
    <xsl:value-of select="$LPAREN"/>
    <xsl:text>delete nodes </xsl:text>
    <xsl:apply-templates/>
    <xsl:value-of select="$RPAREN"/>
  </xsl:template>


  <!-- replaceExpr                                              -->
  <xsl:template match="xqxuf:replaceExpr">
    <xsl:value-of select="$LPAREN"/>
    <xsl:text>replace </xsl:text>
    <xsl:if test="xqxuf:replaceValue">
      <xsl:text>value of </xsl:text>
    </xsl:if>
    <xsl:text>node </xsl:text>
    <xsl:apply-templates select="xqxuf:targetExpr"/>
    <xsl:text> with </xsl:text>
    <xsl:apply-templates select="xqxuf:replacementExpr"/>
    <xsl:value-of select="$RPAREN"/>
  </xsl:template>


  <!-- replacementExpr                                          -->
  <xsl:template match="xqxuf:replacementExpr">
    <xsl:apply-templates/>
  </xsl:template>


  <!-- renameExpr                                               -->
  <xsl:template match="xqxuf:renameExpr">
    <xsl:value-of select="$LPAREN"/>
    <xsl:text>rename node </xsl:text>
    <xsl:apply-templates select="xqxuf:targetExpr"/>
    <xsl:text> as </xsl:text>
    <xsl:apply-templates select="xqxuf:newNameExpr"/>
    <xsl:value-of select="$RPAREN"/>
  </xsl:template>


  <!-- newNameExpr                                              -->
  <xsl:template match="xqxuf:newNameExpr">
    <xsl:apply-templates/>
  </xsl:template>


  <!-- transformExpr                                            -->
  <xsl:template match="xqxuf:transformExpr">
    <xsl:value-of select="$LPAREN"/>
    <xsl:text>copy </xsl:text>
    <xsl:apply-templates select="xqxuf:transformCopies"/>
    <xsl:value-of select="$NEWLINE"/>
    <xsl:text>  modify </xsl:text>
    <xsl:apply-templates select="xqxuf:modifyExpr"/>
    <xsl:value-of select="$NEWLINE"/>
    <xsl:text>  return </xsl:text>
    <xsl:apply-templates select="xqxuf:returnExpr"/>
    <xsl:value-of select="$RPAREN"/>
  </xsl:template>


  <!-- Part of transformExpr                                    -->
  <xsl:template match="xqxuf:transformCopies">
    <xsl:call-template name="commaSeparatedList"/>
  </xsl:template>


  <!-- Part of transformExpr                                    -->
  <xsl:template match="xqxuf:transformCopy">
    <xsl:apply-templates select="xqx:varRef"/>
    <xsl:text> := </xsl:text>
    <xsl:apply-templates select="xqxuf:copySource"/>
  </xsl:template>

  <!-- Part of transformExpr                                    -->
  <xsl:template match="xqxuf:copySource">
    <xsl:apply-templates/>
  </xsl:template>

  <!-- Part of transformExpr                                    -->
  <xsl:template match="xqxuf:modifyExpr">
    <xsl:apply-templates/>
  </xsl:template>


  <!-- Part of transformExpr                                    -->
  <xsl:template match="xqxuf:returnExpr">
    <xsl:apply-templates/>
  </xsl:template>


  <!-- Over-ride the template for functionDecl in XQueryX.xsd   -->
  <xsl:template match="xqx:functionDecl" priority="100">
    <xsl:text>declare </xsl:text>
    <xsl:if test="@xqx:updatingFunction and
                  @xqx:updatingFunction = 'true'">
      <xsl:text>updating </xsl:text>
    </xsl:if>
    <xsl:text>function </xsl:text>
    <xsl:apply-templates select="xqx:functionName"/>
    <xsl:apply-templates select="xqx:paramList"/>
    <xsl:apply-templates select="xqx:typeDeclaration"/>
    <xsl:apply-templates select="xqx:functionBody"/>
    <xsl:if test="xqx:externalDefinition">
      <xsl:text> external </xsl:text>
    </xsl:if>
  </xsl:template>

</xsl:stylesheet>
