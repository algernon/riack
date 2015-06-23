/* riack -- Riak C client library
 * Copyright (C) 2015  Gergely Nagy <algernon@madhouse-project.org>
 * Copyright (C) 2015  Parth Oberoi <htrapdev@gmail.com>
 *
 * This library is free software: you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public License
 * as published by the Free Software Foundation, either version 3 of
 * the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#include "riack/platform.h"

#include <riack/client.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <stdarg.h>
#include <arpa/inet.h>

const char *
riack_version (void)
{
  return PACKAGE_VERSION;
}

const char *
riack_version_string (void)
{
  return PACKAGE_STRING;
}

riack_client_t *
riack_client_new (void)
{
  riack_client_t *client = (riack_client_t *)malloc(sizeof(riack_client_t));
  client->conn = 0;

  return client;

}

int
riack_client_free (riack_client_t *client)
{
  if (client == NULL)
    return -EINVAL;
  if (!client->conn ) {
    free(client);
    return -ENOTCONN;
    }
  else {
    free(client);
    return 0;
    }
}

int
riack_client_connect (riack_client_t *client, ...)
{
  int err_check, flag;
  va_list args;
  char *hostname;
  char port[5];
  struct addrinfo server, *serverinfo, *p;
  memset(&server, 0, sizeof(server));
  server.ai_family = AF_UNSPEC;
  server.ai_socktype = SOCK_STREAM;
  va_start(args, client);
  while ( (flag = va_arg(args, int)) != 0)
  {
    if (flag == RIACK_CONNECT_OPTION_HOST) {
      hostname = (char *)va_arg(args, char *);
      if ((err_check = getaddrinfo(hostname, "8087", &server, &serverinfo)) != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(err_check));
        return -EINVAL;
       }
    }
    else if (flag == RIACK_CONNECT_OPTION_PORT) {
      sprintf(port, "%d", va_arg(args, int));
      if ((err_check = getaddrinfo("127.0.0.1", port, &server, &serverinfo)) != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(err_check));
        return -EINVAL;
        }
     }
   }
  if (flag == RIACK_CONNECT_OPTION_NONE) {
    if ((err_check = getaddrinfo("127.0.0.1", "8087", &server, &serverinfo)) != 0) {
    fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(err_check));
    return -errno;
    }
  }
  // looping through all the results and connect to the first we can
  for(p = serverinfo; p != NULL; p = p->ai_next) {
    if ((client->fd = socket(p->ai_family, p->ai_socktype,
            p->ai_protocol)) == -1) {
        perror("socket");
        continue;
    }

    if (connect(client->fd, p->ai_addr, p->ai_addrlen) == -1) {
        close(client->fd);
        perror("connect");
        continue;
    }
    client->conn =1;
    break; // if we get here, we must have connected successfully
  }

  if (p == NULL) {
    // looped off the end of the list with no connection
    fprintf(stderr, "failed to connect\n");
    return -errno;
  }
  return 0;
  freeaddrinfo(serverinfo); // all done with this structure
  
}



int
riack_client_disconnect (riack_client_t *client)
{
  int rval;
  if (client== NULL || client->conn == 0)
    return -ENOTCONN;


  if(rval=shutdown(client->fd, 2) !=0) {
    
    return -errno;
    }
  else {
    return rval;
    client->conn = 0;
  }
  
}


int
riack_client_send (riack_client_t *client, riack_client_send_option_t option, riack_put_req_t *putreq)
{
  uint32_t length; //length of serialised data
  int scheck;
  
  if (option == RIACK_MESSAGE_PUTREQ)
  {
    struct buff
    {
      uint32_t length;
      uint8_t code;
      uint8_t data[0];
    } *buff;

    length = rpb_put_req__get_packed_size(putreq);

    buff = (struct buff *) malloc(length + sizeof (buff->length) + sizeof (buff->code));
    buff->length = htonl (length + 1);
    buff->code = 11;

    rpb_put_req__pack(putreq, buff->data);
    if (scheck = send(client->fd, buff, length + sizeof (buff->length) + sizeof (buff->code), 0) == -1) {
      free(buff);
      return -errno;
      
      }
    else {
      free(buff);
      return 0;
      }
  }
  }
  
int
riack_client_recv(riack_client_t *client)
{
  int n;
  uint32_t len;
  uint8_t msgcode;
  void *result_msg;
  //Receive response length 
  n = recv(client->fd, &len, 4, MSG_WAITALL);
  if (n != 4) {
    return -errno;
	}

  len = htonl(len);
  
  //Receive message code 
  n = recv(client->fd, &msgcode, 1, MSG_WAITALL);
  if (n != 1) {
	return -errno;
	}
  if (msgcode != 0) {
	//Receive additional data, if such exists.
	if (len>1) {
	  result_msg = malloc(len-1);
	  n = recv(client->fd, result_msg, len-1, MSG_WAITALL);
	  if (n != len-1) {
	    free(result_msg);
	  return -errno;
		}
	free(result_msg);
	return 0;
	  }
    }
}
