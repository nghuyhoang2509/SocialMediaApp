void *listen_socket()
{
    char server_recv[1024];
    int leave_flag = 0;

    while (1)
    {
        if (leave_flag == 1)
        {
            break;
        }
        int receive = recv(server_fd, server_recv, 1024, 0);
        if (receive == 0)
        {
            leave_flag = 1;
        }
        if (receive > 0)
        {
            json_object *data_parse_from_server = json_tokener_parse(server_recv);
            json_object *status_obj;
            json_object_object_get_ex(data_parse_from_server, "status", &status_obj);
            int status = json_object_get_int(status_obj);
            if (status == 1)
            {
                json_object *from_obj;
                json_object *data_obj;
                json_object_object_get_ex(data_parse_from_server, "from", &from_obj);
                json_object_object_get_ex(data_parse_from_server, "data", &data_obj);
                const char *data = json_object_get_string(data_obj);
                const char *from = json_object_get_string(from_obj);
                recieve_message(data);
            }
            else if (status == 2)
            {
                printf("Nguoi dung k online");
            }
        }
    }
}

int send_data_socket(const char *message, const char *to)
{
    char json_data[1024];
    sprintf(json_data, "{\"status\":\"1\",\"from\":\"%s\",\"to\":\"%s\",\"data\":\"%s\"}", USER.mail, to, message);
    if (send(server_fd, json_data, 1024, 0) < 0)
    {
    }
    else
    {
        return 0;
    };
}

int Connect_socket()
{
    WSADATA wsa;
    struct sockaddr_in server;
    char *message, server_reply[200];

    printf("\nInitialising Winsock...");
    if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
    {
        printf("Failed. Error Code : %d", WSAGetLastError());
        exit(EXIT_FAILURE);

        return 1;
    }

    printf("Initialised.\n");

    // Create a socket
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET)
    {
        printf("Could not create socket : %d", WSAGetLastError());
        exit(EXIT_FAILURE);
    }

    printf("Socket created.\n");

    server.sin_addr.s_addr = inet_addr("20.81.191.47"); // 20.81.191.47
    server.sin_family = AF_INET;
    server.sin_port = htons(9001);

    // Connect to remote server
    if (connect(server_fd, (struct sockaddr *)&server, sizeof(server)) < 0)
    {
        puts("connect error");
        exit(EXIT_FAILURE);
    }
    if (send(server_fd, USER.mail, 100, 0) < 0)
    {
        puts("send mail error");
        exit(EXIT_FAILURE);
    }
    if (recv(server_fd, server_reply, 200, 0) < 0)
    {
        puts("recv send mail error");
        exit(EXIT_FAILURE);
    }
    else if (strstr(server_reply, "success connect") == NULL)
    {
        puts("recv not success");
        exit(EXIT_FAILURE);
    }

    pthread_create(&ID_THREAD_SOCKET, NULL, listen_socket, NULL);
}