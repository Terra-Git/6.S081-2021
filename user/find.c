#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fs.h"


void find(char* dir, char* file){


    char buf[512], *p;
    int fd;
    struct dirent de;
    struct stat st;

    if(strlen(dir) + 1 + DIRSIZ + 1 > sizeof buf){
        fprintf(1,"find: path too long\n");
        return ;
    }

    if((fd = open(dir, 0)) < 0){
        fprintf(2, "find: cannot open %s\n", dir);
        return;
    }

    if(fstat(fd, &st) < 0){
        fprintf(2, "find: cannot stat %s\n", dir);
        close(fd);
        return;
    }

    strcpy(buf, dir);
    p = buf+strlen(buf);
    *p++ = '/';

    while(read(fd, &de, sizeof(de)) == sizeof(de)){
        
        if(de.inum == 0){
            continue;
        }
        if (!strcmp(de.name, ".") || !strcmp(de.name, ".."))
        {
            continue;
        }

        memmove(p, de.name, DIRSIZ);
        p[DIRSIZ] = 0;

        if(stat(buf, &st) < 0){
            printf("find: cannot stat %s\n", buf);
            continue;
        }

        switch(st.type){
        case T_FILE:
            if (!strcmp(de.name, file))
            {
                fprintf(1,"%s\n", buf);
            }
            break;
        case T_DIR:
            find(buf,file);
            break;
        }
    }
    close(fd);
}

int main(int argc, char *argv[]){
    if( argc != 3){
        fprintf(2,"Error: find parameter error...\n");
        exit(1);
    }

    char* dir = argv[1];
    char* file = argv[2];
    find(dir,file);
    exit(0);
}