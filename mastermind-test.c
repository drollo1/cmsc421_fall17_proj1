/* YOUR FILE-HEADER COMMENT HERE */

#include <fcntl.h>
#include <pthread.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/user.h>
#include <string.h>

#define mm_path "/dev/mm"
#define mm_ctl_path "/dev/mm_ctl"

int main(void) {
	printf("Hello, world!\n");

	int mm_fd = open(mm_path, O_RDWR);
	if(mm_fd==-1){
		fprintf(stderr, "Cannot open %s. Try again later.", mm_path);
		return 1;
	}
	int mm_ctl_fd= open(mm_ctl_path, O_WRONLY);
	if(mm_ctl_fd==-1){
		fprintf(stderr, "Cannot open %s. Try again later.", mm_ctl_path);
		return 1;
	}

	char testbuff[80];
	memset(testbuff, '\0', 80);
	read(mm_fd, testbuff, 80);
	printf("The game says: %s\n", testbuff);

	write(mm_ctl_fd, "start", 5);

	memset(testbuff, '\0', 80);
	read(mm_fd, testbuff, 80);
	printf("The game says: %s\n", testbuff);

	return 0;
	
}
