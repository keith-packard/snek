/*
 * Copyright 2020 Mikhail Gusarov <dottedmag@dottedmag.net>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 */

#define _GNU_SOURCE

#include "utils.h"
#include <errno.h>
#include <getopt.h>
#include <netinet/in.h>
#include <poll.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/signalfd.h>
#include <sys/socket.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <unistd.h>

#define DEFAULT_PORT 9553
#define DEFAULT_SNEK_PATH ("snek-ev3-" SNEK_VERSION)

static int
poll_noeintr(struct pollfd fds[], nfds_t nfds, int timeout)
{
	for (;;) {
		int ret = poll(fds, nfds, timeout);
		if (ret == -1 && (errno == EINTR || errno == EAGAIN))
			continue;
		return ret;
	}
}

static int
recv_noeintr(int socket, void *buffer, size_t length, int flags)
{
	for (;;) {
		int ret = recv(socket, buffer, length, flags);
		if (ret == -1 && errno == EINTR)
			continue;
		return ret;
	}
}

static int
waitpid_noeintr(pid_t pid, int *stat_loc, int options)
{
	for (;;) {
		int ret = waitpid(pid, stat_loc, options);
		if (ret == -1 && errno == EINTR)
			continue;
		return ret;
	}
}

static int
accept4_noeintr(int sockfd, struct sockaddr *addr, socklen_t *addrlen, int flags)
{
	for (;;) {
		int ret = accept4(sockfd, addr, addrlen, flags);
		if (ret == -1 && errno == EINTR)
			continue;
		return ret;
	}
}

static int
close_noeintr(int fd)
{
	for (;;) {
		int ret = close(fd);
		if (ret == -1 && errno == EINTR)
			continue;
		return ret;
	}
}

static int
dup2_noeintr(int fd, int fd2)
{
	for (;;) {
		int ret = dup2(fd, fd2);
		if (ret == -1 && errno == EINTR)
			continue;
		return ret;
	}
}

static int
setup_listen(int port)
{
	int sock = socket(PF_INET, SOCK_STREAM | SOCK_CLOEXEC, 0);
	if (sock == -1) {
		perror("socket");
		exit(1);
	}

	int val = 1;
	if (setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &val, sizeof(val)) == -1) {
		perror("setsockopt(SO_REUSEADDR)");
		exit(1);
	}

	struct sockaddr_in addr = {
		.sin_family = AF_INET,
		.sin_port = htons(port),
		.sin_addr = {INADDR_ANY},
	};

	if (bind(sock, (const struct sockaddr *) &addr, sizeof(addr)) == -1) {
		perror("bind");
		exit(1);
	}

	if (listen(sock, 5) == -1) {
		perror("listen");
		exit(1);
	}

	return sock;
}

static void
validate(struct pollfd *fd, int allowed_events, const char *name)
{
	if (fd->revents & ~allowed_events) {
		fprintf(stderr, "Unexpected event on %s: 0x%x\n", name, fd->revents);
		exit(1);
	}
}

#define FD_LISTEN 0
#define FD_CONNECTED 1
#define FD_STDIN 2
#define FD_STDOUT 3
#define FD_STDERR 4

#ifdef DEBUG
static const char *
strevs(char *buf, int i)
{
	char *p = buf;
	if (i == 0) {
		*p++ = '-';
	}
	if (i & POLLIN) {
		*p++ = 'I';
	}
	if (i & POLLPRI) {
		*p++ = 'P';
	}
	if (i & POLLOUT) {
		*p++ = 'O';
	}
	if (i & POLLERR) {
		*p++ = 'E';
	}
	if (i & POLLHUP) {
		*p++ = 'H';
	}
	*p++ = 0;
	return buf;
}

static void
print_pre_poll_state(int stdin_buf, int stdout_buf, struct pollfd *fds)
{
	if (stdin_buf == EOF)
		fprintf(stderr, "[]");
	else
		fprintf(stderr, "[0x%02x]", stdin_buf);
	fprintf(stderr, " -> (snek) -> ");
	if (stdout_buf == EOF)
		fprintf(stderr, "[]");
	else
		fprintf(stderr, "[0x%02x]", stdout_buf);
	fprintf(stderr, "\n");

	char r[10];
	fprintf(stderr, "FD_LISTEN fd=%2d events=%5s\n", fds[0].fd, strevs(e, fds[0].events));
	fprintf(stderr, "FD_CONN   fd=%2d events=%5s\n", fds[1].fd, strevs(e, fds[1].events));
	fprintf(stderr, "FD_STDIN  fd=%2d events=%5s\n", fds[2].fd, strevs(e, fds[2].events));
	fprintf(stderr, "FD_STDOUT fd=%2d events=%5s\n", fds[3].fd, strevs(e, fds[3].events));
	fprintf(stderr, "FD_STDERR fd=%2d events=%5s\n", fds[4].fd, strevs(e, fds[4].events));
}

