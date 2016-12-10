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
#include <getopt.h> /* Include Function for parsing -h */


#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>
#include <errno.h>
#include <limits.h>
//#include <arpa/inet.h> /* include for function inet_ntop()

#include <simple_message_client_commandline_handling.h> /* Include external Parser Functions */


/*
 * -------------------------------------------------------------- prototypes --
 */
static void usageinfo(FILE *outputdevice, const char *filename, int status);
int submitmessage(const char* server,const char* port, int* socketdescriptor, int verbose);



/*
 * -------------------------------------------------------------- defines --
 */
#define MAX_BUF_SIZE 10000000 /* maximum Buffer 10 MB */
#define READ_BUF_SIZE 5000

/*
 * -------------------------------------------------------------- global resource variables --
 */
const char* argv0; /*neccessary for verbose for not handing parameter to every function */

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


static void usageinfo(FILE *outputdevice, const char *filename, int status) {

	fprintf(outputdevice, "usage: %s options\n", filename);
	fprintf(outputdevice,"options:\n");
	fprintf(outputdevice,"	-s, --server <server>   full qualified domain name or IP address of the server\n");
	fprintf(outputdevice,"	-p, --port <port>       well-known port of the server [0..65535]\n");
	fprintf(outputdevice,"	-u, --user <name>       name of the posting user\n");
	fprintf(outputdevice,"	-i, --image <URL>       URL pointing to an image of the posting user\n");
	fprintf(outputdevice,"	-m, --message <message> message to be added to the bulletin board\n");
	fprintf(outputdevice,"	-v, --verbose           verbose output\n");
	fprintf(outputdevice,"	-h, --help\n");

	exit(status);
}


/**
 * start of main program
 *
 **/


