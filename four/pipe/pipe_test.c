#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>

#define BUF_SIZE 100

int main(int argc, char *argv[])
{
    int fd[2];
    char buf[BUF_SIZE];
    pid_t pid;
    pid_t ret;
    int len;
    int status;

    if(pipe(fd) < 0)
    {
        perror("failed to pipe");
        exit(1);
    }

    pid = fork();

    if(pid < 0)
    {
        perror("failed to fork");
        exit(1);
    }

    if(pid == 0)
    {
        //scanf("%s", buf);
        //execlp("ls", "ls", "-al", "/");
        execlp(argv[1], argv[1], argv[2], argv[3]);
        dup2(fd[1], 1);
        close(fd[0]);
        //write(fd[1], buf, BUF_SIZE);

    }
    else
    {
        ret = waitpid(pid, &status, WNOHANG);
        close(fd[1]);
        read(fd[0], buf, BUF_SIZE);
        printf("%s", buf);
    }
    return 0;
}
