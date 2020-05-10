#!/system/bin/sh
echo "start copy sounds files to /sdcard/" >> /data/log.txt
setprop sys.liuh_aaa  1
sleep 15
#chmod -R 0777 /system/Sounds
if [ ! -e /data/system.notfirstrun ]; then
	/system/bin/cp -fr /system/Sounds /sdcard/ 
#	/system/bin/cp -fr /system/Sounds/Sleep/"A 01.wav" /sdcard/
	/system/bin/touch /data/system.notfirstrun
fi

setprop sys.liuh_bbb  1
echo "copy files success!" >> /data/log.txt

