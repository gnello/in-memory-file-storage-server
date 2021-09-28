#!/bin/bash

SCRIPTPATH="$( cd -- "$(dirname "$0")" >/dev/null 2>&1 || exit; pwd -P )"
WAIT_SECS=30

# change directory to the client directory
cd ../client/ || exit 1

function foo {
    sleep 5
    echo "ciao"
}

foo &
echo "CUCU" &
echo "pasticcino"
echo "tenero" &


sleep $WAIT_SECS

#Viene lanciato uno script Bash che esegue
#ininterrottamente un numero di processi client (senza usare l’opzione ‘-p’) in modo tale che ce ne siano sempre
#almeno 10 in esecuzione contemporaneamente. Ogni processo client invia più richieste (almeno 5 richieste) al server
#con ‘-t 0’. Il test dopo 30 secondi si interrompe ed invia il segnale SIGINT al server e termina (senza aspettare
#l’eventuale terminazione dei processi client ancora attivi). Lo scopo di questo test è quello di eseguire uno stress test
#del server. Il test si intende superato se non si producono errori a run-time lato server e se il sunto delle statistiche
#prodotto dal server riporta “valori ragionevoli” (cioè, non ci sono valori negativi, valori troppo alti, campi vuoti,
#etc...).