define wc_print
echo "
set $c = (unsigned short *)$arg0
while ( *$c )
  printf "%c", *$c
  set $c++
end
echo "\n
end
