/**
 * @file simple_message_client.c
 * Distributed Systems - Implementation of a simple message client
 *
 * @author Christian Mödlhammer,ic14b027
 * @details This program implements a simple message client using a Library libsimple_message_client_commandline_handling.a for parsing the command line input
 * @date 2016-01-12
 *
 * @version 0.1
 *
 */

/*
 * -------------------------------------------------------------- includes --
 */
#include <stdio.h> /* Include Standard Input/Output Features */
#include <stdlib.h> /* Include Standard Library Functions */
#include <string.h> /* Include String Functions */

#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>

#include <simple_message_client_commandline_handling.h> /* Include external Parser Functions */


/*
 * -------------------------------------------------------------- prototypes --
 */
void bindadd(const char **server,const char **port,const char **message);
static void usageinfo(FILE *outputdevice, const char *filename, int suc_or_fail);

/*
 * -------------------------------------------------------------- defines --
 */
#define BUF_SIZE 10



/**
 * prints the usage information
 *
 * \param outputdevice 	specifies outputdevice for usage message
 * \param filename 		filename for which the usage is valid
 * \param suc_or_fail	exit as success or as failure
 *
 * \return none
 *
 */


/**
 * start of main program
 *
 **/


static void usageinfo(FILE *outputdevice, const char *filename, int suc_or_fail) {

	fprintf(outputdevice, "%s: Usage will be implemented \n", filename);
	exit(suc_or_fail);

}

int main(int argc, const char *argv[]) {

	  const char *server = NULL;
	  const char *port = NULL;
	  const char *user = NULL;
	  const char *message = NULL;
	  const char *imgurl = NULL;
	  char *user_text = "user=\0";
	  char *img_text = "img=\0";
	  char *lf_text = "\n";
	  int verbose = 0;

	  smc_parsecommandline(argc, argv, &usageinfo, &server, &port, &user, &message, &imgurl, &verbose);

	  /* Print values*/
	  fprintf(stdout, "Server:%s \n", server);
	  fprintf(stdout, "Port:%s \n", port);
	  fprintf(stdout, "User:%s \n", user);
	  fprintf(stdout, "Message:%s \n", message);

	  char *sendbuffer = malloc (strlen (user_text) + strlen (user) + strlen (lf_text) + strlen (img_text) + strlen (imgurl) + strlen (lf_text) + strlen (message) + 1);
	  strcpy(sendbuffer,user_text);
	  strcat(sendbuffer,user);
	  strcat(sendbuffer,lf_text);
	  if (imgurl!=NULL) {
		  strcat(sendbuffer,img_text);
		  strcat(sendbuffer,imgurl);
		  strcat(sendbuffer,lf_text);
	  }
	  sendbuffer=strcat(sendbuffer,message);
	  fprintf(stdout, "Text to send:%s \n", sendbuffer);

	  bindadd (&server, &port, &message);
	  return EXIT_SUCCESS;

}

void bindadd(const char **server,const char **port,const char **message){

          struct addrinfo hints;
          struct addrinfo *result, *rp;
          // void *readbuffer;
          int sfd, s;
          size_t len;
          ssize_t retlen;


          fprintf(stdout, "Server:%s \n", *server);
          fprintf(stdout, "Port:%s \n", *port);
          fprintf(stdout, "Message:%s \n", *message);

          /* Obtain address matching host/port */
          memset(&hints, 0, sizeof(struct addrinfo));
          hints.ai_family = AF_UNSPEC;    /* Allow IPv4 or IPv6 */
          hints.ai_socktype = SOCK_DGRAM; /* Datagram socket */
          hints.ai_flags = 0;
          hints.ai_protocol = 0;          /* Any protocol */

          s = getaddrinfo(*server, *port, &hints, &result);
          if (s != 0) {
              fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(s));
              exit(EXIT_FAILURE);
          }

          /* getaddrinfo() returns a list of address structures.
                        Try each address until we successfully connect(2).
                        If socket(2) (or connect(2)) fails, we (close the socket
                        and) try the next address. */

          for (rp = result; rp != NULL; rp = rp->ai_next) {
          sfd = socket(rp->ai_family, rp->ai_socktype,rp->ai_protocol);
                         if (sfd == -1) continue;

                         if (connect(sfd, rp->ai_addr, rp->ai_addrlen) != -1) break; /* Success */

                         close(sfd);
                     }

              if (rp == NULL) {               /* No address succeeded */
                 fprintf(stderr, "Could not connect\n");
                 exit(EXIT_FAILURE);
              }

              freeaddrinfo(result);           /* No longer needed */

              /* Send message datagram to server */

                  len = strlen(*message) + 1;
                  retlen = (ssize_t) len;

                  if (len + 1 > BUF_SIZE) fprintf(stderr, "Ignoring long message in argument\n");

                  if (write(sfd, *message, len) != retlen) {
                                    fprintf(stderr, "partial/failed write\n");
                                    exit(EXIT_FAILURE);
                                }
                  close(sfd);




}


