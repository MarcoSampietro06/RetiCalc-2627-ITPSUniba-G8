#ifndef PROTOCOL_H_
#define PROTOCOL_H_

#if defined WIN32
#include <winsock.h>
#define CLOSESOCKET closesocket
typedef int socklen_t;
#else
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#define CLOSESOCKET close
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define DEFAULT_PORT 28016
#define BUFFERSIZE 64     /* ogni messaggio e' una stringa a lunghezza fissa */
#define QLEN 5
#define TARGET 100        /* se res == TARGET il server invia QUIT */

#define MSG_WELCOME "connessione avvenuta"
#define MSG_QUIT "QUIT"

#endif /* PROTOCOL_H_ */
