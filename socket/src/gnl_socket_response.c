//TODO: response_sb (status, body) STATUS(OK,KO), PAYLOAD/REASON(string) se ok si ferma altrimenti legge il body
//TODO: response_s (status) STATUS(OK,KO)

//TODO: open response_sb
//TODO: read response_sb
//TODO: read_N
//TODO: write
//TODO: append
//TODO: lock response_sb
//TODO: unlock response_sb
//TODO: close response_sb
//TODO: remove response_sb



//TODO: gnl_socket_message_sl (string list) --> len_string + string + num of following elements + len_bytes + bytes + ...
//TODO: capire come usare la read 2147479552bit max (268MB) quindi che si fa se la vittima è un file di 300MB?
//TODO: e se invece nella risposta invio il numero di vittime così il client le puà chiedere? (message_sn)
//TODO: gestire bene la max size, se un file supera ad es. 200MB dividere le richieste
//TODO: nel caso non le voglia allora si attacca