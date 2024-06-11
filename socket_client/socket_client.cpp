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
    int client_fd;
    client_fd = socket(AF_INET, SOCK_STREAM, 0);
    if(client_fd < 0)
    {
        std::cout << "client socket create failed" << std::endl;
        exit(EXIT_FAILURE);
    }

    /*
        int connect(int sockfd, const struct sockaddr *addr, socklen_t addrlen)
            - sockfd: 연결을 시도할 소켓의 파일 디스크립터(File Descriptor)
            - addr: 서버 소켓의 주소 정보가 담긴 'sockaddr' 구조체에 대한 포인터
            - addrlen: 'addr' 구조체의 크기를 나타내는 변수
    */
    struct sockaddr_in server_sockaddr;
    memset(&server_sockaddr, 0, sizeof(server_sockaddr));
    server_sockaddr.sin_family = AF_INET;           // 주소 체계
    server_sockaddr.sin_port = htons(8000);         // 포트 번호
    server_sockaddr.sin_addr.s_addr = INADDR_ANY;   // 모든 네트워크 인터페이스에서 수신
    if(connect(client_fd, (sockaddr *)&server_sockaddr, sizeof(server_sockaddr)) < 0)
    {
        std::cout << "client connect failed" << std::endl;
        exit(EXIT_FAILURE);
    }
    std::cout << "client connect success" << std::endl;

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
        std::string client_message;
        std::cout << "Client: ";
        std::getline(std::cin, client_message);

        send(client_fd, client_message.c_str(), client_message.length(), 0);

        memset(buffer, 0, sizeof(buffer));
        if(recv(client_fd, buffer, sizeof(buffer), 0) < 0)
        {
            std::cout << "server disconnected" << std::endl;
            close(client_fd);
            break;
        }
        std::cout << "Server: " << buffer<< std::endl;
    }

    /*
        int close(int fd)
            - fd: 닫을 파일 디스크립터(File Descriptor)
            - return: 성공 시 0, 실패 시 -1을 반환
    */
    close(client_fd);

    return 0;
}