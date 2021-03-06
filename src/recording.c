#include <stdbool.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

static void record(int fd);

static void play(int fd);

int main(int argc, char **argv) {
    int pipe_fds[2];
    pid_t pid;

    pipe(pipe_fds);
    pid = fork();

    if (pid == 0) {
        close(pipe_fds[1]);
        play(pipe_fds[0]);
    } else if (pid > 0) {
        close(pipe_fds[0]);
        record(pipe_fds[1]);
    }
    return EXIT_SUCCESS;
}

static void record(int fd) {
    char *cmd = "arecord -";
    char buf[256];
    FILE *fp = popen(cmd, "r");

    while (true) {
        size_t result;

        result = fread(buf, 1, sizeof(buf), fp);
        write(fd, buf, result);
    }
    pclose(fp);
}


static void play(int fd) {
    char *cmd = "aplay -";
    char buf[256];
    FILE *fp = popen(cmd, "w");

    while (true) {
        ssize_t nread;
        nread = read(fd, buf, sizeof(buf));
        printf("%zu\n", nread);

        if (nread > 0) {
            fwrite(buf, 1, sizeof(buf), fp);
        } else {
            break;
        }
    }
    pclose(fp);
}
