#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <systemd/sd-bus.h>

// 方法回调: Echo
static int method_echo(sd_bus_message *m, void *userdata, sd_bus_error *ret_error) {
    const char *text;
    int r;

    // 读取输入参数
    r = sd_bus_message_read(m, "s", &text);
    if (r < 0) {
        fprintf(stderr, "Failed to read parameters: %s\n", strerror(-r));
        return r;
    }

    printf("Echo called with: %s\n", text);

    // 构建回复
    r = sd_bus_reply_method_return(m, "s", text);
    if (r < 0) {
        fprintf(stderr, "Failed to send reply: %s\n", strerror(-r));
        return r;
    }

    return 0;
}

// 方法回调: Add
static int method_add(sd_bus_message *m, void *userdata, sd_bus_error *ret_error) {
    int64_t a, b;
    int r;

    // 读取输入参数
    r = sd_bus_message_read(m, "xx", &a, &b);
    if (r < 0) {
        fprintf(stderr, "Failed to read parameters: %s\n", strerror(-r));
        return r;
    }

    printf("Add called with: %ld + %ld\n", a, b);

    // 构建回复
    r = sd_bus_reply_method_return(m, "x", a + b);
    if (r < 0) {
        fprintf(stderr, "Failed to send reply: %s\n", strerror(-r));
        return r;
    }

    return 0;
}

// vtable 定义
static const sd_bus_vtable calculator_vtable[] = {
    SD_BUS_VTABLE_START(0),
    SD_BUS_METHOD("Echo", "s", "s", method_echo, SD_BUS_VTABLE_UNPRIVILEGED),
    SD_BUS_METHOD("Add", "xx", "x", method_add, SD_BUS_VTABLE_UNPRIVILEGED),
    SD_BUS_VTABLE_END
};

int main(int argc, char *argv[]) {
    sd_bus_slot *slot = NULL;
    sd_bus *bus = NULL;
    int r;

    // 连接到会话总线
    /* r = sd_bus_default_system(&bus); */
    r = sd_bus_default_user(&bus);
    if (r < 0) {
        fprintf(stderr, "Failed to connect to user bus: %s\n", strerror(-r));
        goto finish;
    }

    // 注册对象路径
    r = sd_bus_add_object_vtable(
        bus,
        &slot,
        "/com/example/Calculator",
        "com.example.Calculator",
        calculator_vtable,
        NULL
    );

    if (r < 0) {
        fprintf(stderr, "Failed to register object: %s\n", strerror(-r));
        goto finish;
    }

    // 请求服务名
    r = sd_bus_request_name(bus, "com.example.Calculator", 0);
    if (r < 0) {
        fprintf(stderr, "Failed to request name: %s\n", strerror(-r));
        goto finish;
    }

    printf("Calculator service started\n");
    printf("Listening on: /com/example/Calculator\n");
    printf("Service name: com.example.Calculator\n");

    // 事件循环
    for (;;) {
        r = sd_bus_process(bus, NULL);
        if (r < 0) {
            fprintf(stderr, "Failed to process bus: %s\n", strerror(-r));
            break;
        }

        if (r == 0) {
            r = sd_bus_wait(bus, (uint64_t)-1);
            if (r < 0) {
                fprintf(stderr, "Failed to wait for bus: %s\n", strerror(-r));
                break;
            }
        }
    }

finish:
    sd_bus_slot_unref(slot);
    sd_bus_unref(bus);

    return r < 0 ? EXIT_FAILURE : EXIT_SUCCESS;
}
