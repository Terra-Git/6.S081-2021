#include "kernel/types.h"
#include "user/user.h"

int main(int argc, char *argv[]){

    char buf;

    int pipe_fd[2];
    if( -1 == pipe(pipe_fd) ){
        fprintf(2,"create pipe fail...\n");
        exit(1);
    }

    int fork_pid = fork();
    if( -1 == fork_pid ){
        fprintf(2,"fork fail...\n");
        exit(1);
    }
     
    if( 0 < fork_pid ){
        close(pipe_fd[0]);
        char ch = 'a';
        write(pipe_fd[1],&ch,sizeof(ch));
        close(pipe_fd[1]);

        wait(0);
        read(pipe_fd[0],&buf,sizeof(buf));
        fprintf(1,"%d: received pong\n",getpid());
        close(pipe_fd[0]);
    }
    // 子进程
    if( 0 == fork_pid){
        close(pipe_fd[1]);
        read(pipe_fd[0],&buf,sizeof(buf));
        fprintf(1,"%d: received ping\n",getpid());
        close(pipe_fd[1]);

        char ch = 'b';
        write(pipe_fd[1],&ch,sizeof(ch));
        close(pipe_fd[1]);
    }
    exit(0);
}