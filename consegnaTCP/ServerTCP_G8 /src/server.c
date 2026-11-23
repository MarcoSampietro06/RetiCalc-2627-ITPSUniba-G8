/*
 * serverTCP - Esonero Reti di Calcolatori 2026-27
 * Uso: server [porta]
 */
#include "protocol.h"

static void clearwinsock(void) {
#if defined WIN32
	WSACleanup();
#endif
}

static void errorhandler(const char *msg) {
	fprintf(stderr, "%s\n", msg);
}

/* Riceve esattamente len byte (TCP e' uno stream). Ritorna 1 ok, 0 errore/chiusura. */
static int recv_all(int sock, char *buf, int len) {
	int got = 0, n;
	while (got < len) {
		n = recv(sock, buf + got, len - got, 0);
		if (n <= 0)
			return 0;
		got += n;
	}
	return 1;
}

static int send_all(int sock, const char *buf, int len) {
	int sent = 0, n;
	while (sent < len) {
		n = send(sock, buf + sent, len - sent, 0);
		if (n <= 0)
			return 0;
		sent += n;
	}
	return 1;
}

/* Invia una stringa come messaggio di BUFFERSIZE byte */
static int send_msg(int sock, const char *text) {
	char buf[BUFFERSIZE];
	memset(buf, 0, BUFFERSIZE);
	strncpy(buf, text, BUFFERSIZE - 1);
	return send_all(sock, buf, BUFFERSIZE);
}

/* Gestisce un client fino alla chiusura della connessione */
static void handle_client(int csock) {
	char buf[BUFFERSIZE];
	int n1, n2, n3, res;
	char out[BUFFERSIZE];

	if (!send_msg(csock, MSG_WELCOME)) { /* passo 5 */
		errorhandler("send fallita");
		return;
	}

	while (1) {
		if (!recv_all(csock, buf, BUFFERSIZE)) /* passo 8 */
			break; /* il client ha chiuso (dopo QUIT) o errore */
		buf[BUFFERSIZE - 1] = '\0';

		if (sscanf(buf, "%d %d %d", &n1, &n2, &n3) != 3) {
			errorhandler("messaggio non valido ricevuto");
			break;
		}
		res = n1 + n2 - n3;
		printf("la somma di n1+n2-n3=%d\n", res);
		fflush(stdout);

		if (res == TARGET) { /* passo 9 */
			if (!send_msg(csock, MSG_QUIT))
				break;
		} else {
			sprintf(out, "%d", res);
			if (!send_msg(csock, out))
				break;
		}
	}
}

int main(int argc, char *argv[]) {
	int port = DEFAULT_PORT;
	int sock, csock;
	struct sockaddr_in sad, cad;
	socklen_t clen;

	if (argc > 1) {
		port = atoi(argv[1]);
		if (port <= 0 || port > 65535) {
			errorhandler("porta non valida");
			return EXIT_FAILURE;
		}
	}

#if defined WIN32
	WSADATA wsa;
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) {
		errorhandler("Errore in WSAStartup()");
		return EXIT_FAILURE;
	}
#endif

	sock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (sock < 0) {
		errorhandler("creazione socket fallita");
		clearwinsock();
		return EXIT_FAILURE;
	}

#if !defined WIN32
	int yes = 1; /* riavvio rapido del server */
	setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, (const char *) &yes, sizeof(yes));
#endif

	memset(&sad, 0, sizeof(sad));
	sad.sin_family = AF_INET;
	sad.sin_addr.s_addr = htonl(INADDR_ANY);
	sad.sin_port = htons((unsigned short) port);

	if (bind(sock, (struct sockaddr *) &sad, sizeof(sad)) < 0) {
		errorhandler("bind fallita");
		CLOSESOCKET(sock);
		clearwinsock();
		return EXIT_FAILURE;
	}
	if (listen(sock, QLEN) < 0) {
		errorhandler("listen fallita");
		CLOSESOCKET(sock);
		clearwinsock();
		return EXIT_FAILURE;
	}

	printf("Server in ascolto sulla porta %d\n", port);
	fflush(stdout);

	while (1) { /* il server resta sempre in ascolto (passo 11) */
		clen = sizeof(cad);
		csock = accept(sock, (struct sockaddr *) &cad, &clen);
		if (csock < 0) {
			errorhandler("accept fallita");
			continue;
		}
		printf("connesso con il client il cui nome IP: %s\n",
				inet_ntoa(cad.sin_addr)); /* passo 4 */
		fflush(stdout);

		handle_client(csock);
		CLOSESOCKET(csock);
	}

	/* non raggiunto */
	CLOSESOCKET(sock);
	clearwinsock();
	return EXIT_SUCCESS;
}
