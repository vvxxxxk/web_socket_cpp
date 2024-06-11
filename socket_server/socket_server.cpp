#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>

int main()
{
    /*
        int socket(int domain, int type, int protocol)
            - domain: 소켓이 사용할 주소 체계를 지정
                - AF_INET: IPv4
                - AF_INET6: IPv6
            - type: 소켓의 유형 지정
                - SOCK_STREAM: TCP 소켓
                - SOCK_DGRAM: UDP 소켓
            - protocol: 소켓을 사용할 프로토콜 선택, 일반적으로 0을 지정하여 시스템이 주어진 주소체계와 유형에 따라 적절한 프로토콜을 선택
            - return: 소켓을 성공적으로 생성 시 소켓의 파일 디스크립터(File Descriptor)를 반환하며, 실패 시 -1을 반환
                      파일 디스크립터는 이후에 소켓과 상호 작용하기 위해 사용
    */
    int server_fd;
    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if(server_fd < 0)
    {
        std::cout << "server socket create failed" << std::endl;
        exit(EXIT_FAILURE);
    }
    std::cout << "server socket create success" << std::endl;

    /*
        int bind(int sockfd, const struct sockaddr *addr, socklen_t addrlen)
            - sockfd: 바인딩할 소켓의 파일 디스크립터(File Descriptor)
            - addr: 바인딩할 주소 정보를 담고 있는 'sockaddr' 구조체의 포인터
                - sockaddr_in: IPv4 주소 정보 구조체
                - sockaddr_in6: IPv6 주소 정보 구조체
            - addrlen: 'addr' 구조체의 크기, 주로 'sizeof(struct sockaddr)' 또는 구조체적인 주소 구조체의 크기를 지정
            - return: 성공적으로 소켓을 바인딩하면 0을 반환, 실패 시 -1을 반환
    */
    struct sockaddr_in server_sockaddr;
    memset(&server_sockaddr, 0, sizeof(server_sockaddr));
    server_sockaddr.sin_family = AF_INET;           // 주소 체계
    server_sockaddr.sin_port = htons(8000);         // 포트 번호
    server_sockaddr.sin_addr.s_addr = INADDR_ANY;   // 모든 네트워크 인터페이스에서 수신
    if(bind(server_fd, (sockaddr *)&server_sockaddr, sizeof(server_sockaddr)) < 0)
    {
        std::cout << "server socket bind failed" << std::endl;
        exit(EXIT_FAILURE);
    }
    std::cout << "server socket bind success" << std::endl;

    /*
        int listen(int sockfd, int backlog)
            - sockfd: 대기 상태로 만들고자 하는 소켓 파일 디스크립터(File Descriptor)
            - backlog: 대기 큐의 크기를 나타내는 정수, 대기 큐는 클라이언트의 연결 요청을 일시적으로 저장하는 공간
                       일반적으로 클라이언트가 서버에 연결을 시도할 때, 연결 요청은 대기 큐에 들어가게 되며 서버는 대기 큐에서 순서대로 연결을 수락
            - return: 성공하면 0을 반환, 실패 시 -1을 반환
    */
    int listen_queue_size = 10;
    if(listen(server_fd, listen_queue_size) < 0)
    {
        std::cout << "server listen failed" << std::endl;
        exit(EXIT_FAILURE);
    }
    std::cout << "server listening on port 8000..." << std::endl;

    /*
        int accept(int sockfd, struct sockaddr *addr, socklen_t *addrlen)
            - sockfd: 서버 소켓의 파일 디스크립터(File Descriptor)
            - addr: 클라이언트의 주소 정보를 저장할 'sockaddr' 구조체의 포인터
                    accept() 함수 호출 시 이 포인터에 클라이언트의 주소 정보가 채워짐
            - addrlen: 'addr' 구조체의 크기를 나타내는 포인터
            - return: 연결 성공 시 클라이언트와 통신하기 위한 새로운 소켓의 파일 디스크립터(File Descriptor)

        * 일반적으로 accept() 함수는 블록킹 함수로 동작하여, 클라이언트의 연결 요청이 없을 경우에는 연결 요청이 발생할 때까지 대기
    */
    int new_socket;
    struct sockaddr_in client_addr;
    socklen_t client_addr_len = sizeof(client_addr);
    new_socket = accept(server_fd, (sockaddr*)&client_addr, &client_addr_len);
    if(new_socket < 0)
    {
        std::cout << "server accept failed" << std::endl;
        exit(EXIT_FAILURE);
    }
    std::cout << "server accept success" << std::endl;

    /*
        ssize_t recv(int sockfd, void *buf, size_t len, int flags)
            - sockfd: 데이터를 수신할 소켓의 파일 디스크립터(File Descriptor)
            - buf: 수신한 데이터를 저장할 버퍼
            - len: 버퍼의 크기
            - flags: 옵션 플래그, 특수한 전송 옵션을 설정하는데 사용하고 일반적으로 0으로 설정

        ssize_t send(int sockfd, const void *buf, size_t len, int flags)
            - sockfd: 데이터를 전송할 소켓의 파일 디스크립터(File Descriptor)
            - buf: 전송할 데이터가 저장된 버퍼의 포인터
            - len: 버퍼의 크기
            - flages: 옵션 플래그, 특수한 전송 옵션을 설정하는데 사용하고 일반적으로 0으로 설정
    */
    char buffer[1024];
    while(true)
    {
        memset(buffer, 0, sizeof(buffer));
        if(recv(new_socket, buffer, sizeof(buffer), 0) < 0)
        {
            std::cout << "client disconnected" << std::endl;
            close(new_socket);
            break;
        }
        std::cout << "Client: " << buffer << std::endl;

        std::string server_message;
        std::cout << "Server: ";
        std::getline(std::cin, server_message);
        send(new_socket, server_message.c_str(), server_message.length(), 0);
    }

    /*
        int close(int fd)
            - fd: 닫을 파일 디스크립터(File Descriptor)
            - return: 성공 시 0, 실패 시 -1을 반환
    */
    close(server_fd);

    return 0;
}