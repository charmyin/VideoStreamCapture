#!/bin/bash
#############Stop the running capture program progress###########
##----Parameter:ipid eg: ./stop.sh 15
echo $1
i=0
while read line; do
#echo $line
  if [[ -n "$line" ]]; then
   sudo kill ${line}
    ((i++))
  fi
done < $1.pids

