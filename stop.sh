#!/bin/bash
#############Stop the running capture program progress###########
##----Parameter:ipid eg: ./stop.sh 15

#Load config parameters
source /etc/ipcs.config

i=0
while read line; do
#echo $line
  if [[ -n "$line" ]]; then
   sudo kill ${line}
    ((i++))
  fi
done < ${pipeProgramPath}pids/$1.pids
#remove the pids file
sudo rm ${pipeProgramPath}pids/$1.pids
echo "$1 has been stoped!"
