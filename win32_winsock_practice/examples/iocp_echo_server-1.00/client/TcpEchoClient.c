//TcpEchoClient.c - a minimalistic echo client
// -----------------------------------------------------------------------------

// C language includes
#include <stdio.h>
#include <stdlib.h> // exit
#include <string.h>

// Windows includes
#include <windows.h>

#pragma warning(disable: 4996) // sprintf

// -----------------------------------------------------------------------------

// configuration
enum
{
   BUFLEN = 1000,
   SERVICE_PORT = 4000,
   SERVER_ADDRESS = INADDR_LOOPBACK
};

// variables
static int sock;
static int msgNumber;
static char msgBuf[BUFLEN];
static struct sockaddr_in sin;

// prototypes
static void createConnection(void);
static void createSocket(void);
static void init(void);
static void initWinsock(void);
static void prepareEndpoint(void);
static void recvBuffer(void);
static void run(void);
static void sendBuffer(void);

// -----------------------------------------------------------------------------

void main(void)
{
   init();
   run();
}

// -----------------------------------------------------------------------------

static void createConnection(void)
{
   printf("* connecting\n");
   if (connect(sock, (struct sockaddr*)&sin, sizeof(sin)) < 0)
   {
      int err = WSAGetLastError();
      printf("* error %d in connect\n", err);
      exit(1);
   }
   printf("* connected\n");
}

// -----------------------------------------------------------------------------

static void createSocket(void)
{
   sock = socket(AF_INET, SOCK_STREAM, 0);
   if (sock < 0)
   {
      int err = WSAGetLastError();
      printf("* error %d creating socket\n", err);
      exit(1);
   }
}

// -----------------------------------------------------------------------------

static void init(void)
{
   initWinsock();
   createSocket();
   prepareEndpoint();
   createConnection();
}

// -----------------------------------------------------------------------------

static void initWinsock(void)
{
   WSADATA wsaData;

   if (WSAStartup(0x202, &wsaData) == SOCKET_ERROR)
   {
      int err = WSAGetLastError();
      printf("* error %d in WSAStartup\n", err);
      exit(1);
   }
}
// -----------------------------------------------------------------------------

static void prepareEndpoint(void)
{
   sin.sin_family = AF_INET;
   sin.sin_addr.s_addr = htonl(SERVER_ADDRESS);
   sin.sin_port = htons(SERVICE_PORT);
}

// -----------------------------------------------------------------------------

static void recvBuffer(void)
{
   char* buf = msgBuf;
   int pendingLen = BUFLEN;

   printf("* receiving reply\n");

   while (pendingLen > 0)
   {
      int partialLen = recv(sock, buf, pendingLen, 0);

      if (partialLen > 0)
      {
         pendingLen -= partialLen;
         buf += partialLen;
         continue;
      }

      // ------

      if (partialLen == 0)
         printf("* connection closed by the server\n");

      else // partialLen < 0
      {
         int err = WSAGetLastError();
         printf("* error %d in recv\n", err);
      }

      exit(1);
   } // while
}

// -----------------------------------------------------------------------------

static void run(void)
{
   for (;;)
   {
      sendBuffer();
      recvBuffer();
   }
}

// -----------------------------------------------------------------------------

static void sendBuffer(void)
{
   char* buf = msgBuf;
   int pendingLen = BUFLEN;

   printf("* sending message\n");
   sprintf(msgBuf, "%05 *****", msgNumber++);

   while (pendingLen > 0)
   {
      int partialLen = send(sock, buf, pendingLen, 0);

      if (partialLen > 0)
      {
         pendingLen -= partialLen;
         buf += partialLen;
         continue;
      }

      // -----------

      if (partialLen == 0)
         printf("* connection closed by the server\n");

      else // partialLen < 0
      {
         int err = WSAGetLastError();
         printf("* error %d in send\n", err);
      }

      exit(1);
   } // while
}

// -----------------------------------------------------------------------------
// the end
