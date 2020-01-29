#!/bin/bash
printf '%s\n' {0,1,2,3,4}{0,1,2,3,4}{0,1,2,3,4}{0,1,2,3,4}{0,1,2,3,4} | grep -Ev '(.).*\1' | while read -r combo
do
	result="$(./day07 $combo 2>/dev/null)"
	echo $result
done | sort -n | tail -n 1

printf '%s\n' {5,6,7,8,9}{5,6,7,8,9}{5,6,7,8,9}{5,6,7,8,9}{5,6,7,8,9} | grep -Ev '(.).*\1' | while read -r combo
do
	result="$(./day07 $combo 2>/dev/null)"
	echo $result
done | sort -n | tail -n 1
	
