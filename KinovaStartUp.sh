#!/usr/bin/env bash

SCRIPTPATH="$(cd "$(dirname "$0")" ; pwd -P)"

echo "Give the OS some time to finish booting"
sleep 10

echo "Creating logging folders if needed"
mkdir -p ${SCRIPTPATH}/logfiles/archive

echo "Starting KinovaZED"
until mate-terminal -x bash -c 'cd ${SCRIPTPATH} && sudo ./KinovaZED'; do
  echo "KinovaZED crashed with exit code $?. Restarting Script" >&2
  sleep 2
done

