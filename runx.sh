#!/bin/bash
i=0
while read line; do
echo $line
  if [[ -n "$line" ]]; then
    ipcConfig[i]=${line}
    ((i++))
  fi
done < ipcs.info

for i in "${ipcConfig[@]}"
do
	arr=$(echo $i | tr ";" "\n")
	
	for x in $arr
	do
	    echo "$x"
	    
	    (sudo ./main$x &)
		sleep 2
		#echo ${arr[0]}
		(sudo ffmpeg -f h264 -i /tmp/ipcfifo$x -qscale:v 1 -f image2 -vf fps=fps=1/10 /home/media/dkapm1/$x/rgb%08d.jpg )
		#(sudo ffmpeg -f h264 -i /tmp/ipcfifo${arr[0]} -qscale:v 1 -f image2 -vf fps=fps=1/10 /home/media/dkapm1/${arr[0]}/rgb%08d.jpg &)
	    
	    break 
	done
	
	
done

#echo "total array elements: ${#ipcConfig[@]}"
#echo "ipcConfig[2]: ${ipcConfig[2]}"

