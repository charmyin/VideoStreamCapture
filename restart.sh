#!/bin/bash
##Author charmyin
#############Stop the running capture program progress###########
##----Parameter:ipid eg: ./stop.sh 15

#----------Kill the specified running progress-------------#
echo $1
i=0
while read line; do
#echo $line
  if [[ -n "$line" ]]; then
   sudo kill ${line}
    ((i++))
  fi
done < $1.pids

#--------------Restart the capture program with ip first id----------------#
sudo ./runx.sh $1