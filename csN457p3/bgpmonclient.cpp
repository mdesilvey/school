/* 
 * 	Copyright (c) 2007 Colorado State University
 * 
 *	Permission is hereby granted, free of charge, to any person
 *	obtaining a copy of this software and associated documentation
 *	files (the "Software"), to deal in the Software without
 *	restriction, including without limitation the rights to use,
 *	copy, modify, merge, publish, distribute, sublicense, and/or
 *	sell copies of the Software, and to permit persons to whom
 *	the Software is furnished to do so, subject to the following
 *	conditions:
 *
 *	The above copyright notice and this permission notice shall be
 *	included in all copies or substantial portions of the Software.
 *
 *	THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 *	EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
 *	OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 *	NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
 *	HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 *	WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 *	FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 *	OTHER DEALINGS IN THE SOFTWARE.
 * 
 * 
 *  File: bgpmonclient.c
 * 	Authors: Dave matthews, He Yan, Nick Parrish
 *  Data: May 31, 2007 
 */

/*
 * A sample client for the bgpmon server
 *  
 * bgpmonclient server-ip server-port 
 * 
 * Will write stream of XML to stdout.
 * 
 * The required parameters are:
 * 
 * server-ip = ip address of bgpmon server
 * server-port = port number for bgpmon server
 * 
 * Important:
 * If you wish to modify this client, remember that you
 * are monitoring a real-time stream and need to process 
 * it in real-time!  Slow clients will be terminated!
 * 
 * The requests are sent to the bgpmon server in a XML format
 */
 
#include <stdio.h>     
#include <sys/socket.h> 
#include <arpa/inet.h>  
#include <stdlib.h>    
#include <string.h>   
#include <unistd.h>     
#include <errno.h>
#include <vector>
#include <string>
//#include <libxml/xmlmemory.h>
//#include <libxml/parser.h>
using namespace std;

#include "message.h"

vector<string> sendXML;
vector<string> tempXML;

void fatal(char *errorMessage)
{
    perror(errorMessage);
    exit(1);
}

ssize_t
readn( int fd, void *vptr, size_t n)
{
	/* Read N bytes from a socket.
	 */
	unsigned char *ptr;
	ptr = (unsigned char *)vptr;

	size_t 		nleft;
	ssize_t 	nread;
	nleft = n;
	while (nleft > 0)
	{
		if ((nread = read(fd, ptr, nleft)) < 0)
		{
			if (errno == EINTR)
				nread = 0;
			else
				return(-1);
		}
		else if (nread == 0) // EOF
			break;

		nleft -= nread;
		ptr += nread;
	}
	return (n - nleft); // return >= 0
}

void parseXML(char *xmlBuffer, int len, string ipPre)
{
	
	char *xml_ptr = xmlBuffer;
	char array[len];
	
	memcpy(array, xml_ptr, len);
    
    char *pch;
    
    int updateFlag = 0;
    vector<string> savedMessage;
    pch = strtok(xml_ptr,"<><><|");
    while (pch != NULL)
    {
          string temp = pch;
          savedMessage.push_back(temp);
          if(strcmp(pch, "message") == 0)
          {
          	//printf ("Beer1: %s\n",pch);
          	
          }
          if(strcmp(pch, "update") == 0)
          {
          	//printf ("Beer2: %s\n",pch);
          	updateFlag = 1;
          }
          if(strcmp(pch, "/message") == 0)
          {
          	//printf ("Beer3: %s\n",pch);  
          	if(updateFlag == 1)
          	{
          		Message m(savedMessage);
          		         		
          		//m.newMess(savedMessage);
          		m.checkMessage(ipPre);
          		
          	}
          	
          	updateFlag = 0;
          	savedMessage.clear();
          	//printf("FDASFASDFADSFADSFASDFADSASDFASDFASDFAFSDAFSDF\n");        	
          }
          //printf ("Beer: %s\n",pch);
          
          pch = strtok (NULL, "><><|");
    }  
}

int main(int argc, char *argv[])
{
	
    char *servIP = NULL; 
    char *servPort = NULL; 
    char *address_prefix = NULL;    

    switch ( argc )
    {
	    case 2:
 	   		servPort = "17900";
 	   		servIP = "129.82.138.6";
 	   		address_prefix = argv[1];
 	   		break;
 	   	default:
 	   		fprintf(stderr, "Usage: %s <bgpmon IP> <bgpmon Port> \n", argv[0]);
				exit(1);
      	break;
    }

    /* Create a reliable, stream socket using TCP */
    int sock;                        
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0)
        fatal("socket() failed");

    /* Construct the server address structure  and establish a connection */
    unsigned short echoServPort = atoi(servPort);     
    struct sockaddr_in echoServAddr; 
    memset(&echoServAddr, 0, sizeof(echoServAddr));     /* Zero out structure */
    echoServAddr.sin_family      = AF_INET;             /* Internet address family */
    echoServAddr.sin_addr.s_addr = inet_addr(servIP);   /* Server IP address */
    echoServAddr.sin_port        = htons(echoServPort); /* Server port */
    if (connect(sock, (struct sockaddr *) &echoServAddr, sizeof(echoServAddr)) < 0)
        fatal("connect() failed");

    /* Construct the request from command line arguments and send to the server */
    int size=2048;
    char *xml = (char *)malloc(size+1);
    xml[0] = '\0';
    xml[size] = '\0'; // null terminator after the buffer, just in case ;-)
    int lxml = 0;
    char *xmlver = "<?xml version=\"1.0\"?>\n";
    char *xmlname = "test client";
    char *type = "message";
    
    lxml = snprintf( xml, size, "%s<request type=\"%s\" name=\"%s\"/>\n\n", xmlver, type, xmlname );
    
	//fprintf(stderr, "Sending %s\n", xml);
    if ( send(sock, xml, lxml, 0) != lxml )
    	fatal("request sent a different number of bytes than expected");
	//fprintf(stderr, "Receiving\n");

    /* Echo the response back from the server */
    xml[size] = '\0';  
 
    char *tok;
    
    for ( ;; )
    {
        int len = readn(sock, xml, size); // size); 
        /* got it all, now echo it out */
        //parse the xml
        
        char *endPtr;
        for(int i = 0; i < len; i++)
        {
        	endPtr = strstr(xml, "</message>");
        	endPtr += 11;
        }
        
        if( (strstr(xml, "<message>")) != NULL )
        {        	
        	tok = strstr(xml, "<message>");
        	//char tmp[sizeof(tok)];
        //	printf("check: %c", tmp);
        	parseXML(xml, len, address_prefix);
        }
        if (len <= 0 )
          break;
        //fwrite(xml, 1, len, stdout);
        //fflush( stdout );
        // scan for <die> in buffer
        if ( strstr( xml, "<die>" ) != NULL )
        	break;
    }
	
	fprintf(stderr, "Done\n");

    close(sock);
    return 0;
}

