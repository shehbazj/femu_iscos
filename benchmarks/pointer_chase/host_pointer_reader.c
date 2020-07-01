// write_pointer_to_disk.c

#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <error.h>
#include <errno.h>
#include <unistd.h>
#define BLOCK_SIZE 4096

int main(int argc, char *argv[])
{
	FILE* head_pointer_list_fd;
	int disk_fd_read;
	off_t disk_off;

	int current_block, next_block;
	int head_pointer;

	int ret;

	if (argc < 2) {
		printf("Usage ./host_pointer_reader <disk_name>\n");
		exit(0);
	}

	head_pointer_list_fd = fopen("hp.dat", "r");
	if (head_pointer_list_fd == NULL) {
		printf("hp.dat open error: %s\n", strerror(errno));
		exit(1);
	}

	disk_fd_read = open(argv[1], O_RDONLY);
	if (disk_fd_read < 0) {
		printf("disk open error: %s\n", strerror(errno));
		exit(1);
	}

	while (fscanf(head_pointer_list_fd, "%d", &head_pointer) != EOF) {
		printf("Parsing LL with head %d\n", head_pointer);
		current_block = head_pointer;
		while(current_block != 0) {
			disk_off = lseek(disk_fd_read, current_block * BLOCK_SIZE, SEEK_SET);
			ret = read(disk_fd_read, &next_block, sizeof(next_block));
			current_block = next_block;
			printf("Next PTR = %d\n", current_block);
		}
	}

	close(disk_fd_read);
	fclose(head_pointer_list_fd);
}
