adb push easylog_test /system/bin/.
adb shell chmod 777 /system/bin/easylog_test
adb shell ls -l /system/bin/easylog_test

adb shell "/system/bin/easylog_test"