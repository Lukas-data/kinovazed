#!/bin/bash

until mate-terminal -x bash -c 'sudo /home/kinova/Documents/kinovaZED/build/KinovaZED -r;'; do

  echo "KinovaZED crashed with exit code $?. Restarting Script" >&2
  sleep 2
done

