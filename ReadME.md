# KERNEL PROGRAMMING
In this project, it is requested that creating simple Linux kernel and Hooking Module

## Requirement: 
	Linux core with Kernel version is lower than 4.x.x-xx (We've no idea how to perform in linux kernel 5.x.x)
## How to use:
  **1. Random Number Module**
  - Firstly, we need direct Terminal cursor to the Folder that contain module (here inside the parent folder, we named RandomNumberModule)
  ```bash
  cd ~/RandomNumberModule
  ```
  - Creating *.ko file to load to Kernel by typing following 
  ```bash
  make
  ```
  - Load the random_module.ko into kernel by 
  ```bash
  sudo insmod random_module.ko
  ```
  After load into kernel, checking if loaded by 
  ```bash
  lsmod | grep random_module
  ```
  The result will show the Module if it exist
  - Testing the module 
  First, compile test_module.c and run it, type these command respectively
  ```bash
  cc test_module.c -o r_test
  ./r_test
  ```
  **2. Catching Syscall Open and Write**
  - Firstly, we need direct Terminal cursor to the Folder that contain module (here inside the parent folder, we named Hook)
  ```bash
  cd ~/Hook
  ```
  - Creating *.ko file to load to Kernel by typing following 
  ```bash
  make
  ```
  - Load the hook_simple.ko into kernel by 
  ```bash
  sudo insmod hook_simple.ko
  ```
  After load into kernel, checking if loaded by 
  ```bash
  lsmod | grep hook_simple
  ```
  Before testing this module, open dmesg in second terminal by these command to have a view of result
  ```bash
  dmesg -C (This will clear the whole old record)
  dmesg -wH (Show the message respectively)
  ```
  The result will show the Module if it exist
  - Testing the module 
  First, compile test_module.c and run it, type these command respectively
  ```bash
  cc checking.c -o r_checking
  ./r_checking
  ```
  Make an observation in Second Terminal for result
  