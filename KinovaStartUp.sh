#!/usr/bin/env bash

SCRIPTPATH="$(cd "$(dirname "$0")" ; pwd -P)"
LASTLOG=/tmp/KinovaStartUp.log

if [ -f ${LASTLOG} ]; then
  rm -f ${LASTLOG}
fi

echo "Scriptpath: ${SCRIPTPATH}" >> ${LASTLOG}

echo "Give the OS some time to finish booting" >> ${LASTLOG}
sleep 10

echo "Creating logging folders if needed" >> ${LASTLOG}
mkdir -p ${SCRIPTPATH}/logfiles/archive

echo "Starting KinovaZED" >> ${LASTLOG}
until mate-terminal -x $(cd ${SCRIPTPATH} && sudo ./KinovaZED); do
  echo "KinovaZED crashed with exit code $?. Restarting Script" >> ${LASTLOG}
  sleep 2
done

