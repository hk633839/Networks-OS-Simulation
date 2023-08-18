#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
 
 
//structure to store the message received
struct message
{
	int message_type;
	int message_length;
	char total_message[4096];
};

// static table to make it faster
static char encoding_table[] = {'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H',
                                'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P',
                                'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X',
                                'Y', 'Z', 'a', 'b', 'c', 'd', 'e', 'f',
                                'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n',
                                'o', 'p', 'q', 'r', 's', 't', 'u', 'v',
                                'w', 'x', 'y', 'z', '0', '1', '2', '3',
                                '4', '5', '6', '7', '8', '9', '+', '/'};
static char *decoding_table = NULL;
static int mod_table[] = {0, 2, 1};
 
void build_decoding_table() {
 
    decoding_table = malloc(256);
 
    for (int i = 0; i < 64; i++)
        decoding_table[(unsigned char) encoding_table[i]] = i;
}
 
 
void base64_cleanup() {
    free(decoding_table);
} 
 
//function that decodes the given string from base64
unsigned char *base64_decode(const char *data,int input_length,int *output_length) 
{
 
    if (decoding_table == NULL) build_decoding_table();
 
    if (input_length % 4 != 0) return NULL;
 
    *output_length = input_length / 4 * 3;
    if (data[input_length - 1] == '=') (*output_length)--;
    if (data[input_length - 2] == '=') (*output_length)--;
 
    unsigned char *decoded_data = malloc(*output_length);
    if (decoded_data == NULL) return NULL;
 
    for (int i = 0, j = 0; i < input_length;) {
 
        uint32_t sextet_a = data[i] == '=' ? 0 & i++ : decoding_table[data[i++]];
        uint32_t sextet_b = data[i] == '=' ? 0 & i++ : decoding_table[data[i++]];
        uint32_t sextet_c = data[i] == '=' ? 0 & i++ : decoding_table[data[i++]];
        uint32_t sextet_d = data[i] == '=' ? 0 & i++ : decoding_table[data[i++]];
 
        uint32_t triple = (sextet_a << 3 * 6)
        + (sextet_b << 2 * 6)
        + (sextet_c << 1 * 6)
        + (sextet_d << 0 * 6);
 
        if (j < *output_length) decoded_data[j++] = (triple >> 2 * 8) & 0xFF;
        if (j < *output_length) decoded_data[j++] = (triple >> 1 * 8) & 0xFF;
        if (j < *output_length) decoded_data[j++] = (triple >> 0 * 8) & 0xFF;
    }
 
    return decoded_data;
}

int main()
{
 
	int socket_fd, bind_return, PORT=0;
	struct sockaddr_in serverAddr;     //predefined structure of socket address
 
	int newSocket;
	struct sockaddr_in newAddr;
    int out_msg_len;

     
 
	socklen_t addr_size;
 
	char buffer[1024];
	char ip_address[15];              //to dynamically give ip address
	pid_t childpid;                   //for maintaining concurrent client connections
 
	//taking the port number and ip address dynamically
	printf("please enter the ip address:\n");
	scanf("%s", ip_address);

	//take input from the command line for port number
    printf("please enter the port number:\n");
    scanf("%d", &PORT);

	//creates a file descriptor that can be used in function calls  on sockets
	socket_fd = socket(AF_INET, SOCK_STREAM, 0);
	if(socket_fd < 0)
	{
		printf("Error in connection.\n");
		return -1;
	}
	printf("Server Socket is created.\n");
 
	//to make the allocated space zero
	bzero(&serverAddr, sizeof(serverAddr));
	serverAddr.sin_family = AF_INET;     //AF_INET is family of IPV4 address
	serverAddr.sin_port = htons(PORT);   //to convert byte to network byte address
	serverAddr.sin_addr.s_addr = inet_addr(ip_address);

	//binds a unique local name to the socket with descriptor socket
	bind_return = bind(socket_fd, (struct sockaddr*)&serverAddr, sizeof(serverAddr));
	if(bind_return < 0)
	{
		printf("Error in binding.\n");
		return -1;
	}
	printf("Bind to port successful\n");
 
	if(listen(socket_fd, 10) == 0)      //listening/waitiing for a new conncection request
	{
		printf("Listening....\n"); 
	}
	else
	{
		printf("Error in listen function\n");
	}
 

    //maintaining a loop to receive connections from clients
	while(1)
	{
		//accepts a new connection from a client
		newSocket = accept(socket_fd, (struct sockaddr*)&newAddr, &addr_size);
		if(newSocket < 0)
		{
			return -1;
		}
		printf("Connection accepted from %s:%d\n", inet_ntoa(newAddr.sin_addr), ntohs(newAddr.sin_port));
 
		if((childpid = fork()) == 0) 
		{
			close(socket_fd); 
			struct message new_message;

			//maintaining a loop to receive message form clients
			while(1)
			{
				recv(newSocket, &new_message, 4096, 0);   //receive message from client
				
				if(new_message.message_type == 3)         //type-3 message disconnects form a connection
				{
					printf("Client : exit%s\n", new_message.total_message);
					printf("Disconnected from %s:%d\n", inet_ntoa(newAddr.sin_addr), ntohs(newAddr.sin_port));
					break;
				}
				else if(new_message.message_type == 1)   //type-1 message receives data from client
				{
					printf("Received message length: %d\n", new_message.message_length);
                    printf("Client(Encrypted MSG):\t");
					puts(new_message.total_message);
                    char *dec_msg=new_message.total_message;
                    out_msg_len=new_message.message_length;
                    dec_msg=base64_decode(dec_msg, out_msg_len, &out_msg_len);  //decodes from BASE64 to ASCII
                    printf("Original message length: %d\n", out_msg_len-1);
                    printf("Client(Decrypted MSG):\t");
                    puts(dec_msg);
					bzero(&new_message, sizeof(&new_message));
					new_message.message_type = 2;        //type-2 message sends ACK to client
					strcpy(new_message.total_message, "Data has been received!");
					send(newSocket, &new_message, sizeof(new_message), 0);
					bzero(&new_message, sizeof(&new_message));
				}
 
			}
		}
 
	}
 
	close(newSocket);
 
 
	return 0;
}