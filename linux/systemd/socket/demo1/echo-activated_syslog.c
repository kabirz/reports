#include <errno.h>
#include <syslog.h>
#include <poll.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <systemd/sd-daemon.h>

#define BUFSIZE 1024
#define IDLE_TIMEOUT_SEC 30  // 空闲 30 秒后退出

int main() {
    openlog("echo-activated", LOG_PID | LOG_CONS, LOG_DAEMON);
    int n_fds = sd_listen_fds(0);
    if (n_fds <= 0) {
        syslog(LOG_ERR, "Not started by systemd socket activation.");
        return EXIT_FAILURE;
    }

    // systemd 只传递一个 socket，所以使用 fd = SD_LISTEN_FDS_START (=3)
    int listen_fd = SD_LISTEN_FDS_START;

    // 允许地址重用（可选）
    int opt = 1;
    setsockopt(listen_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    syslog(LOG_INFO, "Echo service started via socket activation. Listening on inherited fd %d...", listen_fd);

    for (;;) {
        struct pollfd pfd = {
            .fd = listen_fd,
            .events = POLLIN
        };

        // poll() 会阻塞最多 IDLE_TIMEOUT_SEC 秒
        int ret = poll(&pfd, 1, IDLE_TIMEOUT_SEC * 1000); // 单位：毫秒

        if (ret == -1) {
            if (errno == EINTR) continue; // 被信号中断，重试
            syslog(LOG_ERR, "poll() failed: %m");
            break;
        }

        if (ret == 0) {
            // 超时！无连接到达
            syslog(LOG_INFO, "Idle timeout reached, exiting.");
            break;
        }

        struct sockaddr_in client_addr;
        socklen_t client_len = sizeof(client_addr);
        int client_fd = accept(listen_fd, (struct sockaddr *)&client_addr, &client_len);
        if (client_fd == -1) {
            syslog(LOG_ERR, "accept failed: %m");
            continue;
        }

        char buffer[BUFSIZE];
        ssize_t n = recv(client_fd, buffer, sizeof(buffer) - 1, 0);
        if (n > 0) {
            buffer[n] = '\0';
            syslog(LOG_INFO, "Received: %s", buffer);
            send(client_fd, buffer, n, 0); // echo back
        }
        close(client_fd);
    }

    return EXIT_SUCCESS;
}
