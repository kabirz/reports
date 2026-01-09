// echo-activated.c
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <systemd/sd-daemon.h>

#define BUFSIZE 1024

int main() {
    int n_fds = sd_listen_fds(0);
    if (n_fds <= 0) {
        fprintf(stderr, "Not started by systemd socket activation.\n");
        return EXIT_FAILURE;
    }

    // systemd 只传递一个 socket，所以使用 fd = SD_LISTEN_FDS_START (=3)
    int listen_fd = SD_LISTEN_FDS_START;

    // 允许地址重用（可选）
    int opt = 1;
    setsockopt(listen_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    printf("Echo service started via socket activation. Listening on inherited fd %d...\n", listen_fd);
    fflush(stdout);

    for (;;) {
        struct sockaddr_in client_addr;
        socklen_t client_len = sizeof(client_addr);
        int client_fd = accept(listen_fd, (struct sockaddr *)&client_addr, &client_len);
        if (client_fd == -1) {
            perror("accept");
            continue;
        }

        char buffer[BUFSIZE];
        ssize_t n = recv(client_fd, buffer, sizeof(buffer) - 1, 0);
        if (n > 0) {
            buffer[n] = '\0';
            printf("Received: %s\n", buffer);
            fflush(stdout);
            send(client_fd, buffer, n, 0); // echo back
        }
        close(client_fd);
    }

    return EXIT_SUCCESS;
}
