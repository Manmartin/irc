/**************************************************************************/
/* This sample program provides a code for a connection-oriented server.  */
/**************************************************************************/

/**************************************************************************/
/* Header files needed for this sample program .                          */
/**************************************************************************/
#include <stdio.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/poll.h>
#include <string.h>
#include <unistd.h>

/**************************************************************************/
/* Constants used by this program                                         */
/**************************************************************************/
#define SERVER_PORT    12345
#define BUFFER_LENGTH    250
#define FALSE              0

int main()
{
   /***********************************************************************/
   /* Variable and structure definitions.                                 */
   /***********************************************************************/
   int    sd=-1, sd2=-1;
   int    rc, length, on=1;
   char   buffer[BUFFER_LENGTH];
   struct pollfd fds;
   nfds_t nfds = 1;
   int timeout;
   struct sockaddr_in6 serveraddr;

   /***********************************************************************/
   /* A do/while(FALSE) loop is used to make error cleanup easier.  The   */
   /* close() of each of the socket descriptors is only done once at the  */
   /* very end of the program.                                            */
   /***********************************************************************/
   do
   {
      /********************************************************************/
      /* The socket() function returns a socket descriptor, representing  */
      /* an endpoint.  The statement also identifies that the INET6       */
      /* (Internet Protocol version 6) address family with the TCP        */
      /* transport (SOCK_STREAM) will be used for this socket.            */
      /********************************************************************/
      sd = socket(AF_INET6, SOCK_STREAM, 0);
      if (sd < 0)
      {
         perror("socket() failed");
         break;
      }

      /********************************************************************/
      /* The setsockopt() function is used to allow the local address to  */
      /* be reused when the server is restarted before the required wait  */
      /* time expires.                                                    */
      /********************************************************************/
      rc = setsockopt(sd, SOL_SOCKET, SO_REUSEADDR, 
                      (char *)&on, sizeof(on));
      if (rc < 0)
      {
         perror("setsockopt(SO_REUSEADDR) failed");
         break;
      }

      /********************************************************************/
      /* After the socket descriptor is created, a bind() function gets a */
      /* unique name for the socket.  In this example, the user sets the  */
      /* s6_addr to zero, which allows connections to be established from */
      /* any client that specifies port 12345.                            */
      /********************************************************************/
      memset(&serveraddr, 0, sizeof(serveraddr));
      serveraddr.sin6_family      = AF_INET6;
      serveraddr.sin6_port        = htons(SERVER_PORT);
      memcpy(&serveraddr.sin6_addr, &in6addr_any, sizeof(in6addr_any));

      rc = bind(sd, (struct sockaddr *)&serveraddr, sizeof(serveraddr));
      if (rc < 0)
      {
         perror("bind() failed");
         break;
      }

      /********************************************************************/
      /* The listen() function allows the server to accept incoming       */
      /* client connections.  In this example, the backlog is set to 10.  */
      /* This means that the system will queue 10 incoming connection     */
      /* requests before the system starts rejecting the incoming         */
      /* requests.                                                        */
      /********************************************************************/
      rc = listen(sd, 10);
      if (rc< 0)
      {
         perror("listen() failed");
         break;
      }

      printf("Ready for client connect().\n");

      /********************************************************************/
      /* The server uses the accept() function to accept an incoming      */
      /* connection request.  The accept() call will block indefinitely   */
      /* waiting for the incoming connection to arrive.                   */
      /********************************************************************/
      sd2 = accept(sd, NULL, NULL);
      if (sd2 < 0)
      {
         perror("accept() failed");
         break;
      }

      /********************************************************************/
      /* The poll() function allows the process to wait for an event to   */
      /* occur and to wake up the process when the event occurs.  In this */
      /* example, the system notifies the process only when data is       */
      /* available to read.  A 30 second timeout is used on this poll     */
      /* call.                                                            */
      /********************************************************************/
      timeout = 30000;

      memset(&fds, 0, sizeof(fds));
      fds.fd = sd2;
      fds.events = POLLIN;
      fds.revents = 0;

      rc = poll(&fds, nfds, timeout);
      if (rc < 0)
      {
         perror("poll() failed");
         break;
       }

      if (rc == 0)
      {
         printf("poll() timed out.\n");
         break;
      }

      /********************************************************************/
      /* In this example we know that the client will send 250 bytes of   */
      /* data over.  Knowing this, we can use the SO_RCVLOWAT socket      */
      /* option and specify that we don't want our recv() to wake up until*/
      /* all 250 bytes of data have arrived.                              */
      /********************************************************************/
      length = BUFFER_LENGTH;
      rc = setsockopt(sd2, SOL_SOCKET, SO_REUSEPORT | SO_REUSEADDR,
                      (char *)&length, sizeof(length));
      if (rc < 0)
      {
         perror("setsockopt(SO_RCVLOWAT) failed");
         break;
      }

      /********************************************************************/
      /* Receive that 250 bytes data from the client                      */
      /********************************************************************/
      rc = recv(sd2, buffer, sizeof(buffer), 0);
      if (rc < 0)
      {
         perror("recv() failed");
         break;
      }

      printf("%d bytes of data were received\n", rc);
      if (rc == 0 ||
          rc < sizeof(buffer))
      {
         printf("The client closed the connection before all of the\n");
         printf("data was sent\n");
         break;
      }

      /********************************************************************/
      /* Echo the data back to the client                                 */
      /********************************************************************/
      rc = send(sd2, buffer, sizeof(buffer), 0);
      if (rc < 0)
      {
         perror("send() failed");
         break;
      }

      /********************************************************************/
      /* Program complete                                                 */
      /********************************************************************/

   } while (FALSE);

   /***********************************************************************/
   /* Close down any open socket descriptors                              */
   /***********************************************************************/
   if (sd != -1)
      close(sd);
   if (sd2 != -1)
      close(sd2);
}
