#!/bin/bash
#Author Charmyin
###########Auto run video capture program############
#Saved in directory like : /home/media/dkapm1/20140405-20140425-30
#Start all in ipcs.info : sudo ./runx.sh ; Start the specified ipc : sudo ./runx.sh 18  

#Load config parameters
source ./ipcs.config
echo "---${pipeProgramPath}ipcs.info"

 
	i=0
	while read line; do
	#echo $line
	  if [[ -n "$line" ]]; then
	    ipcConfigLineArray[i]=${line}
	    ((i++))
	  fi
	done < ${pipeProgramPath}ipcs.info
 
