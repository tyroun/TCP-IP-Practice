#!/bin/sh

max_connect=50
tmp_file="/tmp/$$.fifo"
mkfifo $tmp_file
#touch $tmp_file
exec 6<>$tmp_file
rm $tmp_file

for((i=0;i<max_connect;i++));do
	{
		echo "connect $i : LALALALA" | nc 10.10.5.194 1234 
	}&
done >&6
wait
#wc -l <&6
cat <&6
# close fd 6
exec 6>&-

	
