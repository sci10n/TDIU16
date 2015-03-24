#include <stdio.h>
#include <syscall-nr.h>
#include "userprog/syscall.h"
#include "threads/interrupt.h"
#include "threads/thread.h"

/* header files you probably need, they are not used yet */
#include <string.h>
#include "filesys/filesys.h"
#include "filesys/file.h"
#include "threads/vaddr.h"
#include "threads/init.h"
#include "userprog/pagedir.h"
#include "userprog/process.h"
#include "devices/input.h"

static void syscall_handler (struct intr_frame *);

void
syscall_init (void) 
{
  intr_register_int (0x30, 3, INTR_ON, syscall_handler, "syscall");
}


/* This array defined the number of arguments each syscall expects.
   For example, if you want to find out the number of arguments for
   the read system call you shall write:
   
   int sys_read_arg_count = argc[ SYS_READ ];
   
   All system calls have a name such as SYS_READ defined as an enum
   type, see `lib/syscall-nr.h'. Use them instead of numbers.
 */
const int argc[] = {
  /* basic calls */
  0, 1, 1, 1, 2, 1, 1, 1, 3, 3, 2, 1, 1, 
  /* not implemented */
  2, 1,    1, 1, 2, 1, 1,
  /* extended */
  0
};

static void
syscall_handler (struct intr_frame *f) 
{
  int32_t* esp = (int32_t*)f->esp;
  //printf("LOG [DEBUG]: in syscall_handler\n");
  int sys_read_arg_count = argc[ esp[0] ];
  //printf("LOG [DEBUG]: argc = %c\n", ('0' +sys_read_arg_count));
  switch ( esp[0] /* retrive syscall number */ )
    {
    case SYS_HALT:
      printf("LOG [DEBUG]: in SYS_HALT\n");
      power_off();
      break;
    case SYS_EXIT:
      printf("LOG [DEBUG]: in SYS_EXIT\n");
      printf("LOG [DEBIG]: argument: %i\n",esp[1]);
      thread_exit();
      break;
    case SYS_READ:
      {
	//printf("LOG [DEBUG]: in SYS_READ\n");
	char *buff = (char*)esp[2];
	if(esp[1] != STDIN_FILENO)
	  {
	    // printf("LOG [DEBUG]: not STDIN_FILENO in read\n");
	    f->eax = -1;
	    break;
	  }
	int counter = 0;
	while(counter < esp[3])
	  {
	    char c = input_getc();
	    if(c == '\r')
	      c = '\n';
	    *(buff + counter) = c;
	    putbuf((char*)&c,1);
	    counter++;
	  }
	f->eax = counter;
	//printf("LOG [DEBUG]: buffer: %s \n", buff);
      }
      break;
    case SYS_WRITE:
      {
	//printf("LOG [DEBUG]: in SYS_WRITE\n");
	if(esp[1] != STDOUT_FILENO)
	  {
	    // printf("LOG [DEBUG]: not STDOUT_FILENO in write\n");
	    f->eax = -1;
	    break;
	  }
	putbuf((char*)esp[2], esp[3]);
	f->eax = esp[3];
      }
      break;
    case SYS_OPEN:
      {
	char* file = (char*)esp[1];
      }
      break;
    default:
      {
	printf ("Executed an unknown system call!\n");
      
	printf ("Stack top + 0: %d\n", esp[0]);
	printf ("Stack top + 1: %d\n", esp[1]);
      
	thread_exit ();
      }
    }
  //printf("LOG [DEBUG]: end of syscall_handler\n");
}
