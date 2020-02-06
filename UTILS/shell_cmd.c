#include <string.h>
#include <unistd.h>
#include <stdio.h>
#include <fcntl.h>
#include <dirent.h>
#include <errno.h>
#include "shell_cmd.h"

int get_num_fds()
{
	int fd_count;
	char buf[64];
	struct dirent *dp;
	snprintf(buf, 64, "/proc/%i/fd/", getpid());
	fd_count = 0;
	DIR *dir = opendir(buf);
	while ((dp = readdir(dir)) != NULL) {
		fd_count++;
	}
	closedir(dir);
	return fd_count;
}

process_t process(char *cmd[]) 
{
	int pipe_write[2], pipe_read[2];
	pipe(pipe_write);
	pipe(pipe_read);
	// process_t gets passed to parent, so we want access to 
	// the READ end of the pipe that child writes to, and
	// the WRITE end of the pipe that child reads from
	process_t process = { fork(), pipe_write[READ], pipe_read[WRITE] };
	if (process.pid == 0) { // child process
		dup2(pipe_write[WRITE], STDOUT_FILENO);
		close(pipe_write[READ]); close(pipe_write[WRITE]);
		dup2(pipe_read[READ], STDIN_FILENO);
		close(pipe_read[READ]); close(pipe_read[WRITE]);
		// close any FD's that might be from earlier forked programs, those shouldn't be reachable here
		int max_fd = get_num_fds();
		for (int fd = 4; fd < max_fd; fd++) {
			if (fcntl(fd, F_GETFL) == -1 && errno == EBADF) continue;
			else close(fd);
		}
		execvp(cmd[0], cmd);
	} 
	// these ends aren't necessary in parent, close them
	close(pipe_write[WRITE]); 
	close(pipe_read[READ]); 
	return process;
}
