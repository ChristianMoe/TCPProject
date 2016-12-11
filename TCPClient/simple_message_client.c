/**
 * @file simple_message_client.c
 * Distributed Systems - Implementation of a simple TCP message client
 *
 * @author Christian Mödlhammer,ic14b027
 * @author Harald Partmann,ic15b039
 *
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
#include <sys/ioctl.h>

#include <simple_message_client_commandline_handling.h> /* Include external Parser Functions */

/*
 * -------------------------------------------------------------- prototypes --
 */
static void usageinfo(FILE *outputdevice, const char *filename, int status);
int connectsocket(const char *server,const char *port, int *socketdescriptor, int verbose);
int sendingmessage(char *finalmessage, int *socketdescriptor, int verbose);
int readingmessage(char *readbuffer, int *socketdescriptor, int verbose);
int parsebuffer(char *bufferstart,int offset, int verbose);
int writefile(char *bufferstart, char *filename, int filelength, int verbose);

/*
 * -------------------------------------------------------------- defines --
 */
#define MAX_BUF_SIZE 1074000000
#define READ_BUF_SIZE 1024
#define MAX_FILE_SIZE 10000000

/*
 * -------------------------------------------------------------- global resource variables --
 */
const char* argv0; /* necessary for verbose for not handing parameter to every function */

/**
 *
 * \brief The usageinfo for the Client
 *
 * This subroutine prints the usageinfo for the client and exits
 *
 * \param outputdevice the outputdevice for usage message
 * \param filename the file for which the usage info is valid
 * \param suc_or_fail the exit status
 *
 * \return void
 * \retval none
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
 *
 * \brief The main routine for the Client
 *
 * This is the main entry point for the simple_message_client
 *
 * \param argc the number of arguments
 * \param argv the arguments from the command line (including the program name in argv[0])
 *
 * \return always "success"
 * \retval 0 always
 *
 */