static void
print_post_poll_state()
{
	char e[10], r[10];
	fprintf(stderr, "FD_LISTEN fd=%2d events=%5s revents=%5s\n", fds[0].fd, strevs(e, fds[0].events),
		strevs(r, fds[0].revents));
	fprintf(stderr, "FD_CONN   fd=%2d events=%5s revents=%5s\n", fds[1].fd, strevs(e, fds[1].events),
		strevs(r, fds[1].revents));
	fprintf(stderr, "FD_STDIN  fd=%2d events=%5s revents=%5s\n", fds[2].fd, strevs(e, fds[2].events),
		strevs(r, fds[2].revents));
	fprintf(stderr, "FD_STDOUT fd=%2d events=%5s revents=%5s\n", fds[3].fd, strevs(e, fds[3].events),
		strevs(r, fds[3].revents));
	fprintf(stderr, "FD_STDERR fd=%2d events=%5s revents=%5s\n", fds[4].fd, strevs(e, fds[4].events),
		strevs(r, fds[4].revents));
}
#endif

static pid_t
run_snek(const char *cmd, int *subproc_stdin, int *subproc_stdout, int *subproc_stderr)
{
	int stdin_fd[2], stdout_fd[2], stderr_fd[2];
	if (pipe(stdin_fd) == -1) {
		perror("pipe");
		exit(1);
	}
	if (pipe(stdout_fd) == -1) {
		perror("pipe");
		exit(1);
	}
	if (pipe(stderr_fd) == -1) {
		perror("pipe");
		exit(1);
	}

	pid_t pid = fork();
	if (pid == -1) {
		perror("fork");
		exit(1);
	}

	if (pid == 0) {
		dup2_noeintr(stdin_fd[0], 0);

		close_noeintr(stdin_fd[0]);
		close_noeintr(stdin_fd[1]);

		dup2_noeintr(stdout_fd[1], 1);

		close_noeintr(stdout_fd[0]);
		close_noeintr(stdout_fd[1]);

		dup2_noeintr(stderr_fd[1], 2);

		close_noeintr(stderr_fd[0]);
		close_noeintr(stderr_fd[1]);

		if (execl(cmd, cmd, NULL) == -1) {
			perror("execl");
			exit(1);
		}
	}

	*subproc_stdin = stdin_fd[1];
	close_noeintr(stdin_fd[0]);
	*subproc_stdout = stdout_fd[0];
	close_noeintr(stdout_fd[1]);
	*subproc_stderr = stderr_fd[0];
	close_noeintr(stderr_fd[1]);

	return pid;
}

static void
usage(char *program, int val)
{
	fprintf(stderr, "usage: %s [--version] [--help] [--port <port>] [--snek-bin <path>]\n", program);
	exit(val);
}

static const struct option options[] = {
	{.name = "version", .has_arg = 0, .val = 'v'}, {.name = "snek-bin", .has_arg = 1, .val = 'e'},
	{.name = "port", .has_arg = 1, .val = 'p'},    {.name = "help", .has_arg = 0, .val = '?'},
	{.name = NULL, .has_arg = 0, .val = 0},
};

static void
args(int argc, char **argv, int *port, const char **snek_path)
{
	int c;
	while ((c = getopt_long(argc, argv, "p:e:", options, NULL)) != -1) {
		switch (c) {
		case 'v':
			printf("%s version %s\n", argv[0], SNEK_VERSION);
			exit(0);
		case 'p':
			*port = atoi(optarg);
			if (*port <= 0 || *port > 65535) {
				usage(argv[0], 1);
			}
			break;
		case 'e':
			*snek_path = optarg;
			break;
		case '?':
			usage(argv[0], 0);
			break;
		default:
			usage(argv[0], 1);
		}
	}
}

