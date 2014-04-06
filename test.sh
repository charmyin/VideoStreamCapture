#!/bin/bash
###########Auto run video capture program
#Saved in directory like : /home/media/dkapm1/20140405-20140425-30
#
#test -e /dmtsai && echo "exist" || echo "Not exist"
#Main app path
pipeProgramPath=/home/cubie/Development/videocapture/VideoStreamCapture/
#Main path to save the images
imageSaveMainDir=/home/media/dkapm1/
sudo mount /dev/sda1 $imageSaveMainDir

i=0
while read line; do
#echo $line
  if [[ -n "$line" ]]; then
    ipcConfigLineArray[i]=${line}
    ((i++))
  fi
done < ipcs.info

for ipcConfigLine in "${ipcConfigLineArray[@]}"
do
	#Get config file info
	echo -e " \n-a--------$ipcConfigLine---b-----\n "
	ipcConfigArray=($(echo $ipcConfigLine | tr ";" "\n"))
	######Find or create the save directory by the datetime of now##########
	dateTimeNow=`date +%Y%m%d`
	dateTimeIntervalArray=($(echo ${ipcConfigArray[2]} | tr "," "\n"))
	#dateTime And Interval eg. 20140405-20140425-30
	dateTimeIntervalIndex=-1
	#intreval between shooting two images
	imageShootInterval=10
	for dateTimeInterval in "${dateTimeIntervalArray[@]}"
	do
	  dateTimeArray=($(echo $dateTimeInterval | tr "-" "\n"))
	  echo "$dateTimeNow ${dateTimeArray[0]} $dateTimeNow ${dateTimeArray[1]}" 
	  if [ "$dateTimeNow" -gt "${dateTimeArray[0]}" ] && [ "$dateTimeNow" -lt "${dateTimeArray[1]}" ]; then
	 	 dateTimeIntervalIndex=$dateTimeInterval
	 	 imageShootInterval=${dateTimeArray[2]}
	  fi
	  #for dateTime in "${dateTimeArray[@]}"
	  #do
	  	#echo $dateTime
	  	#if[ "$dateTimeNow" -gt ${date}]
	  #done
	  #echo $dateTimeInterval
	done
	
	#if [ "$dateTimeIntervalIndex" -eq "-1" ]; then
	if [ "${#dateTimeIntervalIndex}" -lt "3" ]; then
		echo -e "Warning, The config line upside is out of time! \n-------------\n"
		echo  $ipcConfig
	else
		#Start the jobs
		#1.Create directory  /home/media/dkapm1/15/20140405-20140425-30
		echo "mkdir $imageSaveMainDir${ipcConfigArray[0]}/$dateTimeIntervalIndex"
		(sudo mkdir -p $imageSaveMainDir${ipcConfigArray[0]}/$dateTimeIntervalIndex)
		#2.start job
		echo "sudo ${pipeProgramPath}main${ipcConfigArray[0]}"
		(sudo ${pipeProgramPath}main${ipcConfigArray[0]})
		#(sudo mkdir /home/media/dkapm1/$dateTimeIntervalIndex)
		sleep 2
		echo "sudo ffmpeg -f h264 -i /tmp/ipcfifo${ipcConfigArray[0]} -qscale:v 1 -f image2 -vf fps=fps=1/$imageShootInterval ${imageSaveMainDir}${ipcConfigArray[0]}/$dateTimeIntervalIndex/rgb%08d.jpg"
		(sudo ffmpeg -f h264 -i /tmp/ipcfifo${ipcConfigArray[0]} -qscale:v 1 -f image2 -vf fps=fps=1/$imageShootInterval ${imageSaveMainDir}${ipcConfigArray[0]}/$dateTimeIntervalIndex/rgb%08d.jpg &)
	fi
	#echo $dateTimeIntervalIndex
	
	#create a new directory
	#(sudo mkdir -p /home/media/dkapm1/${arr[0]})
	#echo ${arr[2]}
    #(sudo ./main${arr[0]} &)
	#sleep 2
	#(sudo ffmpeg -f h264 -i /tmp/ipcfifo${arr[0]} -qscale:v 1 -f image2 -vf fps=fps=1/10 /home/media/dkapm1/${arr[0]}/rgb%08d.jpg &)
	
done

#echo "total array elements: ${#ipcConfig[@]}"
#echo "ipcConfig[2]: ${ipcConfig[2]}"

