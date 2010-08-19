import module namespace xqd = "http://www.zorba-xquery.com/modules/xqdoc";
import module namespace file = "http://www.zorba-xquery.com/modules/file";
import schema namespace xqds = "http://www.xqdoc.org/1.0" at "xqdoc-1.0.xsd";

declare sequential function local:remove-date($xqdoc)
{
  let $date := $xqdoc//xqds:date[1]
  return
	replace value of node $date with "";
  exit returning $xqdoc;
};

let $arg := fn:base-uri(<a/>)
let $delim := "/"
let $content := file:read-text(fn:concat(
    if (matches($arg, replace($delim,
                concat('^(.*)', replace($delim, '(\.|\[|\]|\\|\||\-|\^|\$|\?|\*|\+|\{|\}|\(|\))','\\$1'),'.*'),
                '$1')))
       then replace($arg,
                concat('^(.*)', replace($delim, '(\.|\[|\]|\\|\||\-|\^|\$|\?|\*|\+|\{|\}|\(|\))','\\$1'),'.*'),
                '$1')
       else '', "/gdata.xqlib"))
let $xqdoc as schema-element(xqds:xqdoc) := 
  validate lax {
    xqd:xqdoc-content($content)
  }
return
  local:remove-date($xqdoc)

