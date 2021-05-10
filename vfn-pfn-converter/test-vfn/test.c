#include <stdio.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <sys/ioctl.h>

#include "../query_ioctl.h"
#include <stdlib.h>     /* strtol */

// https://github.com/avsej/hashset.c
#include "hashset.h"
#include <assert.h>
#include <errno.h>

#define BUFSIZE 128
hashset_t set;

int main() {
   set  = hashset_create();
       if (set == NULL) {
        fprintf(stderr, "failed to create hashset instance\n");
        abort();
    }

   parse_output();

}
int parse_output(void) {
    char *cmd = "cat /proc/4135/maps";    

    char buf[BUFSIZE];
    FILE *fp;

    if ((fp = popen(cmd, "r")) == NULL) {
        printf("Error opening pipe!\n");
        return -1;
    }


    char *file_name = "/dev/query";
	int fd;
    fd = open(file_name, O_RDWR);   
	if (fd == -1)
	{
		perror("query_apps open");
		return 2;
	}

    while (fgets(buf, BUFSIZE, fp) != NULL) {

    	strtok(buf, "-");
        unsigned long vfn = strtol (buf,NULL, 16);

        query_arg_t q;
        q.vfn = vfn;
        q.pfn = 0;
        q.pid = 4135;

        // if (ioctl(fd, QUERY_SET_VARIABLES, &q) < 0)
        // {
        //     perror("query_apps ioctl get");
        // }
        // if(0)
        if(ioctl(fd, QUERY_SET_VARIABLES, &q) < 0)
        {
            printf("ioctl failed and returned errno %s \n",strerror(errno));
        }
        else
        {
            // char pfn_str[256];
            // sprintf(pfn_str, "%lx", q.pfn);

            // if(hashset_is_member(set, q.pfn)== 1) {
            //     printf("duplicate: pfn : %lx\n", q.pfn);
            // }
            // hashset_add(set, q.pfn);
            printf("hex: %s  vfn: %ld, pfn : %lx\n", buf, q.vfn, q.pfn);
        }

       
    }

    if(pclose(fp))  {
        printf("Command not found or exited with error status\n");
        return -1;
    }

    return 0;
}