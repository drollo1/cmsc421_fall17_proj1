Project 1: Mastermind Kernel Module
This project is due on Tuesday, November 21, at 11:59:59 PM (Eastern standard time). You must use submit to turn in your project like so: submit cs421_jtang proj1 mastermind.c mastermind-test.c

Your module code must be named mastermind.c, and it will be compiled against a 4.12 Linux kernel source tree, via the Kbuild supplied below. It must not have any compilation warnings; warnings will result in grading penalties. This module code must be properly indented and have a file header comment, as described on the coding conventions page. Prior to submission, use the kernel's indentation script to reformat your code, like so:

    ~/linux/scripts/Lindent mastermind.c
  
In addition, you will write a unit test program, mastermind-test.c, and it will be compiled on Ubuntu 16.04 as follows:

  gcc ‐‐std=c99 ‐Wall ‐O2 ‐pthread ‐o mastermind-test mastermind-test.c ‐lm
There must not be any compilation warnings in your submission; warnings will result in grading penalties. In addition, this code must also have a file header comment and be properly indented. You will submit this test code along with your module code.
Mastermind is a code breaking game invented in the early 1970s, based upon an earlier game called Bulls and Cows. In both games, the codemaker selects a 4 digit code, where each digit ranges from 0 through 5, inclusive. The code can have repeating digits. The opposing player, the codebreaker, then tries to guess the code. For each guess, the codemaker marks which digit is correct in both value and position (a black peg) and which is simply correct in value (a white peg). There are many variations of this game playable online; the one closest to the spirit of this assignment uses colors instead of digits.

In this project, you will write a Linux kernel module that implements the basics of Mastermind. The Linux kernel itself will take the role of the codemaker, while human users will be codebreakers. Your module will create two miscellaneous devices, /dev/mm and /dev/mm_ctl. The user controls the game state by writing to /dev/mm_ctl, then make guesses by writing to /dev/mm. The module calculates the number of black and white pegs, and generates a status message. The user retrieves the status message by reading from /dev/mm, then continues guessing. The user can also obtain a history of the current game, showing all guesses and responses, by opening a memory map to /dev/mm.

Part 1: Compile Mastermind Module
All instructions henceforth assume you successfully completed the first homework. If you have not done so, go back and finish the homework before proceeding. You have been warned.
To begin, create a directory for your project and download the following files into that directory via wget:

http://www.csee.umbc.edu/~jtang/cs421.f17/homework/proj1/mastermind.c
Skeleton code for your Mastermind kernel module.
http://www.csee.umbc.edu/~jtang/cs421.f17/homework/proj1/mastermind-test.c
Skeleton code for your unit test code.
http://www.csee.umbc.edu/~jtang/cs421.f17/homework/proj1/Kbuild
Read by Linux kernel's build system, defines what is being built. You do not need to modify this file, nor should you submit it with your work.
http://www.csee.umbc.edu/~jtang/cs421.f17/homework/proj1/Makefile
Builds the kernel module and unit test program, by simply running make. Also included is a clean target to remove all built objects. You do not need to modify this file, nor should you submit it with your work.
Now run make to compile everything. You will get some warnings about unused symbols; by the end of this project you will have used all of them. You should now have the kernel module mastermind.ko. Load that module like so:

    sudo insmod mastermind.ko (enter your password as necessary)
The module was inserted if the following returns a non-empty string:
    lsmod | grep mastermind
So far, all this module does is write a message to the kernel's ring buffer. View the module messages like so:

    dmesg | tail
The number at the beginning is the time stamp of when the message was written. To unload the module, run this command:
    sudo rmmod mastermind
