#include "kernel/types.h"
#include "user/user.h"

static const int size_int = sizeof(int);

void prime(int* pipe_fd){

    close(pipe_fd[1]);

    int num = 0;
    if( size_int != read(pipe_fd[0],&num,size_int) ){
        return ;
    }

    fprintf(1,"prime %d \n",num);

    int pipe_next[2];
    if( -1 == pipe(pipe_next) ){
        fprintf(2,"create pipe fail...\n");
        exit(1) ;
    }

    int fork_next = fork();
    if( -1 == fork_next ){
        fprintf(2,"fork fail...\n");
        exit(1);
    }

    if( 0 == fork_next){
        prime(pipe_next);
        exit(0);
    }

    close(pipe_next[0]);
    int temp = 0;
    while( read(pipe_fd[0],&temp,size_int) == size_int){
        if( (temp % num) != 0){
            write(pipe_next[1],&temp,size_int);
        }
    }
    close(pipe_next[1]);
    wait(0);
}

int main(int argc, char *argv[]){

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

    if( 0 == fork_pid ){
        prime(pipe_fd);
        exit(0);
    }

    int limiter = 35;
    if( 2 == argc ){
        limiter = atoi(argv[1]);
    }

    close(pipe_fd[0]);
    for( int num = 2; num <= limiter; ++num){
        write(pipe_fd[1],&num,size_int);
    }

    close(pipe_fd[1]);
    wait(0);
    exit(0);
}
