#!/bin/bash

SCRIPTPATH="$( cd -- "$(dirname "$0")" >/dev/null 2>&1 || exit; pwd -P )"
WAIT_SECS=30
SIMULTANEOUS_CLIENTS=12
ACTIVE_CLIENTS_PID=()

# change directory to the client directory
cd ../client/ || exit 1

# list of files to send
FILES=(\
"generic/little-prince-excerpt.txt" \
"generic/lorem-ipsum.txt" \
"generic/meme.jpg" \
"generic/one-late-night.txt" \
"generic/reallyinterestingimage.zip" \
"generic/simple-library/black-hole.txt" \
"generic/simple-library/states-of-a-programmer.png" \
"generic/simple-library/year-2038-problem.txt" \
)

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
      WRITE_COMMAND="-W ../test/dataset/$FILE"
      if [[ ${FILES_SENT[$FILE_INDEX]} -eq 1 ]]; then
        WRITE_COMMAND=""
      fi

      # set the file as sent
      FILES_SENT[$FILE_INDEX]=1

      # run the client
      ./main -f /tmp/LSOfilestorage_stress_test.sk -t 0 $WRITE_COMMAND -r $SCRIPTPATH/dataset/$FILE -R 10 &

      # update the active clients pid array
      ACTIVE_CLIENTS_PID+=($!)
    fi
  done
}

#./main -f /tmp/LSOfilestorage_stress_test.sk -w ../test/dataset/generic/

run_clients &
RUN_CLIENTS_PID="$!"

sleep $WAIT_SECS

kill -9 $RUN_CLIENTS_PID