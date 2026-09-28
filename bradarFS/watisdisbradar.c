#include <stdlib.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdio.h>

int main(int argc,char *argv[]) {

	char cmd[200];
	struct stat st;
	if(stat(argv[1], &st) != 0) {
		fprintf(stderr, "pak you %s\n",argv[1]);
		return 1;
	}
	execlp("stat", "stat","--printf",
		   "File:     %n\n"
		   "Type:     %F\n"
		   "Size:     %s bytes\n"
		   "Perms:    %A (%a)\n"
		   "Owner:    %U:%G\n"
		   "Modified: %y\n",
		   "--", argv[1], (char *)NULL);

	snprintf(cmd,sizeof(cmd),"stat %s",argv[1]);
	system(cmd);

	return 0;
}
