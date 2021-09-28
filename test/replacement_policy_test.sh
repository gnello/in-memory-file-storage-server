#!/bin/bash

SCRIPTPATH="$( cd -- "$(dirname "$0")" >/dev/null 2>&1 || exit; pwd -P )"

# change directory to the client directory
cd ../client/ || exit 1

# run clients (-W)
for COUNTER in 1 2 3 4 5 6 7 8
do
./main -f /tmp/LSOfilestorage_feature_test.sk -p -W $SCRIPTPATH/dataset/big/big$COUNTER.txt -D /tmp
done