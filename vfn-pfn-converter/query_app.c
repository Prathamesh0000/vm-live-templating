#include <stdio.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <sys/ioctl.h>

#include "query_ioctl.h"
#include <stdlib.h>     /* strtol */
void get_vars(int fd)
{
	query_arg_t q;
	q.pfn = 0;
	if (ioctl(fd, QUERY_GET_VARIABLES, &q) == -1)
	{
		perror("query_apps ioctl get");
	}
	else
	{
		printf("pfn    : %lx\n", q.pfn);
	}
}
void clr_vars(int fd)
{
	if (ioctl(fd, QUERY_CLR_VARIABLES) == -1)
	{
		perror("query_apps ioctl clr");
	}
}
void set_vars(int fd)
{
	int v;
	query_arg_t q;

	printf("Enter processid: ");
	scanf("%d", &v);
	getchar();
	q.pid = v;
	printf("Enter vfn: ");

	char *line = NULL;
	size_t len = 0;
	ssize_t lineSize = 0;
	lineSize = getline(&line, &len, stdin);
	char * pEnd;
	
	q.vfn =  strtol (line,NULL,0);
	q.pfn = 0;
	// q.pid = 3896;
	// q.vfn = 140652526358528;
	// 7f52c6273000 1006dbf 1 1 0 1 /usr/lib/locale/locale-archive
	if (ioctl(fd, QUERY_SET_VARIABLES, &q) == -1)
	{
		perror("query_apps ioctl set");
	}

		// q.vfn = v;
	printf("pid    : %d\n", q.pid);
	printf("vfn    : %lx\n", q.vfn);
	printf("pfn    : %lx\n", q.pfn);
}

int main(int argc, char *argv[])
{
	char *file_name = "/dev/query";
	int fd;
	enum
	{
		e_get,
		e_clr,
		e_set
	} option;

	if (argc == 1)
	{
		option = e_get;
	}
	else if (argc == 2)
	{
		if (strcmp(argv[1], "-g") == 0)
		{
			option = e_get;
		}
		else if (strcmp(argv[1], "-c") == 0)
		{
			option = e_clr;
		}
		else if (strcmp(argv[1], "-s") == 0)
		{
			option = e_set;
		}
		else
		{
			fprintf(stderr, "Usage: %s [-g | -c | -s]\n", argv[0]);
			return 1;
		}
	}
	else
	{
		fprintf(stderr, "Usage: %s [-g | -c | -s]\n", argv[0]);
		return 1;
	}
	fd = open(file_name, O_RDWR);
	if (fd == -1)
	{
		perror("query_apps open");
		return 2;
	}

	switch (option)
	{
		case e_get:
			get_vars(fd);
			break;
		case e_clr:
			clr_vars(fd);
			break;
		case e_set:
			set_vars(fd);
			break;
		default:
			break;
	}

	close (fd);

	return 0;
}
