#!/bin/sh

DRIVER=/opt/mud/bin/driver
ADDR_SERVER=/opt/mud/bin/addr_server
CONFIG=/opt/mud/bin/config

test -f $DRIVER || exit 0
test -f $ADDR_SERVER || exit 0
test -f $CONFIG || exit 0

echo "Starting address server ..."
$ADDR_SERVER 9997 2> /dev/null 1>&2 &

echo "Starting mud ..."
# $DRIVER $CONFIG 2> /dev/null 1>&2 &
$DRIVER $CONFIG

echo "Mud online!"