int main(int argc, const char * argv[]) {

	  argv0=argv[0]; /*copy prog name to global variable*/
	  const char *server = NULL;
	  const char *port = NULL;
	  const char *user = NULL;
	  const char *message = NULL;
	  const char *imgurl = NULL;
	  int verbose = FALSE;
	  int opt = 0; /* variable for getopt */
	  int buffersize = 0; /* size for sendbuffer */
	  int* socketdescriptor=NULL; /* pointer to socketdescriptor for subroutine*/


	  /* calling parsing function for return of command line parameters */
	  smc_parsecommandline(argc, argv, &usageinfo, &server, &port, &user, &message, &imgurl, &verbose);

	  /* checking whether -h is a parameter of command line */
	  while ((opt = getopt(argc,(char **) argv, "h:")) != -1) {
	               switch (opt) {
	               case 'h':
	            	   usageinfo(stdout,argv[0],EXIT_SUCCESS);
	            	   break;
	               default:
	            	   break;
	               }
	  }

	  if (verbose==TRUE){
		 fprintf(stdout,"%s [%s, %s(), line %d]: Using the following options: server=\"%s\" port=\"%s\", user=\"%s\", img_url=\"%s\", message=\"%s\"\n", argv[0],__FILE__, __func__ ,__LINE__, server, port, user, imgurl, message);
	  }

	  /*variable for string to be sent*/
	  if (imgurl!=NULL) buffersize = (strlen(user) + strlen(imgurl) + strlen(message) + 20);
	  if (imgurl==NULL) buffersize = (strlen(user) + strlen(message) + 20);
	  char *sendbuffer = (char*) malloc (buffersize);

	  /*building string to be sent */
	  strcpy(sendbuffer,"user=");
	  strcat(sendbuffer,user);
	  strcat(sendbuffer,"\n");
	  if (imgurl!=NULL) {
		  strcat(sendbuffer,"img=");
		  strcat(sendbuffer,imgurl);
		  strcat(sendbuffer,"\n");
	  }
	  sendbuffer=strcat(sendbuffer,message);
	  sendbuffer=strcat(sendbuffer,"\0");

	  const char *finalmessage = sendbuffer;

	  socketdescriptor=malloc(sizeof(int));

	  if (submitmessage(server,port,socketdescriptor,verbose)==-1){
		  free(sendbuffer);
		  free (socketdescriptor);
		  exit(EXIT_FAILURE);
	  }

          /* variables for sending */
          size_t len = 0;
          ssize_t retlen = 0;
          ssize_t byteswritten = 0;

          /* Send message datagram to server */

              len = strlen(finalmessage); /* */

          /* checking whether message is to big */
              if (len > MAX_BUF_SIZE) {
            	  fprintf(stderr, "Message to send is too big - Maximum is 10 MB\n");
            	  free (sendbuffer);
                  if (close (*socketdescriptor)!=0){
                	  fprintf(stderr,"%s [%s, %s(), line %d]: Failed to close socket! \n",argv0,__FILE__, __func__ ,__LINE__);
                  	  }
        		  free ((int)*socketdescriptor);
            	  exit(EXIT_FAILURE);
              }

          /* sending message */
              while (byteswritten!= (ssize_t)len) {

            	  retlen=write((int)*socketdescriptor, finalmessage, len); /*adding bytes written if partial write is performed */
            	  if (retlen==-1){
            		  fprintf(stderr, "Write failed: %s\n", strerror(errno));
                	  free (sendbuffer);
                      if (close (*socketdescriptor)!=0){
                    	  fprintf(stderr,"%s [%s, %s(), line %d]: Failed to close socket! \n",argv0,__FILE__, __func__ ,__LINE__);
                      	  }
            		  free ((int)*socketdescriptor);
            		  exit(EXIT_FAILURE);
            	  }
            	  byteswritten+=retlen;
              }


              if (verbose==TRUE){
               		  fprintf(stdout,"write successful!\n");
               }



           /* shutdown Write from Client side */
           if (shutdown((int)*socketdescriptor,SHUT_WR)==-1){
                      fprintf(stderr, "Client Shutdown SHUT_WR failed: %s\n", strerror(errno));
                	  free (sendbuffer);
                      if (close (*socketdescriptor)!=0){
                    	  fprintf(stderr,"%s [%s, %s(), line %d]: Failed to close socket! \n",argv0,__FILE__, __func__ ,__LINE__);
                      	  }
            		  free ((int)*socketdescriptor);
                      exit(EXIT_FAILURE);
           }

           if (verbose==TRUE){
            		  fprintf(stdout,"shutdown SHUT_WR successful!\n");
            }


           /* open file for read from server */

           void *readbuffer=malloc(READ_BUF_SIZE);
           char *tempbuffer=malloc(MAX_BUF_SIZE);
           int offset=0;

           ssize_t bytesread=0;

           /*open file for write and write buffer in file */
           //FILE *write_fp = fopen("returnmessage.txt","w");
           size_t char_written=0;
           size_t char_written_sum=0;

           strcpy(tempbuffer,"");

           while ((bytesread=read(*socketdescriptor,readbuffer,READ_BUF_SIZE))!=0){
        	   if (bytesread==-1){
        		   fprintf(stderr,"read failed: %s\n", strerror(errno));
             	   free (sendbuffer);
                   if (close (*socketdescriptor)!=0){
                	   fprintf(stderr,"%s [%s, %s(), line %d]: Failed to close socket! \n",argv0,__FILE__, __func__ ,__LINE__);
                   	   }
                   free ((int)*socketdescriptor);
        		   free (readbuffer);
        		   exit(EXIT_FAILURE);
        	   	   }

        	  /* while ((char_written_sum)<(size_t)bytesread){
        			    char_written=fwrite(readbuffer, sizeof(char), bytesread ,write_fp);
        			    if ((char_written==0)&&(ferror(write_fp))){
        			            	      		                   fprintf(stderr,"fwrite failed!\n");
        			            	       		                   fclose(write_fp);
        			            	       		                   close (socketdescriptor);
        			            	       		                   exit(EXIT_FAILURE);
        			            	              	           	    }
        			    fflush(write_fp);
        			    char_written_sum+=char_written;
        			    fprintf(stdout,"%d bytes written\n", (int)char_written_sum);
        	   }
*/
        	   if ((offset+bytesread)>MAX_BUF_SIZE){
	                   fprintf(stderr,"Server Reply exceeded Maximum Limit of 10MB! --> EXIT Error\n");
	                   free (sendbuffer);
                       if (close (*socketdescriptor)!=0){
                      	 fprintf(stderr,"%s [%s, %s(), line %d]: Failed to close socket! \n",argv0,__FILE__, __func__ ,__LINE__);
                       	 }
                       free ((int)*socketdescriptor);
    	       		   free (readbuffer);
    	       		   exit(EXIT_FAILURE);
    	        	   }

               memcpy((tempbuffer+offset),readbuffer,bytesread);
               offset+=bytesread;
           }

           /*find "file=" in string and parse filename*/
           char* pos_file=strstr(tempbuffer,"file=");
           pos_file+=strlen("file=");
           char* pos_end=strstr(pos_file,"\n");
           char* filename = malloc ((int)pos_end-(int)pos_file+1);
           strncpy(filename,pos_file,((int)pos_end-(int)pos_file));
           FILE *write_html = fopen(filename,"w");
           if (write_html==NULL){
                 fprintf(stderr,"Failed to open HTML File!\n");
           	     free (sendbuffer);
                 if (close (*socketdescriptor)!=0){
                	 fprintf(stderr,"%s [%s, %s(), line %d]: Failed to close socket! \n",argv0,__FILE__, __func__ ,__LINE__);
                 	 }
                 free ((int)*socketdescriptor);
                 free (readbuffer);
    	       	 exit(EXIT_FAILURE);
    	         }

           free(filename);

           /*find "len=" in string and parse filename*/
           pos_file=strstr(pos_end,"len=");
           pos_file+=strlen("len=");
           pos_end=strstr(pos_file,"\n");
           char* length = malloc ((int)pos_end-(int)pos_file+1);
           strncpy(length,pos_file,((int)pos_end-(int)pos_file));
           char **endptr=malloc ((int)pos_end-(int)pos_file+1);
           long int filelength=strtol(length, endptr, 10);
           free(length);
           free(endptr);


           /*writing bytewise*/
           pos_end++;
           char_written_sum=0;
           while ((int)char_written_sum<(int)filelength){
                   	char_written=fwrite(pos_end, sizeof(char), filelength ,write_html);
                   	if ((char_written==0)&&(ferror(write_html))){
                   	     fprintf(stderr,"fwrite write_html failed!\n");
                   	     free (sendbuffer);
                         if (close (*socketdescriptor)!=0){
                        	 fprintf(stderr,"%s [%s, %s(), line %d]: Failed to close socket! \n",argv0,__FILE__, __func__ ,__LINE__);
                         	 }
                         free ((int)*socketdescriptor);
                         free (readbuffer);
                   	     fclose(write_html);
                   	     exit(EXIT_FAILURE);
                   	     }
                   	fflush(write_html);
                   	char_written_sum+=char_written;
           	   	   	}
           fclose(write_html);

           /*find "file=" in string and parse filename*/
           pos_file=strstr(pos_end,"file=");
           pos_file+=strlen("file=");
           pos_end=strstr(pos_file,"\n");
           filename = malloc ((int)pos_end-(int)pos_file+1);
           strncpy(filename,pos_file,((int)pos_end-(int)pos_file));
           FILE *write_png = fopen(filename,"w");
           if (write_png==NULL){
                 fprintf(stderr,"Failed to open PNG File!\n");
           	     free (sendbuffer);
                 if (close (*socketdescriptor)!=0){
                	 fprintf(stderr,"%s [%s, %s(), line %d]: Failed to close socket! \n",argv0,__FILE__, __func__ ,__LINE__);
                 	 }
                 free ((int)*socketdescriptor);
                 free (readbuffer);
    	       	 exit(EXIT_FAILURE);
    	         }
           free(filename);

           /*find "len=" in string and parse filename*/
           pos_file=strstr(pos_end,"len=");
           pos_file+=strlen("len=");
           pos_end=strstr(pos_file,"\n");
           length = malloc ((int)pos_end-(int)pos_file+1);
           strncpy(length,pos_file,((int)pos_end-(int)pos_file));
           endptr=malloc ((int)pos_end-(int)pos_file+1);
           filelength=strtol(length, endptr, 10);
           free(length);
           free(endptr);


           /*writing bytewise*/
           pos_end++;
           char_written_sum=0;
           while ((int)char_written_sum<(int)filelength){
                   	char_written=fwrite(pos_end, sizeof(char), filelength ,write_png);
                   	if ((char_written==0)&&(ferror(write_png))){
                   	     fprintf(stderr,"fwrite write_png failed!\n");
                   	     fclose(write_png);
                   	     free (sendbuffer);
                         if (close (*socketdescriptor)!=0){
                        	 fprintf(stderr,"%s [%s, %s(), line %d]: Failed to close socket! \n",argv0,__FILE__, __func__ ,__LINE__);
                         	 }
                         free ((int)*socketdescriptor);
                         free (readbuffer);
                   	     exit(EXIT_FAILURE);
                   	     }
                   	fflush(write_png);
                   	char_written_sum+=char_written;
           	   	   	}
           fclose(write_png);


/*finally free resources */
  	  free (sendbuffer);
      if (close (*socketdescriptor)!=0){
    	  fprintf(stderr,"%s [%s, %s(), line %d]: Failed to close socket! \n",argv0,__FILE__, __func__ ,__LINE__);
      	  }
	  free ((int)*socketdescriptor);
      free (readbuffer);
      free (readbuffer);

      return (EXIT_SUCCESS); /* 0 if execution was successful */
}

