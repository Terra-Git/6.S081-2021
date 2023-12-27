#include "types.h"
#include "riscv.h"
#include "param.h"
#include "defs.h"
#include "date.h"
#include "memlayout.h"
#include "spinlock.h"
#include "proc.h"

uint64
sys_exit(void)
{
  int n;
  if(argint(0, &n) < 0)
    return -1;
  exit(n);
  return 0;  // not reached
}

uint64
sys_getpid(void)
{
  return myproc()->pid;
}

uint64
sys_fork(void)
{
  return fork();
}

uint64
sys_wait(void)
{
  uint64 p;
  if(argaddr(0, &p) < 0)
    return -1;
  return wait(p);
}

uint64
sys_sbrk(void)
{
  int addr;
  int n;

  if(argint(0, &n) < 0)
    return -1;
  
  addr = myproc()->sz;
  if(growproc(n) < 0)
    return -1;
  return addr;
}

uint64
sys_sleep(void)
{
  int n;
  uint ticks0;


  if(argint(0, &n) < 0)
    return -1;
  acquire(&tickslock);
  ticks0 = ticks;
  while(ticks - ticks0 < n){
    if(myproc()->killed){
      release(&tickslock);
      return -1;
    }
    sleep(&ticks, &tickslock);
  }
  release(&tickslock);
  return 0;
}


#ifdef LAB_PGTBL
int 
pgaccess(pagetable_t pagetable, uint64 page_start, int page_num, uint64 result){

  if ( page_num < 0 || 64 < page_num )
    return -1;

  uint64 mask = 0;
  uint64 begin = page_start;
  
  pte_t* pet = 0;
  for( int index = 0; index < page_num; index++){
    pet = walk(pagetable, begin, sizeof(begin));
    if( 0 == pet )
      return -1;
    if( *pet & PTE_A ){
      mask |= (1 << index);
      *pet &= (~PTE_A);
    }
    begin += PGSIZE;
  }
  copyout(pagetable, result, (char*)(&mask),sizeof(mask));
  return 0;
}

int
sys_pgaccess(void)
{
  uint64 page_start;
  if( argaddr(0,&page_start) < 0)
    return -1;
  
  int page_num;
  if( argint(1,&page_num) < 0)
    return -1;

  uint64 result;
  if( argaddr(2,&result) < 0)
    return -1;
  
  struct proc *p = myproc();
  if( pgaccess(p->pagetable,page_start,page_num,result))
    return -1;

  return 0;
}
#endif

uint64
sys_kill(void)
{
  int pid;

  if(argint(0, &pid) < 0)
    return -1;
  return kill(pid);
}

// return how many clock tick interrupts have occurred
// since start.
uint64
sys_uptime(void)
{
  uint xticks;

  acquire(&tickslock);
  xticks = ticks;
  release(&tickslock);
  return xticks;
}
