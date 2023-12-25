#include "kernel/types.h"
#include "user/user.h"

static const int PARAM_SIZE = 32;
static const int BUF_SIZE = 256;
static const int READ_SIZE = 32;

// exec(char *, char **) 二维数组表示有不同的参数
// 需要使用 read 从输出缓冲区读数据，注意 read 读到的数据不一定等于，我们要求读的大小，按其返回值来计算。
// read 到 \n 则表示当前命令的参数完整了，调用 exec 执行命令，之后再继续读其它.
// ‘ ’ 空格表示不同的参数,对应 exec 第二参数的不同 char 数组。
// 这里可以使用连续的 param_buf 来代替 char**，往其中存入不同的 index 位指针即可
int main(int argc, char *argv[])
{
  char* exec_param[PARAM_SIZE];  // exec 的参数
  
  char  param_buf[BUF_SIZE];     // exec 参数暂存
  char* buf_head = param_buf;    // buf 头,这里偏移 buffer 头，使用一整段连续的内存来作为二维数组
  int   buf_index = 0;       // 暂存索引

  char  read_buf[READ_SIZE];
  int   read_size = -1;

  int pre_param = argc - 1; // xargs 前置参数
  int param_index = pre_param;  // 索引参数

  for(int index = 0; index < pre_param; index++ ){
    exec_param[index] = argv[index + 1];
  } 

  while( 0 < (read_size = read(0,read_buf,READ_SIZE)) ){
    for( int index = 0; index < read_size; ++index){
      char cur_char = read_buf[index];
      if( cur_char == '\n' ){
        param_buf[buf_index] = 0;
        exec_param[param_index++] = buf_head;
        exec_param[param_index] = 0;
        if(0 == fork()){
            exec(argv[1],exec_param);
        }
        wait(0);
        param_index = pre_param;
        buf_index = 0;
        buf_head = param_buf;
 
      }else if( cur_char == ' ' ){
        param_buf[buf_index++] = 0;
        exec_param[param_index++] = buf_head;
        buf_head = param_buf + buf_index;
      }else{
        param_buf[buf_index++] = cur_char;
      }
    }
  }
  exit(0);
}