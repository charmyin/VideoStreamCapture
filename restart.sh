#!/bin/bash
##Author charmyin
#############Stop the running capture program progress###########
##----Parameter:ipid eg: ./stop.sh 15

#Load config parameters
source /etc/ipcs.config

#----------Kill the specified running progress-------------#
echo $1
i=0
while read line; do
#echo $line
  if [[ -n "$line" ]]; then
   sudo kill ${line}
    ((i++))
  fi
done < ${pipeProgramPath}/pids/$1.pids

#--------------Restart the capture program with ip first id----------------#
sudo ${pipeProgramPath}runx.sh $1
