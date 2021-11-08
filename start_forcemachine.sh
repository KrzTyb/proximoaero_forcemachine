#!/bin/sh

export QT_QPA_EGLFS_NO_LIBINPUT=1
export QT_QPA_EGLFS_TSLIB=1
export TSLIB_PARAMETERS=mode=1024x600
export TSLIB_TSEVENTTYPE=INPUT
export TSLIB_CALIBFILE=/etc/pointercal
export TSLIB_CONFFILE=/etc/ts.conf
export TSLIB_PLUGINDIR=/usr/lib/ts
export TSLIB_TSDEVICE=/dev/input/touchscreen0
export QT_QPA_GENERIC_PLUGINS=evdevtouch:/dev/input/touchscreen0
export QT_QPA_EVDEV_TOUCHSCREEN_PARAMETERS=/dev/input/touchscreen0:rotate=270

./Proximo_Aero_ForceMachine