int main(int argc, const char * argv[]) {

	/* define the program variables */
		/* for parsing and getopt (-h) */
			const char *server = NULL;
			const char *port = NULL;
			const char *user = NULL;
			const char *message = NULL;
			const char *imgurl = NULL;
			int verbose = FALSE;
			int opt = 0; /* variable for getopt */
		/* for building the sent message string */
			int buffersize = 0; /* size of sendmessage */
			char *sendmessage = NULL;
		/* for connecting the socket */
			int *socketdescriptor=NULL; /* pointer to socket descriptor for subroutine*/
		/* for reading from server */
			char *readbuffer=NULL;
			int bytesread=0;
		/* for parsing subroutine */
		 	int offset=0;
	/* end of variable definition */

		argv0=argv[0]; /*copy prog name to global variable*/

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

	/* build final message to be transfered to server */

		/*variable for string to be sent*/
			if (imgurl!=NULL) buffersize = (strlen(user) + strlen(imgurl) + strlen(message) + 20);
			if (imgurl==NULL) buffersize = (strlen(user) + strlen(message) + 20);
			sendmessage = (char*) malloc (buffersize);
			if (sendmessage==NULL){
				fprintf(stderr,"%s [%s, %s(), line %d]: Failed to allocate memory! \n",argv0,__FILE__, __func__ ,__LINE__);
				exit(EXIT_FAILURE);
				}

		/*building string to be sent */
			strcpy(sendmessage,"user=");
			strcat(sendmessage,user);
			strcat(sendmessage,"\n");
			if (imgurl!=NULL) {
				strcat(sendmessage,"img=");
				strcat(sendmessage,imgurl);
				strcat(sendmessage,"\n");
		  		}
			sendmessage=strcat(sendmessage,message);
			sendmessage=strcat(sendmessage,"\0");

	/* calling subroutine for connecting to server and managing failure case */
		socketdescriptor=malloc(sizeof(int));
		if (socketdescriptor==NULL){
			fprintf(stderr,"%s [%s, %s(), line %d]: Failed to allocate memory! \n",argv0,__FILE__, __func__ ,__LINE__);
			free(sendmessage);
			exit(EXIT_FAILURE);
			}

		if (connectsocket(server,port,socketdescriptor,verbose)==-1){
			free(sendmessage);
			free(socketdescriptor);
			exit(EXIT_FAILURE);
			}

	/* calling subroutine for sending message to server and managing failure case */
		if (sendingmessage(sendmessage, socketdescriptor, verbose)==-1){
			if (close (*socketdescriptor)!=0){
				fprintf(stderr,"%s [%s, %s(), line %d]: Failed to close socket! \n",argv0,__FILE__, __func__ ,__LINE__);
				}
			free(socketdescriptor);
		 	exit(EXIT_FAILURE);
			}
		free(sendmessage); /*resource no longer needed*/

	/* shutdown Write from Client side and manage failure case */
		if (shutdown((int)*socketdescriptor,SHUT_WR)==-1){
			fprintf(stderr, "%s [%s, %s(), line %d]: Client Shutdown SHUT_WR failed: %s\n",argv0,__FILE__, __func__ ,__LINE__, strerror(errno));
			if (close (*socketdescriptor)!=0){
				fprintf(stderr,"%s [%s, %s(), line %d]: Failed to close socket! \n",argv0,__FILE__, __func__ ,__LINE__);
				}
			free(socketdescriptor);
			exit(EXIT_FAILURE);
			}

		if (verbose==TRUE){
			fprintf(stdout,"%s [%s, %s(), line %d]: Shutdown SHUT_WR successful! \n",argv0,__FILE__, __func__ ,__LINE__);
            }

	/* calling subroutine for reading message from server and managing failure case */
		readbuffer=malloc(MAX_BUF_SIZE);
		if (readbuffer==NULL){
			fprintf(stderr,"%s [%s, %s(), line %d]: Failed to allocate memory! \n",argv0,__FILE__, __func__ ,__LINE__);
			if (close (*socketdescriptor)!=0){
				fprintf(stderr,"%s [%s, %s(), line %d]: Failed to close socket! \n",argv0,__FILE__, __func__ ,__LINE__);
				}
			free(socketdescriptor);
			exit(EXIT_FAILURE);
			}

		if ((bytesread=readingmessage(readbuffer, socketdescriptor, verbose))==-1){
			if (close (*socketdescriptor)!=0){
				fprintf(stderr,"%s [%s, %s(), line %d]: Failed to close socket! \n",argv0,__FILE__, __func__ ,__LINE__);
				}
			free(socketdescriptor);
			free(readbuffer);
			exit(EXIT_FAILURE);
			}

    	if (verbose==TRUE){
   	    	fprintf(stdout,"%s [%s, %s(), line %d]: Total of %d bytes read from server!\n" ,argv0,__FILE__, __func__ ,__LINE__,bytesread);
		   	}

	/* calling subroutines for parsing and writing and managing failure case */
        while (offset<bytesread){
        	if ((offset=parsebuffer(readbuffer, offset, verbose))==-1){
        		if (close (*socketdescriptor)!=0){
        			fprintf(stderr,"%s [%s, %s(), line %d]: Failed to close socket! \n",argv0,__FILE__, __func__ ,__LINE__);
					}
        		free(socketdescriptor);
        		free(readbuffer);
        		exit(EXIT_FAILURE);
				}
        	}

	/*finally free resources */
        if (close (*socketdescriptor)!=0){
			fprintf(stderr,"%s [%s, %s(), line %d]: Failed to close socket! \n",argv0,__FILE__, __func__ ,__LINE__);
      	  	}
		if (verbose==TRUE){
			fprintf(stdout,"%s [%s, %s(), line %d]: Socket closed - Connection to server successfully terminated! \n", argv[0],__FILE__, __func__ ,__LINE__);
			}
		free(socketdescriptor);

	return (EXIT_SUCCESS); /* 0 if execution was successful */
}

/**
 *
 * \brief Connect to socket
 *
 * Subroutine that tries to connect to the socket!
 *
 * \param sever the server that should be connected to (IPv4 or IPv6 address or www.xyz.com)
 * \param port the port that should be used to connect
 * \param socketdescriptor the pointer to descriptor as returnvalue for later use
 * \param verbose tells whether subroutine should be verbose
 *
 * \retval 0 on success
 * \retval -1 on failure
 *
 */

