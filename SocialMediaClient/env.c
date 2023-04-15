int LOGINED = 0; // 0 is did not login 1 is logined

struct user_str
{
    char id[100];
    char mail[100];
    char fullname[100];
};

struct user_str USER;

pthread_t ID_THREAD_SOCKET;
SOCKET server_fd;
char mail_selected[100];
