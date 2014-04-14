#!/bin/bash

sudo ntpdate -u pool.ntp.org
sudo echo "Time synced at `date`" >> /home/cubie/Development/videocapture/VideoStreamCapture/log/dateSync.log