Re-examine the ring buffer to see the message generated during module exit. Every time you make a change and recompile mastermind.c, you will need to first unload the module and then reinsert it.
When the mastermind module is loaded, the kernel calls its init function (similar to a C program's main() function), where execution begins. Currently, this module's init function calls pr_info() and allocates some memory for itself. The pr_info() function is an easy way to generate logging messages within kernel code. It accepts a format string like printf(), but has additional format specifiers useful for kernel programming.

Part 2: Create Miscellaneous Devices
You are about to make changes to your Linux kernel. There is a slight chance of accidentally erasing your virtual machine's hard drive. Create a snapshot of your virtual machine before proceeding.
Creating a custom character device can be daunting, and in this project, you will create two of them. Fortunately, the Linux kernel has the miscellaneous devices subsystem to simplify this task. For this project, the mastermind module will use miscdevice to control /dev/mm and /dev/mm_ctl.

Start off by examining mastermind.c, specifically the stub functions mm_read() and mm_write(), and the prewritten function mm_mmap(). Follow these steps to create the device /dev/mm:

Create a global variable of type static const struct file_operations to handle /dev/mm. Set its read callback to mm_read, write callback to mm_write, and mmap callback to mm_mmap.
Create a global variable of type static struct miscdevice for /dev/mm. Set its minor field to MISC_DYNAMIC_MINOR, name field to "mm", fops field to point to the previously created struct file_operations, and mode callback to 0666.
In mastermind_init(), call misc_register() to create the character device. In mastermind_exit() call misc_deregister() to undo the registration.
When modifying mastermind_init(), you are responsible for freeing all resources in case of an error. Specifically, mastermind_init() should be structured with goto statements to clean up resources.
If all of the above works, when the module is loaded, you will now have a character device /dev/mm:

    $ sudo insmod mastermind.ko
    $ ls -l /dev/mm
    crw-rw-rw- 1 root root 10, 56 Oct 28 18:38 /dev/mm
    $ echo -n 'Hi there' > /dev/mm
    bash: echo: write error: Operation not permitted
  
If implemented incorrectly, your kernel ring buffer may contain a message that looks like this:
    [27656.939815] Oops: 0000 [#1] SMP
    [27656.939815] Modules linked in: mastermind(O+) ntfs msdos vboxsf(O) snd_intel8x0 snd_ac97_codec ac97_bus
    vboxvideo(O) snd_pcm drm_kms_helper snd_seq_oss snd_seq_midi_event syscopyarea snd_seq sysfillrect snd_seq_
    device sysimgblt snd_timer fb_sys_fops ttm snd drm soundcore vboxguest(O) serio_raw autofs4 hid_generic usb
    hid hid psmouse ohci_pci e1000 ohci_hcd [last unloaded: mastermind]
    [27656.939815] CPU: 3 PID: 3624 Comm: insmod Tainted: G           O    4.12.9+ #3
    [27656.939815] Hardware name: innotek GmbH VirtualBox/VirtualBox, BIOS VirtualBox 12/01/2006
    [27656.939815] task: ffff9d41e5d4e040 task.stack: ffffa47200ce8000
    [27656.939815] RIP: 0010:misc_register+0xd/0x170
    [27656.939815] RSP: 0018:ffffa47200cebc80 EFLAGS: 00010246
    [27656.939815] RAX: ffffffffc03a8420 RBX: ffffffffc03a80c0 RCX: 0000000000000000
    [27656.939815] RDX: 0000000000000000 RSI: ffffffffc03a70cf RDI: 0000000000000000
    [27656.939815] RBP: ffffa47200cebc98 R08: ffffa47200c60000 R09: 00003fffffe00000
    [27656.939815] R10: ffff9d421625aa90 R11: ffffa47200c5ffff R12: 0000000000000018
    [27656.939815] R13: 0000000000000000 R14: ffffffffc03a8110 R15: 0000000000000001
    [27656.939815] FS:  00007f49009cc700(0000) GS:ffff9d421fd80000(0000) knlGS:0000000000000000
    [27656.939815] CS:  0010 DS: 0000 ES: 0000 CR0: 0000000080050033
    [27656.940466] CR2: 0000000000000000 CR3: 000000016ea8b000 CR4: 00000000000406a0
    [27656.940484] Call Trace:
    [27656.940494]  ? 0xffffffffc024e000
    [27656.940505]  mastermind_init+0x59/0x1000 [mastermind]
    [27656.940522]  do_one_initcall+0x4e/0x180
  
Read the call trace to determine where within your code caused the fault that the Linux kernel had detected. Most errors are unrecoverable; a symptom of an unrecoverable condition is when the kernel refuses to unload the module:
    $ sudo rmmod mastermind
    rmmod: ERROR: Module mastermind is in use
  
In this case, your only recourse is to reboot the virtual machine.
After creating the /dev/mm miscellaneous device, similarly create the /dev/mm_ctl device. This second device has only one callback, write, which should be set to mm_ctl_write. As before, ensure that all calls within mastermind_init() are checked and resources are released upon error.

Part 3: Implement Game Control and Status Message
The next step is to implement the callback mm_ctl_write(). Read its comments. This function needs to parse the user's input. If the user writes start to /dev/mm_ctl, then start a new game:

Set the target code to 0012.
Set the number of guesses made so far to 0.
Clear the contents of user_view.
Set game_active to true.
Set the game status message.
Be aware that mm_ctl_write() starts a game when the user enters start, not start plus a newline character.
If instead the user enters quit (again, without the trailing newline), then set game_active to false and the game status message with the target code.

When constructing game statuses, use the Linux kernel-specific function scnprintf(). This function prevents common buffer overflow problems inherent to snprintf().

Now implement mm_read(). This function simply copies the game status to the user. It is responsible for handling partial reads and non-zero offsets.

Recompile and reinsert your module. You should now be able to minimally interact with your game:

    $ sudo insmod mastermind.ko
    $ cat /dev/mm
    No game yet
    $ echo "start" > /dev/mm_ctl
    bash: echo: write error: Invalid argument
    $ echo -n "start" > /dev/mm_ctl
    $ cat /dev/mm
    Starting game
    $ echo -n "quit" > /dev/mm_ctl
    $ cat /dev/mm
    Game over. The code was 0012.
  
Note how the above passes the ‐n flag to echo to suppress the trailing newline.
Part 4: Implement Game Play
The next function to implement is mm_write(). Parse the user's input as per the function's comments:

Handle up to the first NUM_PEGS characters. Convert the ASCII characters in the guess to their numeric equivalent.
Calculate the number of black pegs, based upon guessed digits, that have both the correct value and correct position.
Calculate the number of white pegs, based upon guessed digits, that are the correct value but incorrect position. Duplicate guesses of the same digit are bounded by the number of actual digits. For example, if the user guesses 1153, then this function only calculates one white peg.
Increment the number of guesses made.
Update the game status with the number of black and white pegs.
Append to user_view a string (null-terminated) with the guessed digits, number of black pegs, and number of white pegs.
Recompile and reinsert your module. You should now be able to play the game:

    $ sudo insmod mastermind.ko
    $ echo -n "start" > /dev/mm_ctl
    $ echo -n "0022" > /dev/mm
    $ cat /dev/mm
    Guess 1: 3 black peg(s), 0 white peg(s)
    $ echo -n "2010" > /dev/mm
    $ cat /dev/mm
    Guess 2: 2 black peg(s), 2 white peg(s)
    $ echo -n "5432" > /dev/mm
    $ cat /dev/mm
    Guess 3: 1 black peg(s), 0 white peg(s)
    $ echo -n "I WIN" > /dev/mm
    bash: echo: write error: Invalid argument
    $ echo -n "0012" > /dev/mm
    $ cat /dev/mm
    Guess 4: 4 black peg(s), 0 white peg(s)
  
Write for yourself a minimal C program that creates a read-only memory map to /dev/mm. Use PAGE_SIZE as the number of bytes to map. PAGE_SIZE is defined in the header <sys/user.h>. For the above sequence of guesses, the memory map contents should thus be:

    Guess 1: 0022  | B3 W0
    Guess 2: 2010  | B2 W2
    Guess 3: 5432  | B1 W0
    Guess 4: 0012  | B4 W0
  
Part 5: Unit Tests and Documentation
Now that you have (in theory) a fully working module, you must then write your own unit tests. Modify mastermind-test.c to open /dev/mm and /dev/mm_ctl. Again, create a read-only memory mapping to /dev/mm. This program is to exercise all of the functionality as described above. This includes a mix of inputs when writing to the device nodes, reading and verifying the contents of the memory map, and verifying the calculations of black and white pegs are correct.

You will need to create your own testing framework; as a suggestion, reuse the one employed in homework 4. The unit tests must have comments that explain what things are being tested. Your goal is to test boundary conditions of your miscellaneous devices; you will be graded based upon the thoroughness of the tests. For example, you are responsible for checking that user cannot read past the length of the game status.

As that your tests will perform multiple reads and writes from the devices, you will probably need to reposition your file pointer after each operation.

Other Hints and Notes
Ask plenty of questions on Blackboard.
At the top of your submission, list any help you received as well as web pages you consulted. Please do not use any URL shorteners, such as goo.gl or tinyurl.
Use the Linux Cross-Reference website to quickly search through kernel source code.
You may modify any of the provided code. You may need to add more functions and global variables than those provided.
If you dynamically allocate any additional memory, you are responsible for freeing them during module unload.
Make sure you indent your code one last time prior to submission. Unindented kernel code will result in grading penalties.
Extra Credit
Normally, the kernel module always picks the same target code of 0012. This does not make for an interesting game after the first play. You may earn an additional 10% on this assignment by randomly selecting the target code. Add the following features to your code:

Add a kernel module parameter, random_code, that defaults to false.
If random_code is true, then randomly select 4 digits.
Add a command line parameter to mastermind-test.c that, if specified, tests that the target code really is randomized.
If you choose to perform this extra credit, put a comment at the top of your file, alerting the grader.