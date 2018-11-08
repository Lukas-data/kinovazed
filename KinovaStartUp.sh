#!/bin/bash

until mate-terminal -x bash -c '/home/kinova/Documents/kinovaZED/_build/src -r;'; do

  echo "KinovaZED crashed with exit code $?. Restarting Script" >&2
  sleep 2

done

