##
## @file Makefile
## Distributed Systems - simple_message-client Makefile
## 
## @author Christian Mödlhammer
## @date 2016-09-12
##
## @version 0.1
##


##
## ------------------------------------------------------------- variables --
##

CC=gcc52
CFLAGS=-Wall -Werror -Wextra -Wstrict-prototypes -pedantic -fno-common -O3 -g -std=gnu11
LDFLAGS=-lsimple_message_client_commandline_handling
CP=cp
CD=cd
MV=mv
RM=rm

OBJ1=simple_message_client.o
BIN1=simple_message_client

##
## ----------------------------------------------------------------- rules --
##

%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<

##
## --------------------------------------------------------------- targets --
##

.PHONY: all
all: $(BIN1)

$(BIN1): $(OBJ1)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS) 

.PHONY: clean
clean:
	@-rm -rf $(BIN) $(OBJ1) $@
	
	echo $(OBJ1) --> deleted!
	echo $(BIN1) --> deleted!


##
## ---------------------------------------------------------- documentation --
##

##
## ---------------------------------------------------------- dependencies --
##

##
## =================================================================== eof ==
##
