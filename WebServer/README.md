# Web Server

## To Run:
Simply use make to make the server object file. Then bring up the server on your desired port with command:
```console
{Your Path}/server {Port (i.e. 1234)}
```

You can then send HTTP requests from a browser to your port, and see the output in the terminal.
You can also load the following sample files which are available in the *files* folder. Each one is for testing different file types.

<!-- `404.html` `cat.gif` `cat.png` `flower.jpg` `home.png` `index.html` `piano.mp3` `sample.pdf` -->

File name  | File type
------------- | -------------
cat.gif  | gif
cat.png  | png
flower.jpg  | jpg
home.png  | png
index.html  | html
piano.mp3  | mp3
sample.pdf  | pdf
404.html  | html



## Root directory and project structure

The root directory consists of the files folder along with Makefile, *sever.cpp* and our results.

The makefile is simply used to generate the sever object file. There are also plenty of informative comments in the *sever.cpp* file that explain the different parts of our code.
The following is an explanation of *server.cpp* in more details.

## server.cpp

### Building the server:
to do so, we input a server port from the command line and set the required settings.
```Cpp
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
```

### Waiting for requests:
We listen on our *sock_fd* and queue at most 10 connections. We then accept requests on *newsockfd* from time to time, and check whether there's been an actual request or not. 
```Cpp
    listen(sockfd, 10); 
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


```

### Parsing and handling data:
If there's been an actual request, we proceed to parse it and extract different components such as the protocol used or the url that was entered. At the same time, we'll build `response_header` or `error_header` (in the case of running into an error), which will later be written to `newsockfd` repectively.
The `response_header` consists of status, current date and content type (which are extracted from the request).
The `error_header` consists of protocol, error_status, current date and etc. 

If all is well, we'll look up the file that was entered, open it, and show its content.
If such file (or url), doesn't exist, we'll redirect the user to a simple *404 Not Found* page. 
```Cpp
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
```

Finally, note that if the user doesn't input anything. We redirect him to a simple *index.html* page.

## Results:
![Results](https://github.com/HesamAsad/CN_CHomeworks1/blob/main/WebServer/results.gif)
