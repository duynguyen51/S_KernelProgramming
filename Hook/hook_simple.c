/* 	DISCLAMER 
	These following code refer by " github.com/GreenCandlePrinter/syscall-hook "
	We use it to get to know about hook
*/


#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/errno.h>
#include <linux/types.h>
#include <linux/unistd.h>
#include <asm/current.h>
#include <linux/sched.h>
#include <linux/syscalls.h>
#include <linux/delay.h>
#include <asm/paravirt.h>
#include <linux/string.h>
#include <linux/fdtable.h>

#define DRIVER_AUTHOR "Phan H.T, Nguyen H.T.D"
#define DRIVER_DESC "Hooking to syscall"
#define MAX_SIZE 1000

// The prototype for the nesscessary functions
unsigned long	**sys_call_table;

asmlinkage int ( *original_write ) ( unsigned int, const char __user *, size_t );
asmlinkage int ( *original_read ) ( unsigned int, const char __user *, size_t );
asmlinkage int ( *original_open ) ( const char __user *, int, mode_t );

/*
	This following function : new_write -> use for detect new Task call for writing
*/
asmlinkage int	new_write( unsigned int fd, const char __user *buf, size_t count ) {
   char *tmp;
   char *pathname;
   struct file *file;
   struct path *path;
   spin_lock(&current->files->file_lock);
   file = fcheck_files(current->files, fd);
   if (!file) {
      spin_unlock(&current->files->file_lock);
      return -ENOENT;
   }

   path = &file->f_path;
   path_get(path);
   spin_unlock(&current->files->file_lock);


   tmp = (char *)__get_free_page(GFP_KERNEL);

   if (!tmp) {
      path_put(path);
      return -ENOMEM;
   }

   pathname = d_path(path, tmp, PAGE_SIZE);
   path_put(path);

   /* do something here with pathname */

   int bytes = ( *original_write )( fd, buf, count );

   if (strstr(pathname,"output.txt") != NULL) {
      printk( KERN_EMERG "WRITE - file name: %s\n", pathname );
      printk( KERN_EMERG "WRITE - bytes written: %d\n", bytes);
   }
   free_page((unsigned long)tmp);
   return bytes;
}

/*
	This following function : new_read -> use for detect new Task call for read file
*/
asmlinkage int	new_read( unsigned int fd, const char __user *buf, size_t count ) {

   return ( *original_read )( fd, buf, count );
}

/*
	This following function : new_open -> use for detect new Task call for open file
*/
asmlinkage int	new_open( const char __user * pathname, int flags, mode_t mode ) {
   if (strstr(pathname,"output") != NULL) {
      printk( KERN_EMERG "Opened: %s\n", pathname );
      printk( KERN_EMERG "Opened: %s\n", current->comm);
   }
   //printk( KERN_EMERG "opened ===> %s\n", pathname );
   return ( *original_open )( pathname, flags, mode );
}

/*
	This function is used for get syscall table via iterator PAGE_OFFSET
*/
static void	aquire_sys_call_table( void ) {
   unsigned long int	offset;

   for ( offset = PAGE_OFFSET; offset < ULLONG_MAX; offset += sizeof( void * )) {
      sys_call_table = ( unsigned long ** ) offset;
      if (sys_call_table[ __NR_close ] == ( unsigned long * ) sys_close)
      break ;
   }
   printk( KERN_EMERG "Syscall Table Address: %p\n", sys_call_table );
}

static void	allow_writing( void ) {
   write_cr0( read_cr0() & ~0x10000 );
}

static void	disallow_writing( void ) {
   write_cr0( read_cr0() | 0x10000 );
}

static int init_mod( void ) {
   aquire_sys_call_table();

   original_write = ( void * ) sys_call_table[ __NR_write ];
   original_read = ( void * ) sys_call_table[ __NR_read ];
   original_open = ( void * ) sys_call_table[ __NR_open ];
   allow_writing();
   sys_call_table[ __NR_write ] = ( unsigned long * ) new_write;
   sys_call_table[ __NR_read ] = ( unsigned long * ) new_read;
   sys_call_table[ __NR_open ] = ( unsigned long * ) new_open;
   disallow_writing();

   return 0;
}

static void	exit_mod( void ) {
   allow_writing();
   sys_call_table[ __NR_write ] = ( unsigned long * ) original_write;
   sys_call_table[ __NR_read ] = ( unsigned long * ) original_read;
   sys_call_table[ __NR_open ] = ( unsigned long * ) original_open;
   disallow_writing();
}

module_init(init_mod);
module_exit(exit_mod);

MODULE_LICENSE("GPL");
MODULE_AUTHOR(DRIVER_AUTHOR);
MODULE_DESCRIPTION(DRIVER_DESC);
