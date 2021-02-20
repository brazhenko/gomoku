#include "Network.h"

static bool port_is_gomoku(std::string ip, int port){

	struct sockaddr_in address;  /* the libc network address data structure */
	short int sock = -1;         /* file descriptor for the network socket */
	fd_set fdset;
	struct timeval tv;

	address.sin_family = AF_INET;
	address.sin_addr.s_addr = inet_addr(ip.c_str()); /* assign the address */
	address.sin_port = htons(port);

	/* translate int2port num */
	sock = socket(AF_INET, SOCK_STREAM, 0);
	fcntl(sock, F_SETFL, O_NONBLOCK);

	connect(sock, (struct sockaddr *)&address, sizeof(address));

	FD_ZERO(&fdset);
	FD_SET(sock, &fdset);
	tv.tv_sec = 0;             /* timeout */
	tv.tv_usec = 50;

	if (select(sock + 1, NULL, &fdset, NULL, &tv) == 1)
	{
		int so_error;
		socklen_t len = sizeof so_error;

		getsockopt(sock, SOL_SOCKET, SO_ERROR, &so_error, &len);

		if (so_error == 0){
            std::cout << "printing" << std::endl;
            std::string message("gomoku");
            write(sock, message.c_str(), strlen(message.c_str()));
			char recvBuff[1024];
			int n = 0;
            n = read(sock, recvBuff, sizeof(recvBuff)-1);
			//while ( (n = read(sock, recvBuff, sizeof(recvBuff)-1)) > 0)
			//{
            recvBuff[n] = 0;
			//}
            std::string mes(recvBuff);
			close(sock);
            std::cout << "recieved: " << mes << std::endl;
            // Если порт вернул "gomoku", значит наш
            if (mes == "gomoku")
                return true;
			return false;
		}else{
			close(sock);
			return false;
		}
	}    
	else
	    close(sock);

	return false;
}

std::vector<std::string> Network::find_active_users(int port)
{
    std::vector<std::string> players;
    const std::string ip = "192.168.";

    for (int i = 0; i < 255; i++) {
        
        std::string host = std::to_string(i);

        for (int j = 0; j < 250; j++) {                      
			

            std::string host2 = std::to_string(j);
            std::string ip2 = ip + host + "." + host2;

            if (port_is_gomoku(ip2, Network::port)){         
                players.push_back(ip2);       
                //printf("%s:%d is open\n", ip2.c_str(), Network::port);                
            } else {
                // printf("%s:%d is closed\n", ip2.c_str(), port);    
            }
        }       
    }
    return players;
}

std::string Network::send_message(std::string ip, int port, std::string message)
{
    struct sockaddr_in address;  /* the libc network address data structure */
	short int sock = -1;         /* file descriptor for the network socket */
	fd_set fdset;
	struct timeval tv;

	address.sin_family = AF_INET;
	address.sin_addr.s_addr = inet_addr(ip.c_str()); /* assign the address */
	address.sin_port = htons(port);

	/* translate int2port num */
	sock = socket(AF_INET, SOCK_STREAM, 0);
	fcntl(sock, F_SETFL, O_NONBLOCK);

	connect(sock, (struct sockaddr *)&address, sizeof(address));
    

	FD_ZERO(&fdset);
	FD_SET(sock, &fdset);
	tv.tv_sec = 0;             /* timeout */
	tv.tv_usec = 50;

	// if (select(sock + 1, NULL, &fdset, NULL, &tv) == 1)
	// {
	// 	int so_error;
	// 	socklen_t len = sizeof so_error;

	// 	getsockopt(sock, SOL_SOCKET, SO_ERROR, &so_error, &len);

	// 	if (so_error == 0){
            std::cout << "writing: " << message << std::endl;
            write(sock, message.c_str(), strlen(message.c_str()));
			char recvBuff[1024];
			int n = 0;
            n = read(sock, recvBuff, sizeof(recvBuff)-1);
			//while ( (n = read(sock, recvBuff, sizeof(recvBuff)-1)) > 0)
			//{
            recvBuff[n] = 0;
			//}
            std::string resp(recvBuff);
			close(sock);
            return resp;
		// }else{
		// 	close(sock);
		// 	return "";
	// 	}
	// }    
	// else
	//     close(sock);

	return "";
}

void Network::run_server(int port)
{
    int listenfd = 0, connfd = 0;
    struct sockaddr_in serv_addr;

    char sendBuff[1025];

    listenfd = socket(AF_INET, SOCK_STREAM, 0);
    memset(&serv_addr, '0', sizeof(serv_addr));
    memset(sendBuff, '0', sizeof(sendBuff));

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    serv_addr.sin_port = htons(port);

    bind(listenfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr));

    listen(listenfd, 10);

    while(1) {
        // Принимаю соединения
        connfd = accept(listenfd, (struct sockaddr*)NULL, NULL);

        char recvBuff[1024];
        int n = 0;
        n = read(connfd, recvBuff, sizeof(recvBuff)-1);
        //while ( (n = read(sock, recvBuff, sizeof(recvBuff)-1)) > 0)
        //{
        recvBuff[n] = 0;
        //}
        std::string mes(recvBuff);
        std::cout << "recieved: " << mes << std::endl;

        if (mes == "gomoku") {
            snprintf(sendBuff, sizeof(sendBuff), "%s", "gomoku");
            write(connfd, sendBuff, strlen(sendBuff));
            // std::string resp("gomoku");
            std::cout << "writing: " << sendBuff << std::endl;
            // std::string message("gomoku");
            // write(connfd, message.c_str(), strlen(message.c_str()));
        }

        close(connfd);
        sleep(1);
     }
}