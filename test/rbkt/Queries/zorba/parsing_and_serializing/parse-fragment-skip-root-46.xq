import module namespace z = "http://www.zorba-xquery.com/modules/xml";
import schema namespace opt = "http://www.zorba-xquery.com/modules/xml-options";

z:parse-xml("<?xml version='1.0'?>
level0 text
<level1/>
level0 text
<level1/>
level0 text
<level1/>
level0 text
<level1/>
level0 text
<level1/>
level0 text
<level1/>
level0 text
<level1/>
level0 text
<level1/>
level0 text
<level1/>
level0 text
<level1/>
level0 text
<level1/>
level0 text
<level1/>
level0 text
<level1/>
level0 text
<level1/>
level0 text",
<opt:options>
  <opt:parseExternalParsedEntity/>
</opt:options> 
)
