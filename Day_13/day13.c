#include <stdio.h>
#include <sys/wait.h>
#include "../UTILS/shell_cmd.h"
#include "../UTILS/util.h"

int main() 
{
	char *cmd[] = { "intcode", "./input.txt", "-1", "1", NULL };
	process_t machine = process(cmd);
	wait(NULL);
	return 1;
}