int connectsocket(const char* server,const char* port, int* socketdescriptor, int verbose){

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

/* Info: getaddrinfo() returns a list of address structures.
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
     }; */

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

/**
 *
 * \brief Send message to Server
 *
 * Subroutine that tries to send the message string to the server
 *
 * \param finalmessage the message to be sent
 * \param socketdescriptor the connected socket to send the data to
 * \param verbose tells whether to be verbose
 *
 * \retval 0 on success
 * \retval -1 on error
 *
 */

int sendingmessage(char *finalmessage, int *socketdescriptor, int verbose){

	/* variables for sending */
		long int len = 0;
		long int byteswritten = 0;
		size_t len_check= 0;

	/* start of logic for subroutine */
   	    len = strlen(finalmessage);
   	    len_check = (size_t) len;

	/* checking whether message is to big */
   	    if (len_check > MAX_BUF_SIZE) {
   	    	fprintf(stderr, "%s [%s, %s(), line %d]: Message to send is too big - Maximum is %d!\n",argv0,__FILE__, __func__ ,__LINE__,MAX_BUF_SIZE);
   	    	return -1;
   	    	}

   	    if (verbose==TRUE){
   	    	fprintf(stdout,"%s [%s, %s(), line %d]: Going to send the following message consisting of %ld bytes ...\n %s\n" ,argv0,__FILE__, __func__ ,__LINE__,len,finalmessage);
   	    }

	/* sending message */
   	    while (byteswritten!=len) {
   	    	len=write((int)*socketdescriptor, finalmessage, len); /*adding bytes written if partial write is performed */
   	    	if (len==-1){
   	    		fprintf(stderr, "%s [%s, %s(), line %d]: Write failed: %s\n",argv0,__FILE__, __func__ ,__LINE__, strerror(errno));
   	    		return -1;
   	    		}
   	    	byteswritten+=(long int)len; /* counting the sum of written bytes */
   	    	}
   	    if (verbose==TRUE){
   	    	fprintf(stdout,"%s [%s, %s(), line %d]: Message sent to server!\n" ,argv0,__FILE__, __func__ ,__LINE__);
		   }

   	 return 0; /*return for successfully executed subroutine*/
}

/**
 *
 * \brief Reads message from Server
 *
 * Subroutine that tries to read response from server
 *
 * \param readbuffer the message string read from server
 * \param socketdescriptor the connected socket to send the data to
 * \param verbose tells whether to be verbose
 *
 * \retval 0 on success
 * \retval -1 on error
 *
 */

int readingmessage(char *readbuffer, int *socketdescriptor, int verbose){

	/* support variables for reading */
    	void *tmp_readbuffer=malloc(READ_BUF_SIZE);
		if (readbuffer==NULL){
			fprintf(stderr,"%s [%s, %s(), line %d]: Failed to allocate memory! \n",argv0,__FILE__, __func__ ,__LINE__);
			return -1;
			}

    	size_t offset=0;
    	ssize_t bytesread=0;
    	int avdata;

    /* start of logic for subroutine */
    	ioctl(socketdescriptor, FIONREAD, &avdata);
    	strcpy(readbuffer,"");

    	if (verbose==TRUE){
   	    	fprintf(stdout,"%s [%s, %s(), line %d]: Starting reading from socket ...\n" ,argv0,__FILE__, __func__ ,__LINE__);
		   	}

    /* perform reading */
    	while ((bytesread=read(((*socketdescriptor,tmp_readbuffer,READ_BUF_SIZE))!=0)||(offset==avdata))){
    		if (bytesread==-1){
    			fprintf(stderr,"%s [%s, %s(), line %d]: Read from Server failed: %s\n",argv0,__FILE__, __func__ ,__LINE__, strerror(errno));
    			free (tmp_readbuffer);
    			return -1;
    			}

       	/* check whether received message is exceeding maximum size */
       	    if ((offset+bytesread)>MAX_BUF_SIZE){
       	    	fprintf(stdout,"%s [%s, %s(), line %d]: Server Reply exceeded Maximum Limit of %d bytes. Data may be lost.\n" ,argv0,__FILE__, __func__ ,__LINE__,MAX_BUF_SIZE);
    			free (tmp_readbuffer);
    			return -1;
       	    	}
       	    memcpy((readbuffer+offset),tmp_readbuffer,bytesread); /* append read bytes to readbuffer */
       	    offset+=bytesread;
       	    fprintf(stdout,"%d bytes read\n",bytesread);
    		}

    free (tmp_readbuffer); /* no longer needed resource */
	return offset; /*returns bytes read upon success*/
}

/**
 *
 * \brief Parses and eventually call write subroutine
 *
 * Subroutine that tries to parse the received string and calls write subroutine upon successful parsing
 *
 * \param readbuffer the message string read from server
 * \param i_offset the value to create flexible pointer to buffer
 * \param verbose tells whether to be verbose
 *
 * \retval 0 on success
 * \retval -1 on error
 *
 */

int parsebuffer(char *bufferstart, int i_offset, int verbose){

	/* support variables for parsing */
	    char *pos_file=NULL;
	    char *pos_end=NULL;
	    char **endptr=NULL;

	/* start of logic for subroutine */

	/* search for "file=" in substring */
	    if((pos_file=strstr((bufferstart+i_offset),"file="))==NULL){
			fprintf(stdout,"%s [%s, %s(), line %d]: No filename found in Server response! \n" ,argv0,__FILE__, __func__ ,__LINE__);
			return MAX_BUF_SIZE; /* high return value to get out of subroutine queue */
			}
	    pos_file+=strlen("file=");

		if((pos_end=strstr(pos_file,"\n"))==NULL){
			fprintf(stderr,"%s [%s, %s(), line %d]: End of Line not found! \n" ,argv0,__FILE__, __func__ ,__LINE__);
			return -1;
			}

	/* copying value to new string */
		char tmp_filename[(size_t)(pos_end-pos_file+1)];
		memcpy(tmp_filename,pos_file,(size_t)(pos_end-pos_file));
		tmp_filename[strlen(tmp_filename)]='\0';

	    if (verbose==TRUE){
	    	fprintf(stdout,"%s [%s, %s(), line %d]: Filename %s parsed!\n" ,argv0,__FILE__, __func__ ,__LINE__,tmp_filename);
	    			}

	/* search for "len=" in substring */
		if((pos_file=strstr(pos_end,"len="))==NULL){
	   		fprintf(stderr,"%s [%s, %s(), line %d]: String \"len=\" not found! \n" ,argv0,__FILE__, __func__ ,__LINE__);
	   		return 0;
	   		}
		pos_file+=strlen("len=");

	    if((pos_end=strstr(pos_file,"\n"))==NULL){
	    	fprintf(stderr,"%s [%s, %s(), line %d]: End of Line not found! \n" ,argv0,__FILE__, __func__ ,__LINE__);
	   		return -1;
	   		}

	/* copying value to new string */
		char tmp_length[(size_t)(pos_end-pos_file+1)];
        memcpy(tmp_length,pos_file,(size_t)(pos_end-pos_file));
        tmp_length[strlen(tmp_length)-1]='\0';

    /* converting to numeric */
		endptr=malloc ((int)(pos_end-pos_file+1));
		if (endptr==NULL){
			fprintf(stderr,"%s [%s, %s(), line %d]: Failed to allocate memory! \n",argv0,__FILE__, __func__ ,__LINE__);
			return -1;
			}
		long int filelength=strtol(tmp_length, endptr, 10);
	    free(endptr);
	    if (verbose==TRUE){
	    	fprintf(stdout,"%s [%s, %s(), line %d]: File length %d parsed! \n" ,argv0,__FILE__, __func__ ,__LINE__,(int)filelength);
	    	}
    /* writing file up to MAX_FILE_SIZE */
	    if (filelength>=MAX_FILE_SIZE){
		    if (verbose==TRUE){
		    	fprintf(stdout,"%s [%s, %s(), line %d]: Skipping to write %s because of exceeding file size! \n" ,argv0,__FILE__, __func__ ,__LINE__,tmp_filename);
		    	}
	    	}
	/* writing file up to MAX_FILE_SIZE */
		else{
            if (writefile(++pos_end, tmp_filename, (int)filelength, verbose)==-1){
            	return -1;
	        	}
		    }

	return (i_offset+(pos_end-bufferstart)+filelength); /*return offset for next read*/

}

/**
 *
 * \brief Write file
 *
 * Subroutine that tries to write a file
 *
 * \param readbuffer the message string read from server
 * \param filename the name of the file to write
 * \param filelength number of characters/bytes to be read from readbuffer into file
 * \param verbose tells whether to be verbose
 *
 * \retval 0 on success
 * \retval -1 on error
 *
 */

int writefile(char *bufferstart, char *filename, int filelength, int verbose){

	/* support variables for reading */
		size_t char_written=0;
		size_t char_written_sum=0;

	/* start of logic for subroutine */
		FILE *write_fd = fopen(filename,"w");
		if (write_fd==NULL){
			fprintf(stderr, "%s [%s, %s(), line %d]: Failed to open File %s!\n",argv0,__FILE__, __func__ ,__LINE__,filename);
			return -1;
    		}
		if (verbose==TRUE){
			fprintf(stdout,"%s [%s, %s(), line %d]: File %s opened for writing!\n" ,argv0,__FILE__, __func__ ,__LINE__,filename);
			fprintf(stdout,"%s [%s, %s(), line %d]: Trying to write %d bytes ...\n" ,argv0,__FILE__, __func__ ,__LINE__,filelength);
			}
	/* perform writing to file */
	    while ((int)char_written_sum<(int)filelength){
	    	char_written=fwrite(bufferstart, sizeof(char), filelength ,write_fd);
	    	if ((char_written==0)&&(ferror(write_fd))){
	    		fprintf(stderr,"%s [%s, %s(), line %d]: Write to %s failed !\n" ,argv0,__FILE__, __func__ ,__LINE__,filename);
	            fclose(write_fd);
	            return -1;
	    		}
	    	fflush(write_fd);
	    	char_written_sum+=char_written;
	    	}

	    if (verbose==TRUE){
			fprintf(stdout,"%s [%s, %s(), line %d]: %d bytes written to %s!\n" ,argv0,__FILE__, __func__ ,__LINE__,char_written_sum,filename);
			}

    fclose(write_fd); /* file descriptor not needed anymore */
    if (verbose==TRUE){
			fprintf(stdout,"%s [%s, %s(), line %d]: File %s closed!\n" ,argv0,__FILE__, __func__ ,__LINE__,filename);
			}

    return 0; /*return for successfully executed subroutine*/
}
