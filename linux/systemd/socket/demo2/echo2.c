#include <unistd.h>
#include <syslog.h>

int main() {
    openlog("echo2", LOG_PID | LOG_CONS, LOG_DAEMON);
    char buf[1024];
    ssize_t n;

    // 直接从 stdin 读，写到 stdout
    while ((n = read(STDIN_FILENO, buf, sizeof(buf))) > 0) {
        buf[n] = '\0';
        write(STDOUT_FILENO, buf, n);
        syslog(LOG_INFO, "Received: %s", buf);
    }

    // 处理完自动退出
    return 0;
}
