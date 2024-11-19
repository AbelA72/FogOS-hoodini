#include "types.h"
#include "riscv.h"
#include "defs.h"
#include "param.h"
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

int
name_of_inode(struct inode *ip, struct inode *parent, char buf[DIRSIZ]) {
    uint off;
    struct dirent de;
    for (off = 0; off < parent->size; off += sizeof(de)) {
        if (readi(parent, (char*)&de, off, sizeof(de)) != sizeof(de))
            panic("couldn't read dir entry");
        if (de.inum == ip->inum) {
            safestrcpy(buf, de.name, DIRSIZ);
            return 0;
        }
    }
    return -1;
}

int
name_for_inode(char* buf, int n, struct inode *ip) {
    int path_offset;
    struct inode *parent;
    char node_name[DIRSIZ];
    if (ip->inum == namei("/")->inum) { //namei is inefficient but iget isn't exported for some reason
        buf[0] = '/';
        return 1;
    } else if (ip->type == T_DIR) {
        parent = dirlookup(ip, "..", 0);
        ilock(parent);
        if (name_of_inode(ip, parent, node_name)) {
            panic("could not find name of inode in parent!");
        }
        path_offset = name_for_inode(buf, n, parent);
        safestrcpy(buf + path_offset, node_name, n - path_offset);
        path_offset += strlen(node_name);
        if (path_offset == n - 1) {
            buf[path_offset] = '\0';
            return n;
        } else {
            buf[path_offset++] = '/';
        }
        iput(parent); //free
        return path_offset;
    } else if (ip->type == T_DEV || ip->type == T_FILE) {
        panic("process cwd is a device node / file, not a directory!");
    } else {
        panic("unknown inode type");
    }
}

int
sys_getcwd(void)
{
    char *p;
    int n;
    if(argint(1, &n) < 0 || argptr(0, &p, n) < 0)
        return -1;
    return name_for_inode(p, n, proc->cwd);
}
