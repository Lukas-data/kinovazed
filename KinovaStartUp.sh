#!/usr/bin/env bash

SCRIPTPATH="$(cd "$(dirname "$0")" ; pwd -P)"

sleep 10

until mate-terminal -x bash -c 'cd ${SCRIPTPATH} && sudo ./KinovaZED -r;'; do
  echo "KinovaZED crashed with exit code $?. Restarting Script" >&2
  sleep 2
done

