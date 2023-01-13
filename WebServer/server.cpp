#include <bits/stdc++.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <fcntl.h>

#define BUFF_SIZE 8192
#define FILE_SIZE 4194304

using namespace std;

//used to log errors
void error (string msg) {
	cerr << msg << endl;
	exit(1);
}

int main(int argc, char* argv[]) {
	int sockfd = 0, newsockfd = 0; 
	int port_number = 0; 
	char clientReq[BUFF_SIZE] = { 0 }; 
	struct sockaddr_in serv_addr, cli_addr;

	int n = 0;
	if (argc < 2)
		error("You need to provide a port!");

	sockfd = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP); 
	
	if (sockfd < 0)
		error("Can't open socket!");

	memset(&serv_addr, 0, sizeof(serv_addr));
	memset(&cli_addr, 0, sizeof(cli_addr));

	//getting port number from cmd
	port_number = atoi(argv[1]);

	//ipv4 will be used to fill serv_addr
	serv_addr.sin_family = AF_INET; 

	//INADDR_ANY is an argument to bind that tells the socket to listen on all available interfaces.
	serv_addr.sin_addr.s_addr = htonl(INADDR_ANY); 
	
	//convert from host to network byte order
	serv_addr.sin_port = htons(port_number); 

	if (bind(sockfd, (struct sockaddr*) & serv_addr, sizeof(serv_addr)) < 0) 
		error("Cannot bind");
	else	
		cout << "Server is up..." << endl;

	//can queue at most 10 connection requests
	listen(sockfd, 10); 

	while (true) {
		socklen_t clilen = sizeof(cli_addr); 
		newsockfd = accept(sockfd, (struct sockaddr*) &cli_addr, &clilen);
		
		if (newsockfd < 0) 
			error("Cannot accept client request!"); 

		memset(clientReq, 0, BUFF_SIZE); 
		n = read(newsockfd, clientReq, BUFF_SIZE); 
		
		if (n < 0) 
			error("Cannot read from socket");
	
		if (n == 0) {
			cout << "No request sent from web..." << endl;
			close(newsockfd);
			continue;
		}
		cout << "Client request is:\n" << clientReq << endl;

		char data_copy[BUFF_SIZE] = { 0 };
		char file_name[32] = { 0 }, protocol[32] = { 0 }, status[32] = { 0 };
		char* parsed_data = NULL;

		strcpy(data_copy, clientReq); 
		parsed_data = strtok(data_copy, " ");  //get command (returns GET)
		parsed_data = strtok(NULL, " "); //get path (for example /404.html)

		//if there's a special file to be shown, we would copy the name to file_name
		if(parsed_data)
			sprintf(file_name, "%s", parsed_data);

		if (strlen(file_name) <= 1) //default file to be shown is localhost:{PORT}/index.html
			strcpy(file_name, "/files/index.html"); 

		//calculating and storing current time
		char date_temp[32] = { 0 };
		char response_header_date[64] = "Date: ";
		time_t timer = time(NULL); 
		struct tm* t = localtime(&timer);
		//Converts the date and time information from a given calendar time 
		//to a null-terminated multibyte character string str according to format string format.		
		strftime(date_temp, 32, "%c", t);
		strcat(response_header_date, date_temp); 
		
		if (parsed_data) {
			//store protocol in parsed_data (for example HTTP/1.1)
			parsed_data = strtok(NULL, " \r\n");
			if(parsed_data) 
				strcpy(protocol, parsed_data); 
			else
				error("No protocol defined!");
		}

		//We keep track of status to use in response header
		strcpy(status, protocol); 
		strcat(status, " 200 OK\r\n"); 

		//extract file type
		strcpy(data_copy, file_name); 
		parsed_data = strtok(data_copy, "."); 
		parsed_data = strtok(NULL, " \r\n"); //stores file type (for example html)

		char file_format[8] = { 0 };
		if (parsed_data) //if we have a file type
			strcpy(file_format, parsed_data); 
		else 
			strcpy(file_format, ""); 

		//start building content type for header
		char content_type[64] = "\nContent-Type: ";
		if (strcmp(file_format, "html") == 0 || strcmp(file_format, "htm") == 0) 
			strcat(content_type, "text/html"); 
		else if (strcmp(file_format, "gif") == 0) 
			strcat(content_type, "image/gif"); 
		else if (strcmp(file_format, "jpeg") == 0 || strcmp(file_format, "jpg") == 0) 
			strcat(content_type, "image/jpeg"); 
		else if (strcmp(file_format, "mp3") == 0) 
			strcat(content_type, "audio/mp3"); 
		else if (strcmp(file_format, "pdf") == 0) 
			strcat(content_type, "application/pdf"); 
		else if (strcmp(file_format, "ico") == 0) 
			strcat(content_type, "image/x-icon"); 
		strcat(content_type,"; charset=utf-8\r\n");  
		
		int header_size = strlen(status) + strlen(response_header_date) + strlen(content_type) + strlen("\r\n"); 
		char response_header[header_size] = { 0 }; 
		
		strcat(response_header, status); 
		strcat(response_header, response_header_date); 
		strcat(response_header, content_type); 
		strcat(response_header, "\r\n"); 

		char error_status[32] = { 0 };
		strcpy(error_status, protocol); 
		strcat(error_status, " 404 Not Found\r\n"); 
		int error_size = strlen(error_status) + strlen(response_header_date) + strlen("\r\n"); 
		char error_header[error_size] = { 0 }; 
		strcpy(error_header, error_status); 
		strcat(error_header, response_header_date); 
		strcat(error_header, "\r\n");

		char curr_dir[1024] = { 0 };
		getcwd(curr_dir, 1024); 
		strcat(curr_dir, "/files/");
		strcat(curr_dir, file_name); 

		int file_fd = open(curr_dir, O_RDONLY); 
		if (file_fd <= 0)
			file_fd = open("files/404.html", O_RDONLY); 
		if (file_fd > 0) {
			int res = write(newsockfd, response_header, header_size);
			if (res <= 0)
				error("Cannot write header!");

			char file_content[FILE_SIZE] = { 0 }; 
			while ( (n = read(file_fd, file_content, FILE_SIZE)) > 0) {
				res = write(newsockfd, file_content, n); 
				if (res <= 0)
					error("Cannot write file!");
				memset(file_content, 0, FILE_SIZE);
			}
			close(file_fd);
		}
		else {
			int res = write(newsockfd, error_header, error_size);
			if (res <= 0)
				error("Cannot write header!");
		}
		close(newsockfd);
	}
	close(sockfd);
    return 0; 
}
