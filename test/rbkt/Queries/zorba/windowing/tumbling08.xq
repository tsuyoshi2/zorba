declare namespace window = "foo:bar";

for tumbling window $window:w in (1 to 10)
start $s when true()
end $e when false() 
return <window>{$window:w}</window>
