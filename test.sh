#!/bin/sh
function BigData_test(){
	data_num=$1
	tmp_file="/tmp/$$.fifo"
	touch $tmp_file
   	for((i=0;i<data_num;i++)); do
		echo -n "$i" >> $tmp_file
	done
	echo "" >> $tmp_file	
#	cat $tmp_file	
	nc $IP_Address 1234 < $tmp_file
	rm $tmp_file  
}

function multi_connect_test(){
	max_connect=$1
	tmp_file="/tmp/$$.fifo"
	#mkfifo $tmp_file
	touch $tmp_file
	#exec 6<>$tmp_file
	
	for((i=0;i<max_connect;i++));do
		{
			echo "connect $i : LALALALALALALALAALALALA" | nc $IP_Address 1234 
		}&
	done > $tmp_file
	wait
	cat $tmp_file 
	echo "totoal echo ="&&wc -l $tmp_file
	
	#close fd 6
	rm $tmp_file
	#exec 6>&-
}

IP_Address=$3
echo $IP_Address
	
if [ $1 = "-B" ]; then
	BigData_test $2
elif [ $1 = "-M" ]; then
	multi_connect_test $2
else
	echo "parameter is not correct ./test.sh [-B][-H] number IP"
	echo "-B : send how many data to R/W"
	echo "-M : make how many connection to server"
fi

