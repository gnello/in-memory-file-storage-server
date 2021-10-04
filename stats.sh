#!/bin/bash

function avg_op {
  local TIMES=0
  local ARR
  local AVG=0
  local COUNT=0

  # count the times the op was performed
  TIMES=`grep -o "$1" $2 | wc -l`

  if [[ $TIMES -eq 0 ]]; then
    AVG_OP_RESULT=0
    return
  fi

  # get the array of op data
  ARR=`grep -o "$1" $2 | cut -d ' ' -f 2`

  # sum the op data
  for i in $ARR; do let "COUNT += $i"; done

  # calculate the average
  AVG=$(($COUNT/$TIMES))

  if [[ $AVG -eq 0 ]]; then
    AVG=0
  fi

  AVG_OP_RESULT=$AVG
}

function max_op {
  local ARR
  local MAX=0

  # get the array of op data
  ARR=`grep -o "$1" $3 | cut -d ' ' -f $2`

  # sum the op data
  for i in $ARR;
  do
    if (( $(echo "$i > $MAX" | bc -l) )); then
      MAX=$i
    fi
  done

  MAX_OP_RESULT=$MAX
}

if [[ $# -ne 1 ]]; then
    echo "Usage: $0 [server log path]"
    exit 1
fi

if [[ ! -f $1 ]]; then
    echo "File $1 does not exist"
    exit 1
fi

LOG_PATH=$1

# calculate totals
READS=`grep "READ request" $LOG_PATH | wc -l`
WRITES=`grep "WRITE request" $LOG_PATH | wc -l`
OPENS=`grep "OPEN request" $LOG_PATH | wc -l`
OPEN_LOCKS=`grep "Open: O_CREATE flag set" $LOG_PATH | wc -l`
LOCKS=`grep -w "LOCK request" $LOG_PATH | wc -l`
UNLOCKS=`grep "UNLOCK request" $LOG_PATH | wc -l`
CLOSES=`grep "CLOSE request" $LOG_PATH | wc -l`
EVICTIONS=`grep "Start eviction" $LOG_PATH | wc -l`
BROADCASTS_AFTER_LOCK=`grep "broadcast to pid [0-9]\+" $LOG_PATH | wc -l`
REQUESTS_HANDLED=`grep "client [0-9]\+ request handled" $LOG_PATH | wc -l`
DEADLOCK_AVOIDED=`grep "deadlock avoided" $LOG_PATH | wc -l`

# calculate the bytes read average
avg_op "Read: [0-9]\+ bytes read" $LOG_PATH
BYTES_READ_AVG=$AVG_OP_RESULT

# calculate the bytes written average
avg_op "Write: [0-9]\+ bytes written" $LOG_PATH
BYTES_WRITTEN_AVG=$AVG_OP_RESULT

# calculate the max storage size reached
max_op "The heap size is now [0-9]\+\.[0-9]\+ MB" 6 $LOG_PATH
MAX_STORAGE_MB=$MAX_OP_RESULT

# calculate the max file count reached
max_op "The file system has now [0-9]\+ files" 6 $LOG_PATH
MAX_STORAGE_FILES=$MAX_OP_RESULT

# calculate the max simultaneous connections reached
max_op "the server has now [0-9]\+ active connections" 5 $LOG_PATH
MAX_CONN=$MAX_OP_RESULT

echo "total \"read\" operations: $READS, average read bytes: $BYTES_READ_AVG"
echo "total \"write\" operations: $WRITES, average written bytes: $BYTES_WRITTEN_AVG"
echo "total \"lock\" operations: $LOCKS"
echo "total \"open\" operations: $OPENS"
echo "total \"open-lock\" operations (of $OPENS opens): $OPEN_LOCKS"
echo "total \"unlock\" operations: $UNLOCKS"
echo "total \"close\" operations: $CLOSES"
echo "total \"eviction\" operations: $EVICTIONS"
echo "total \"broadcasts\" operations after waiting a lock: $BROADCASTS_AFTER_LOCK"
echo "total deadlocks avoided: $DEADLOCK_AVOIDED"
echo "Maximum storage size reached: $MAX_STORAGE_MB MB"
echo "Maximum storage files reached: $MAX_STORAGE_FILES"

echo "Request handled per thread:"
if [[ $REQUESTS_HANDLED -eq 0 ]]; then
  echo "No requests handled"
else
  awk '/worker_[0-9]+.DEBUG client [0-9]+ request handled/ {arr[$3]++}END{for (a in arr) print a, arr[a]}' $LOG_PATH \
  | sort -g | sed 's/.DEBUG/:/' | sed 's/gnl_fss_//' | sed 's/_/ /'
fi

echo "Maximum simultaneous connections reached: $MAX_CONN"