int
main(int argc, char **argv)
{
	int	    port = DEFAULT_PORT;
	const char *snek_path = DEFAULT_SNEK_PATH;

	args(argc, argv, &port, &snek_path);

	int listen_socket = setup_listen(port);
	int connected_socket = -1;

	int subproc_stdin, subproc_stdout, subproc_stderr;
	int subproc_pid = run_snek(snek_path, &subproc_stdin, &subproc_stdout, &subproc_stderr);

	int stdin_buf = EOF, stdout_buf = EOF;

	for (;;) {
		struct pollfd fds[] = {
			[FD_LISTEN] =
				{
					.fd = listen_socket,
					.events = POLLIN,
				},
			[FD_CONNECTED] =
				{
					.fd = connected_socket,
					.events = POLLIN | POLLOUT | POLLPRI,
				},
			[FD_STDIN] =
				{
					.fd = subproc_stdin,
					.events = 0,
				},
			[FD_STDOUT] =
				{
					.fd = subproc_stdout,
					.events = POLLIN,
				},
			[FD_STDERR] =
				{
					.fd = subproc_stderr,
					.events = POLLIN,
				},
		};

		// Do not read from socket if previous byte is not yet read by Snek
		if (stdin_buf != EOF) {
			fds[FD_CONNECTED].events &= ~POLLIN;
		}

		// Do not expect to write to socket if there is no output from Snek
		if (stdout_buf == EOF) {
			fds[FD_CONNECTED].events &= ~POLLOUT;
		}

		// Try to sink buffered data to Snek
		if (stdin_buf != EOF) {
			fds[FD_STDIN].events |= POLLOUT;
		}

		// Do not read from Snek if a client is connected and hasn't
		// read the buffered data yet
		if (stdout_buf != EOF && connected_socket != -1) {
			fds[FD_STDOUT].events &= ~POLLIN;
			fds[FD_STDERR].events &= ~POLLIN;
		}

#ifdef DEBUG
		print_pre_poll_state(stdin_buf, stdout_buf, fds);
#endif
		int ret = poll_noeintr(fds, sizeof(fds) / sizeof(fds[0]), -1);
		if (ret == -1) {
			perror("poll");
			exit(1);
		}

#ifdef DEBUG
		print_post_poll_state(fds);
#endif

		// errors

		validate(&fds[FD_LISTEN], POLLIN, "listening socket");
		validate(&fds[FD_CONNECTED], POLLIN | POLLOUT | POLLHUP | POLLERR | POLLPRI, "connected socket");
		validate(&fds[FD_STDIN], POLLOUT | POLLHUP | POLLERR, "stdin pipe");
		validate(&fds[FD_STDOUT], POLLIN | POLLHUP | POLLERR, "stdout pipe");
		validate(&fds[FD_STDERR], POLLIN | POLLHUP | POLLERR, "stderr pipe");

		// connected socket is closed

		if (fds[FD_CONNECTED].revents & (POLLHUP | POLLERR)) {
			close_noeintr(connected_socket);
			connected_socket = -1;
			stdout_buf = stdin_buf = EOF;
			continue;
		}

		// new connection, replace existing

		if (fds[FD_LISTEN].revents & POLLIN) {
			struct sockaddr_in addr;
			socklen_t	   addr_len = sizeof(addr);

			int sock = accept4_noeintr(listen_socket, (struct sockaddr *) &addr, &addr_len, SOCK_CLOEXEC);
			if (sock == -1) {
				fprintf(stderr, "Warning: unable to accept connection, dropped: %s\n", strerror(errno));
				continue;
			}

			close_noeintr(connected_socket);
			connected_socket = sock;
			stdout_buf = stdin_buf = EOF;
			continue;
		}

		// process input

		if (fds[FD_CONNECTED].revents & (POLLIN | POLLPRI)) {
			// Ctrl-C from other side is delivered as OOB
			int  oob = fds[FD_CONNECTED].revents & POLLPRI;
			char c;
			int  read_ = recv_noeintr(connected_socket, &c, 1, oob ? MSG_OOB : 0);
			if (read_ == 0) {
				close_noeintr(connected_socket);
				connected_socket = -1;
				stdout_buf = stdin_buf = EOF;
			} else if (read_ == 1) {
				if (oob) {
					kill(subproc_pid, SIGINT);
				} else {
					stdin_buf = c;
				}
			}
			continue;
		}

		if (fds[FD_STDIN].revents & POLLOUT) {
			char c = stdin_buf;
			int  written = write_noeintr(subproc_stdin, &c, 1);
			if (written == 1) {
				stdin_buf = EOF;
			}
			continue;
		}

		// process output

		if (fds[FD_CONNECTED].revents & POLLOUT) {
			char c = stdout_buf;
			if (write_noeintr(connected_socket, &c, 1) == 1) {
				stdout_buf = EOF;
			}
			continue;
		}

		if (fds[FD_STDERR].revents & POLLIN) {
			char c;
			if (read_noeintr(subproc_stderr, &c, 1) == 1) {
				if (connected_socket != -1) {
					stdout_buf = c;
				}
				// else discard, a-la serial port
			}
			continue;
		}

		if (fds[FD_STDOUT].revents & POLLIN) {
			char c;
			if (read_noeintr(subproc_stdout, &c, 1) == 1) {
				if (connected_socket != -1) {
					stdout_buf = c;
				}
				// else discard, a-la serial port
			}
			continue;
		}

		// Snek exited

		if ((fds[FD_STDIN].revents & (POLLHUP | POLLERR)) || (fds[FD_STDOUT].revents & (POLLHUP | POLLERR)) ||
		    (fds[FD_STDERR].revents & (POLLHUP | POLLERR))) {
			close_noeintr(subproc_stdin);
			close_noeintr(subproc_stdout);
			close_noeintr(subproc_stderr);

			int   status;
			pid_t pid = waitpid_noeintr(subproc_pid, &status, 0);
			if (pid == -1) {
				perror("waitpid");
				exit(1);
			}

			if (pid != subproc_pid) {
				fprintf(stderr, "unexpected pid: %d instead of %d\n", pid, subproc_pid);
				exit(1);
			}

			if (WIFEXITED(status)) {
				fprintf(stderr, "snek exited with code %d\n", WEXITSTATUS(status));
			}

			if (WIFSIGNALED(status)) {
				fprintf(stderr, "snek terminated due to signal %d%s\n", WTERMSIG(status),
					WCOREDUMP(status) ? " (core dumped)" : "");
			}
			exit(1);
		}

		// unreachable
		exit(255);
	}
}