int submitmessage(const char* server,const char* port, int* socketdescriptor, int verbose){

/* variables for socket */
	struct addrinfo hints; /* struct for parameters for getaddrinfo*/
	struct addrinfo *result, *rp;
	int gea_ret; /* variable for getaddrinfo return */

/* Obtain address matching host/port */
	memset(&hints, 0, sizeof(struct addrinfo)); /* allocate memory and set all values to 0 */
	hints.ai_family = AF_UNSPEC;    /* Allow IPv4 or IPv6 */
	hints.ai_socktype = SOCK_STREAM; /* TCP Stream socket */
	hints.ai_flags = 0;
	hints.ai_protocol = 0;          /* Any protocol */

/*
 * getaddrinfo return == 0 if success, otherwise Error-Code
 * and fills results with
 */

	if ((gea_ret=getaddrinfo(server, port, &hints, &result))!= 0) {
		fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(gea_ret));
		return -1; 			/* return failure to main */
		}
	if (verbose==TRUE){
		fprintf(stdout,"%s [%s, %s(), line %d]: Obtained IP address for server %s and port %s\n",argv0,__FILE__, __func__ ,__LINE__,server,port);
		}

/* getaddrinfo() returns a list of address structures.
              Try each address until successfully connected.
              If socket (or connect) fails, close the socket
              and try the next address.

   struct addrinfo {
         int              ai_flags;
         int              ai_family;
         int              ai_socktype;
         int              ai_protocol;
         size_t           ai_addrlen;
         struct sockaddr *ai_addr;
         char            *ai_canonname;
         struct addrinfo *ai_next;
     };

*/

	for (rp = result; rp != NULL; rp = rp->ai_next) {
		/* socket()  creates  an endpoint for communication and returns a descriptor */
		*socketdescriptor=socket(rp->ai_family, rp->ai_socktype,rp->ai_protocol);
		if (*socketdescriptor == -1) continue;
        if (verbose==TRUE) fprintf(stdout,"%s [%s, %s(), line %d]: Socket successfully created!\n" ,argv0,__FILE__, __func__ ,__LINE__);
        if (connect(*socketdescriptor, rp->ai_addr, rp->ai_addrlen) != -1)  break; /* Success */
        close(*socketdescriptor);
        }

	/* No address could be successfully connected */
    if (rp == NULL) {
    	fprintf(stderr, "Could not connect to socket: %s\n", strerror(errno));
    	freeaddrinfo(result); 		/* result of getaddrinfo no longer needed */
    	return -1;			/* return failure to main */
    	}

    if (verbose==TRUE){
    	fprintf(stdout,"%s [%s, %s(), line %d]: Connected to port %s of server %s" ,argv0,__FILE__, __func__ ,__LINE__,port, server);
    	}

    freeaddrinfo(result);     /* result of getaddrinfo no longer needed */
    return 0; /*return for successfully executed subroutine*/
}
