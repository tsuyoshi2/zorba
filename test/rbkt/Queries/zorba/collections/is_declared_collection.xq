import module namespace sc = "http://www.zorba-xquery.com/module/static-context";
import datamodule namespace ns = "http://example.org/datamodule/" at "collections.xqdata";

(sc:is-declared-collection(xs:QName("ns:http")), sc:is-declared-collection(xs:QName("ns:nothing")))
