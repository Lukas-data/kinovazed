#!/usr/bin/env bash

KINOVABUILD="$(cd "$(dirname "$0")" ; pwd -P)"
LASTLOG=/tmp/KinovaStartUp.log

echo "Kinova build path: ${KINOVABUILD}" >> ${LASTLOG}

echo "Give the OS some time to finish booting" >> ${LASTLOG}
sleep 10

echo "Creating logging folders if needed" >> ${LASTLOG}
mkdir -p ${KINOVABUILD}/logs/

export KINOVABUILD

echo "Starting KinovaZED" >> ${LASTLOG}
until mate-terminal -x bash -c 'cd ${KINOVABUILD} && sudo ./KinovaZED'; do
  echo "KinovaZED crashed with exit code $?. Restarting Script" >> ${LASTLOG}
  sleep 2
done

