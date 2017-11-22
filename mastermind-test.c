/* YOUR FILE-HEADER COMMENT HERE */

#include <fcntl.h>
#include <pthread.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/user.h>

#define mm_path "/dev/mm"
#define mm_ctl_path "/dev/mm_ctl"

int main(void) {
	printf("Hello, world!\n");



	return 0;
}
