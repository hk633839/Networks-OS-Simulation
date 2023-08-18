#include <stdio.h> 
#include <stdlib.h>
#include <sys/socket.h> 
#include <stdlib.h> 
#include <netinet/in.h> 
#include <arpa/inet.h>
#include <string.h> 
#include <unistd.h>
#include <stdint.h>
 
 
//structure to define messages 
struct message
{
    int message_type;
    int messge_length;
    char total_msg[4096];
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
static int mod_table[] = {0, 2, 1};
 
 
//function to encode the given string into base64
char *base64_encode(const unsigned char *data,int input_length,int *output_length) 
{
 
    *output_length = 4 * ((input_length + 2) / 3);
 
    char *encoded_data = malloc(*output_length);
    if (encoded_data == NULL) return NULL;
 
    for (int i = 0, j = 0; i < input_length;) {
 
        uint32_t octet_a = i < input_length ? (unsigned char)data[i++] : 0;
        uint32_t octet_b = i < input_length ? (unsigned char)data[i++] : 0;
        uint32_t octet_c = i < input_length ? (unsigned char)data[i++] : 0;
 
        uint32_t triple = (octet_a << 0x10) + (octet_b << 0x08) + octet_c;
 
        encoded_data[j++] = encoding_table[(triple >> 3 * 6) & 0x3F];
        encoded_data[j++] = encoding_table[(triple >> 2 * 6) & 0x3F];
        encoded_data[j++] = encoding_table[(triple >> 1 * 6) & 0x3F];
        encoded_data[j++] = encoding_table[(triple >> 0 * 6) & 0x3F];
    }
 
    //loop to make sure whether to pad the result or not
    for (int i = 0; i < mod_table[input_length % 3]; i++)
        encoded_data[*output_length - 1 - i] = '=';
 
    return encoded_data;
}

 
int main() 
{ 
    int socket_fd = 0, data_read, PORT=0;       
    struct sockaddr_in server_address;       //predefined structure of socket address
    char ip_address[15];                     //to dynamically give ip address
    struct message new_message;
    int temp_len;
 
    //take input from the command line for ip address
    printf("please enter the ip address:\n");
    scanf("%s", ip_address);

    //take input from the command line for port number
    printf("please enter the port number:\n");
    scanf("%d", &PORT);
 
    if ((socket_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) 
    { 
        printf("Socket creation error"); 
        return -1; 
    } 
    printf("client socket created\n");
 
    //to make the allocated space zero
    bzero(&server_address, sizeof(server_address));
 
    //assigning the family and port to the structure
    server_address.sin_family = AF_INET; 
    server_address.sin_port = htons(PORT); 
 
    // Convert IPv4 address from text to binary form 
    if(inet_pton(AF_INET, ip_address, &server_address.sin_addr)<=0)  
    { 
        printf("The given ip address is not supported"); 
        return -1; 
    } 

    // condition to connect and checking whether the connection was successful
    if (connect(socket_fd, (struct sockaddr *)&server_address, sizeof(server_address)) < 0) 
    { 
        printf("Connection Failed"); 
        return -1; 
    } 
    else 
    {
        printf("Connected to server\n");
    }
 
    //running a loop to maintain the connection
    while(1)
    {
        char temp[4096];
 
        //take input for the message to be sent
        printf("please enter the message type and corresponding message:\n");
        scanf("%d\n", &new_message.message_type);
        
        //checking the type of messages
        if(new_message.message_type !=1 && new_message.message_type !=3)
        {
            printf("please enter a valid message type!\n");
        }

        //taking the input data 
        fgets(temp, 4096, stdin);
        temp_len=strlen(temp);
        char *enc_msg=base64_encode(temp,temp_len,&temp_len);
        strcpy(new_message.total_msg, enc_msg);
        new_message.messge_length = strlen(new_message.total_msg);
 
        //sending the data to server
        if(new_message.message_type ==1 || new_message.message_type ==3)
        {
            send(socket_fd , &new_message , sizeof(new_message) , 0); 
            printf("Entered data has been sent\n");
        }

        //when to close the socket
        if(new_message.message_type == 3)
        {
            close(socket_fd);
            printf("The connection has been terminated\n");
            return -1;
        }
 
 
        //checking whether the received data is less than zero bytes
        if((data_read = read( socket_fd , &new_message, 4096)) < 0)
        {
            printf("An error occurred while reading the data");
            return -1;
        }
        else 
        {
            //sending an acknowledgement message
            if(new_message.message_type == 2)
            {
                printf("ACK message from the server: %s\n", new_message.total_msg);
                bzero(&new_message, sizeof(&new_message)); //making the buffer zero 
            }
        }
 
    }
    return 0; 
} 