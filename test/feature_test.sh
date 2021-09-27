#!/bin/bash

# change directory to the client directory
cd ../client/ || exit 1

# run client 1
./main -f /tmp/LSOfilestorage_feature_test.sk -p -t 200 -w ../test/dataset/













#esegue uno script Bash che lancia uno o più processi client
#configurati per testare singolarmente tutte le operazioni implementate dal server, con un ritardo tra diverse operazioni
#di 200 millisecondi. Il server deve essere terminato con il segnale SIGHUP al termine dell’esecuzione dello script.
#Lo script Bash lancia tutti client con l’opzione ‘-p’.