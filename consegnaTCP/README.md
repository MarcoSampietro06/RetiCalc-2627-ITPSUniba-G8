# Consegna TCP - Reti di Calcolatori 2026-27
GRUPPO : G8
Componenti: Vergine Valerio, Sampietro Marco
- Sistema operativo in uso: macOS, Windows11.
- Progetti: "clientTCP_G8" e "serverTCP_G8" (sorgenti C, portabili Windows/Linux/macOS)

## Contenuto
- clientTCP_G8: sorgenti del client (client.c, protocol.h)
- serverTCP_G8: sorgenti del server (server.c, protocol.h)

## compilazione 
macOS:  

gcc -o server servverTCP_G8/src/server.c

gcc -o client clientTCP_G8/src/client.c

./server 28016

./client


Windows:

gcc -o server.exe serverTCP_G8/src/server.c -lws2_32

gcc -o server.exe serverTCP_G8/src/client.c -lws2_32

server.exe 28016

client.exe

## Esecuzione
1. Avviare il server: ./server 28016
2. Avviare il client e inserire IP (127.0.0.1) e porta (28016) da tastiera.
3. Il client invia tre interi n1 n2 n3; il server calcola n1+n2-n3.
   Se il risultato è 100 risponde QUIT e il client termina, altrimenti
   restituisce il risultato. Il server resta sempre in ascolto.
