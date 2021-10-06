#!/bin/bash

# the path where to generate random files
FILES_BASE_DIR=/tmp

# the time to wait before stop the test
WAIT_SECS=30

# number of minimum simultaneous clients to run
SIMULTANEOUS_CLIENTS=12

# array of active clients pid
ACTIVE_CLIENTS_PID=()

# counter of the files created
FILE_COUNTER=0

# change directory to the client directory
cd ../client/ || exit 1

# Remove the given pid from the array of active clients
function remove_pid {
  local NEW=()

  for PID in "${ACTIVE_CLIENTS_PID[@]}"; do
    [[ $PID -ne $1 ]] && NEW+=("$PID")
  done

  ACTIVE_CLIENTS_PID=("${NEW[@]}")
  unset NEW
}

# generate a random file with a random size (max 15MB)
function generate_file {
  local FILENAME=$FILES_BASE_DIR/gnl_stress_test_$((FILE_COUNTER++))
  local BYTES=$(((1 + $RANDOM) * (1 + $RANDOM % 32)))
  
  head -c $BYTES /dev/urandom > $FILENAME
  
  GENERATE_FILE_RESULT=$FILENAME
}

# spawn clients continuously
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

      # generate a random file
      generate_file
      FILENAME=$GENERATE_FILE_RESULT

      # run the client
      ./main -f /tmp/LSOfilestorage_stress_test.sk \
      -t 0 \
      -W $FILENAME \
      -r $FILENAME \
      -l $FILENAME \
      -u $FILENAME \
      -l $FILENAME \
      -c $FILENAME \
      &

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

#remove all the generated files
rm $FILES_BASE_DIR/gnl_stress_test_*