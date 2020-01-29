#!/bin/bash
## ./script.sh 2>&1 | sort -n | tail -n1
trap 'pkill intcode.bin' TERM INT QUIT
mkfifo pipe{1,2,3,4,5}

# PART 1 LOOP
printf '%s\n' {0,1,2,3,4}{0,1,2,3,4}{0,1,2,3,4}{0,1,2,3,4}{0,1,2,3,4} | grep -Ev '(.).*\1' | while read -r combo
do
	opt1="${combo:0:1}"
	opt2="${combo:1:1}"
	opt3="${combo:2:1}"
	opt4="${combo:3:1}"
	opt5="${combo:4:1}"

	# to see each engine's output change the -1 to positive integer, eg: 1-5
	./intcode.bin -1 0 2>/dev/null < pipe1 > pipe2 &
	./intcode.bin -1 0 2>/dev/null < pipe2 > pipe3 &
	./intcode.bin -1 0 2>/dev/null < pipe3 > pipe4 &
	./intcode.bin -1 0 2>/dev/null < pipe4 > pipe5 &
	./intcode.bin -1 0 < pipe5 >> /tmp/day07part1 &
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
done

echo -e "solution part 1: "
sort -n /tmp/day07part1 | tail -n 1

rm pipe{1,2,3,4,5};
rm /tmp/day07part1
