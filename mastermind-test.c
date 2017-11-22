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
	if(read(mm_fd, testbuff, 11)==-1)
		goto read_err;
	printf("%s\n", testbuff);

	printf("User starts game\n");
	if(write(mm_ctl_fd, "start", 5)==-1)
		goto write_err;
	memset(testbuff, '\0', 80);
	if(read(mm_fd, testbuff, 13)==-1)
		goto read_err;
	printf("mm: %s\n", testbuff);

	printf("User enters 0000\n");
	if(write(mm_fd, "0000", 4)==-1)
		goto write_err;
	memset(testbuff, '\0', 80);
	if(read(mm_fd, testbuff, 40)==-1)
		goto read_err;
	printf("User enters 1100\n");
	if(write(mm_fd, "1100", 4)==-1)
		goto write_err;
	printf("mm: %s\n", testbuff);
	printf("User enters 0011\n");
	if(write(mm_fd, "0011", 4)==-1)
		goto write_err;
	printf("User enters 001234\n");
	if(write(mm_fd, "001234", 6)==-1)
		goto write_err;

	printf("The mem map:\n");
	char * start = NULL;
    start = mmap(start, PAGE_SIZE, PROT_READ, MAP_PRIVATE, mm_fd, 0);
    printf("%.*s", (int)PAGE_SIZE, start);

    printf("User quits\n");
	if(write(mm_ctl_fd, "quit", 4)==-1)
		goto write_err;
	memset(testbuff, '\0', 80);
	if(read(mm_fd, testbuff, 28)==-1)
		goto read_err;
	printf("mm: %s\n", testbuff);

	printf("The mem map:\n");
	start = NULL;
    start = mmap(start, PAGE_SIZE, PROT_READ, MAP_PRIVATE, mm_fd, 0);
    printf("%.*s", (int)PAGE_SIZE, start);

	close(mm_fd);
	close(mm_ctl_fd);

	return 0;

	write_err:
		printf("There was an error writing to device.\n");
		return 1;
	read_err:
		printf("There was an error reading from device.\n");
		return 1;
}
