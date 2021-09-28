#!/bin/bash

SCRIPTPATH="$( cd -- "$(dirname "$0")" >/dev/null 2>&1 || exit; pwd -P )"

# change directory to the client directory
cd ../client/ || exit 1

# run client 1 (-w)
./main -f /tmp/LSOfilestorage_feature_test.sk -p -t 200 -w ../test/dataset/generic/

# run client 2, 3 (-r)
./main -f /tmp/LSOfilestorage_feature_test.sk -p -t 200 -r $SCRIPTPATH/dataset/generic/simple-library/year-2038-problem.txt,$SCRIPTPATH/dataset/generic/one-late-night.txt
./main -f /tmp/LSOfilestorage_feature_test.sk -p -t 200 -r $SCRIPTPATH/dataset/generic/reallyinterestingimage.zip,$SCRIPTPATH/dataset/generic/simple-library/black-hole.txt -d /tmp

# run client 4 (-R)
./main -f /tmp/LSOfilestorage_feature_test.sk -p -t 200 -R 5 -d /tmp/

# run client 5 (-c)
./main -f /tmp/LSOfilestorage_feature_test.sk -p -t 200 -c $SCRIPTPATH/dataset/generic/simple-library/states-of-a-programmer.png,$SCRIPTPATH/dataset/generic/meme.jpg,$SCRIPTPATH/dataset/generic/little-prince-excerpt.txt

# run client 6 (-l, -u)
./main -f /tmp/LSOfilestorage_feature_test.sk -p -t 200 -l $SCRIPTPATH/dataset/generic/simple-library/a-simple-folder/Ec83SuPWkAAIueg.jpg -u $SCRIPTPATH/dataset/generic/simple-library/a-simple-folder/Ec83SuPWkAAIueg.jpg

# run client 7 (-W)
./main -f /tmp/LSOfilestorage_feature_test.sk -p -t 200 -W ../test/dataset/generic/meme.jpg,../test/dataset/generic/little-prince-excerpt.txt
