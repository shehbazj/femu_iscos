#include "../stream_common/common.h"
#include <stdbool.h>

static __inline__ unsigned long long rdtsc(void)
{
  unsigned hi, lo;
  __asm__ __volatile__ ("rdtsc" : "=a"(lo), "=d"(hi));
  return ( (unsigned long long)lo)|( ((unsigned long long)hi)<<32 );
}

void compress(int inputfile_fd, int fd)
{
	int err;
	int ret;
	int count = 0;
	uint8_t *buf;
	if (posix_memalign((void **)&buf, getpagesize(), IO_SEGMENT_SIZE)) {
                return;
	}

	memset(buf, 0, IO_SEGMENT_SIZE);

	printf("Opened fd from nw_thread: %d\n", fd);
	err = set_computational_stream_directive(fd, COMPRESSION_ENABLE);
	if (err<0){
		fprintf(stderr, "enable computational stream directive status:%#x(%s)\n", errno, strerror(errno));
		return;
	}else{
		printf("enable computational stream directive successful\n");
	}

	ret = read(inputfile_fd, buf, IO_TRANSFER_SIZE);
	if(ret < 0) {
		printf("read failed %s\n", strerror(errno));
		exit(1);
	}
	while (ret > 0) {
//		printf("writing data block %d bytes %d\n", count++, ret);
		err = write(fd, buf, ret);
		if (err < 0) {
			printf("compression failed\n");
			exit(1);
		}
		ret = read(inputfile_fd, buf, IO_TRANSFER_SIZE);
	}

	err = set_computational_stream_directive(fd, COMPRESSION_DISABLE);
	// disable before leaving...
	if (err<0){
		fprintf(stderr, "enable computational stream directive status:%#x(%s)\n", errno, strerror(errno));
	} else{
		printf("disable computational stream directive successful\n");
	}
	free(buf);
}

bool check_root()
{
	if (geteuid() == 0) {
		return true;
	}
	else {
		return false;
	}
}

int main(int argc, char **argv)
{
	static const char *perrstr;
	int err, fd, i;
	int inputfile_fd;
	unsigned long long start, end;
	int s_gb;

	if (argc != 2) {
		fprintf(stderr, "Usage: %s <size in GB>\n", argv[0]);
		return 1;
	}

	if (check_root() == false) {
		printf("Please run as sudo\n");
		exit(1);
	}

	sscanf(argv[1], "%d", &s_gb);

	printf("Creating IP FILE of size %d from 10G file\n", s_gb);
	char cmd[200];
	char infile[100];
#ifdef RAMDISK
	strcpy (infile, "/mnt/ramdisk/IPFILE");
#else
	strcpy (infile, "IPFILE");
#endif
	sprintf (cmd, "dd if=/home/vm/10G of=%s bs=1G count=%d", infile, s_gb);
	system(cmd);
	printf("IPFile Created\n");

	inputfile_fd = open(infile, O_RDONLY);
	if (inputfile_fd < 0) {
		goto perror;
	}
	
	fd = open("/dev/nvme0n1", O_RDWR);
	if (fd < 0) {
		goto perror;
	}
	/*
	char c;
	printf("SET CPU Limit\n");
	printf("cpulimit -p <process> -l <limit>\n");
	printf("Press key once CPU Limit has been set\n");
	scanf("%c", &c);
	*/

	err = enable_stream_directive(fd);

	if (err<0){
		fprintf(stderr, "enable stream directive status:%#x(%s)\n", errno, strerror(errno));
	}else{
		printf("enable stream directive successful\n");
	}

	err = alloc_stream_resources(fd, 1);
	if (err<0){
		fprintf(stderr, "allocate stream resource status:%#x(%s)\n", errno, strerror(errno));
	}else{
		printf("allocate stream resource successful\n");
	}

	start = rdtsc();
	compress(inputfile_fd, fd);
	end = rdtsc();

        printf("CYCLE: %llu\n",end - start);

//	testing if counting is disabled correctly.
//	computational_read(&f, 0);


   close(fd);
   close(inputfile_fd);

	
	sprintf (cmd, "rm %s", infile);
	system(cmd);
	return 0;

perror:
   close(fd);
	perror(perrstr);
	return 1;
}
