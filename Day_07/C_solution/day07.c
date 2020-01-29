#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include "shell_cmd.h"
#include "util.h"

int receive_engine_code(FILE *f)
{
	int code = 0, ret = 0;
	ret = fscanf(f, "%d", &code);
	if (ret != 1) die("failed reading code, aborting program.");
	return code;
}

void send_engine_code(process_t p, int code) 
{
	dprintf(p.fd_read, "%d\n", code);
}


void setup_engines(process_t *processes, int *charges, FILE *files[], int amt)
{
	char engNo[2];
	for (int i = 0; i < amt; i++) {
		snprintf(engNo, 2, "%d", i+1);
		char *cmd[] = { "./intcode", engNo, "0", NULL};
		processes[i] = process(cmd);
		dprintf(processes[i].fd_read, "%d\n", charges[i]);
		files[i] = fdopen(processes[i].fd_write, "r");
	}
}

int run_engine_loop(FILE *files[], int charges[])
{
	int startCode = 0;
	process_t ps[5];

	setup_engines(ps, charges, files, 5);
	while (waitpid(0, NULL, WNOHANG) == 0) {
		send_engine_code(ps[0], startCode);
		send_engine_code(ps[1], receive_engine_code(files[0]));
		send_engine_code(ps[2], receive_engine_code(files[1]));
		send_engine_code(ps[3], receive_engine_code(files[2]));
		send_engine_code(ps[4], receive_engine_code(files[3]));
		startCode = receive_engine_code(files[4]);
	}
	wait(NULL);
	return startCode;
}

int main (int argc, char *argv[]) {
	if (argc < 2) die("Engine numbers required!");

	FILE *files[5]; int in[5];
	for (int i = 0; i < 5; i++) {
		in[i] = (int) argv[1][i] - (int) '0';
	}
	printf("%d\n", run_engine_loop(files, in));
	return 0;
}
