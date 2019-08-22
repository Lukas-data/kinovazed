#!/usr/bin/env bash

SCRIPTPATH="$(cd "$(dirname "$0")" ; pwd -P)"

echo "Give the OS some time to finish booting"
sleep 10

echo "Creating logging folders if needed"
mkdir -p ${SCRIPTPATH}/logfiles/archive

BASHRC=~/.bashrc
BASHRCTMP=/tmp/.bashrc

if [ -f ${BASHRC} ]; then
  cp ${BASHRC} ${BASHRCTMP}
fi
echo "cd ${SCRIPTPATH}" >> ${BASHRCTMP}

echo "Starting KinovaZED"
until mate-terminal -x /usr/bin/env bash --rcfile ${BASHRCTMP} -c 'sudo ./KinovaZED'; do
  echo "KinovaZED crashed with exit code $?. Restarting Script" >&2
  sleep 2
done

rm -f ${BASHRCTMP}

