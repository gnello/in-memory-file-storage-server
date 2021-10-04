#!/bin/bash

SCRIPTPATH="$( cd -- "$(dirname "$0")" >/dev/null 2>&1 || exit; pwd -P )"
WAIT_SECS=30
SIMULTANEOUS_CLIENTS=12
ACTIVE_CLIENTS_PID=()

# list of files to send (credits to https://stackoverflow.com/questions/1767384/ls-command-how-can-i-get-a-recursive-full-path-listing-one-line-per-file)
FILES=($(ls -laR dataset | grep '^-\|d' | awk ' /:$/&&f{s=$0;f=0} /:$/&&!f{sub(/:$/,"");s=$0;f=1;next} NF&&f{ print s"/"$9 }' | grep '.txt\|.jpg\|.zip\|.png'))

# change directory to the client directory
cd ../client/ || exit 1

# if the position i is equal to 1, then the
# respectively file in $FILES at position i
# was sent to the server
FILES_SENT=()

# init the array
for _ in ${#FILES[@]}
do
  FILES_SENT+=(0)
done

function remove_pid {
  local NEW=()

  for PID in "${ACTIVE_CLIENTS_PID[@]}"; do
    [[ $PID -ne $1 ]] && NEW+=("$PID")
  done

  ACTIVE_CLIENTS_PID=("${NEW[@]}")
  unset NEW
}

function run_clients {
  # endless loop
  while true
  do
    # update the active clients pid array
    for PID in "${ACTIVE_CLIENTS_PID[@]}"
    do
      # check if the given pid is terminated
      if ! kill -0 "$PID" >/dev/null 2>&1; then
        # remove pid from the array
        remove_pid $PID
      fi
    done

    # check if we need to run another client
    if [[ ${#ACTIVE_CLIENTS_PID[@]} -lt $SIMULTANEOUS_CLIENTS ]]; then
      # pick a random file
      FILE_INDEX=$(($RANDOM % ${#FILES[@]}))
      FILE=${FILES[$FILE_INDEX]}

      # if the file was already sent, remove it
      WRITE_COMMAND="-W ../test/$FILE"
      if [[ ${FILES_SENT[$FILE_INDEX]} -eq 1 ]]; then
        WRITE_COMMAND=""
      fi

      # set the file as sent
      FILES_SENT[$FILE_INDEX]=1

      # run the client
      ./main -f /tmp/LSOfilestorage_stress_test.sk -t 0 $WRITE_COMMAND -r $SCRIPTPATH/$FILE -R 3 -l $SCRIPTPATH/$FILE -u $SCRIPTPATH/$FILE &

      # update the active clients pid array
      ACTIVE_CLIENTS_PID+=($!)
    fi

  done
}

run_clients &
RUN_CLIENTS_PID="$!"

sleep $WAIT_SECS

kill -9 $RUN_CLIENTS_PID
#kill $(ps aux | grep 'stress' | awk '{print $2}')