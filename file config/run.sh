#!/bin/sh

if P=$(pgrep RD)
then
	echo "RD is running"
else
	echo "RD is not running"
	RD &
fi
if P=$(pgrep RD_SMART)            
then                            
        echo "RD_SMART is running"                    
else                                                
        echo "RD_SMART is not running"                
        RD_SMART &
fi
if P=$(ps | grep 'python3 RDhcPy/main.py' | grep -v "grep" | wc -l)
then
        echo "python3 RDhcPy/main.py is running"
else
	if [ -e /usr/lib/python3.7/site-packages/signalrcore ]
	then
		echo "python3 RDhcPy/main.py is not running"
        	python3 RDhcPy/main.py &
        else
        	echo "check again lib signalrcore"
        fi
fi
if P=$(pgrep UDP_1907)
then
        echo "UDP_1907 is running"
else
        echo "UDP_1907 is not running"
        UDP_1907 &
fi
