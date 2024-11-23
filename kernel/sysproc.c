#include "types.h"
#include "riscv.h"
#include "param.h"
#include "defs.h"
#include "memlayout.h"
#include "spinlock.h"
#include "proc.h"

uint64
sys_exit(void)
{
  int n;
  argint(0, &n);
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
  argaddr(0, &p);
  return wait(p);
}

uint64
sys_sbrk(void)
{
  uint64 addr;
  int n;

  argint(0, &n);
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


  argint(0, &n);
  acquire(&tickslock);
  ticks0 = ticks;
  while(ticks - ticks0 < n){
    if(killed(myproc())){
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
sys_pgaccess(void)
{
  // lab pgtbl: your code here.
  uint64 virtual_addr, user_addr;
  int pg_nums;
  uint64 bitmask = 0;

  argaddr(0, &virtual_addr);
  argint(1, &pg_nums);
  argaddr(2, &user_addr);

  for(int i=0; i<pg_nums; i++){
    uint64 pg_addr = virtual_addr + i*PGSIZE;

    pte_t* pte = walk(myproc()->pagetable, pg_addr, 0);

    if (!pte || !(*pte & PTE_V)) {
      continue;  // Bỏ qua nếu PTE không hợp lệ
    }

    if (*pte & PTE_A){ // Kiểm tra access bit có được bật hay không?
      bitmask = bitmask | (1 << i); // Bật bit thứ i trong bitmask để đánh dấu trang này đã được truy cập
      *pte = *pte & (~PTE_A); // Tắt bit PTE_A
    }
  }

  if (copyout(myproc()->pagetable, user_addr, (char*)&bitmask, sizeof(bitmask)) < 0) {
    return -1; 
  }

  return 0;
}


#endif

uint64
sys_kill(void)
{
  int pid;

  argint(0, &pid);
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
