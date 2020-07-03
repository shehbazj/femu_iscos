int count_bits(char *buf)
{
        int i, j;
        char c;
        int count =0;

        for (i = 0 ; i < 4096 ; i++) {
                c = buf[i];
                for (j = 0 ; j < 8 ; j++) {
                        if (c & (1 << j)) {
                                count +=  1;
                        }
                }
        }
        return count;
}

int get_disk_pointer(char *buf)
{
	int *p = buf;
	return p[0];
}
