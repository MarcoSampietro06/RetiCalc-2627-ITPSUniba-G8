/*
 * clientTCP - Esonero Reti di Calcolatori 2026-27
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

int main(void) {
	char ip[64];
	int port, sock;
	struct sockaddr_in sad;
	char buf[BUFFERSIZE];
	int n1, n2, n3;

#if defined WIN32
	WSADATA wsa;
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) {
		errorhandler("Errore in WSAStartup()");
		return EXIT_FAILURE;
	}
#endif

	/* passo 2: IP e porta da tastiera */
	printf("Inserisci l'IP del server: ");
	fflush(stdout);
	if (scanf("%63s", ip) != 1) {
		errorhandler("IP non valido");
		clearwinsock();
		return EXIT_FAILURE;
	}
	printf("Inserisci la porta del server: ");
	fflush(stdout);
	if (scanf("%d", &port) != 1 || port <= 0 || port > 65535) {
		errorhandler("porta non valida");
		clearwinsock();
		return EXIT_FAILURE;
	}

	memset(&sad, 0, sizeof(sad));
	sad.sin_family = AF_INET;
	sad.sin_addr.s_addr = inet_addr(ip);
	sad.sin_port = htons((unsigned short) port);
	if (sad.sin_addr.s_addr == INADDR_NONE) {
		errorhandler("indirizzo IP non valido");
		clearwinsock();
		return EXIT_FAILURE;
	}

	sock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (sock < 0) {
		errorhandler("creazione socket fallita");
		clearwinsock();
		return EXIT_FAILURE;
	}

	/* passo 3: richiesta di connessione */
	if (connect(sock, (struct sockaddr *) &sad, sizeof(sad)) < 0) {
		errorhandler("connessione al server fallita");
		CLOSESOCKET(sock);
		clearwinsock();
		return EXIT_FAILURE;
	}

	/* passo 6: ricezione e stampa della stringa di benvenuto */
	if (!recv_all(sock, buf, BUFFERSIZE)) {
		errorhandler("ricezione fallita");
		CLOSESOCKET(sock);
		clearwinsock();
		return EXIT_FAILURE;
	}
	buf[BUFFERSIZE - 1] = '\0';
	printf("%s\n", buf);

	while (1) {
		/* passo 7: lettura di n1 n2 n3 e invio */
		printf("Inserisci tre interi n1 n2 n3: ");
		fflush(stdout);
		if (scanf("%d %d %d", &n1, &n2, &n3) != 3) {
			errorhandler("input non valido");
			break;
		}
		memset(buf, 0, BUFFERSIZE);
		sprintf(buf, "%d %d %d", n1, n2, n3);
		if (!send_all(sock, buf, BUFFERSIZE)) {
			errorhandler("invio fallito");
			break;
		}

		/* passo 10: ricezione risposta */
		if (!recv_all(sock, buf, BUFFERSIZE)) {
			errorhandler("ricezione fallita");
			break;
		}
		buf[BUFFERSIZE - 1] = '\0';
		printf("%s\n", buf);

		if (strcmp(buf, MSG_QUIT) == 0)
			break; /* chiude la connessione e termina */
		printf("risultato res = %s\n", buf);
	}

	CLOSESOCKET(sock);
	clearwinsock();
	return EXIT_SUCCESS;
}
