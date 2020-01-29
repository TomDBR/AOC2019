#!/bin/bash
## ====== USAGE ======
## ./2.sh 2>&1 | sort -n | tail -n1
## pipe stderr to stdout so we can sort it
## if running script takes more than a few seconds it stalled, kill it and try again
trap 'pkill intcode.bin' TERM INT QUIT
mkfifo pipe{1,2,3,4,5}

# PART 2 LOOP
printf '%s\n' {5,6,7,8,9}{5,6,7,8,9}{5,6,7,8,9}{5,6,7,8,9}{5,6,7,8,9} | grep -Ev '(.).*\1' | while read -r combo
do
	opt1="${combo:0:1}"
	opt2="${combo:1:1}"
	opt3="${combo:2:1}"
	opt4="${combo:3:1}"
	opt5="${combo:4:1}"
	# to see the output of each engine, change the -1 to a positive integer, eg: 1 to 5 since we have 5 engines
	./intcode.bin -1 0 2>/dev/null < pipe1 > pipe2 &
	./intcode.bin -1 0 2>/dev/null < pipe2 > pipe3 &
	./intcode.bin -1 0 2>/dev/null < pipe3 > pipe4 &
	./intcode.bin -1 0 2>/dev/null < pipe4 > pipe5 &
	# this tee thing doesn't work for some reason so we'll just read from stderr
	# ./intcode.bin -1 0 < pipe5 | unbuffer -p tee -ia /tmp/day07part2 >pipe1 &
	./intcode.bin -1 0 < pipe5 >pipe1 &
	# this doesn't seem to make it work 100% reliably anyway so don't even bother, if script stalls just try again
	#sleep 0.1
	echo "$opt1" > pipe1
	echo "$opt2" > pipe2
	echo "$opt3" > pipe3
	echo "$opt4" > pipe4
	echo "$opt5" > pipe5
	echo 0 > pipe1
	wait
	dd if=pipe1 iflag=nonblock of=/dev/null &>/dev/null
	dd if=pipe2 iflag=nonblock of=/dev/null &>/dev/null
	dd if=pipe3 iflag=nonblock of=/dev/null &>/dev/null
	dd if=pipe4 iflag=nonblock of=/dev/null &>/dev/null
	dd if=pipe5 iflag=nonblock of=/dev/null &>/dev/null
	pkill intcode.bin
done 

rm pipe{1,2,3,4,5};
