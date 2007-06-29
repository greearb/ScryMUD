// $Id$

//
//ScryMUD Server Code
//Copyright (C) 2007  Edward Roper
//
//This program is free software; you can redistribute it and/or
//modify it under the terms of the GNU General Public License
//as published by the Free Software Foundation; either version 2
//of the License, or (at your option) any later version.
//
//This program is distributed in the hope that it will be useful,
//but WITHOUT ANY WARRANTY; without even the implied warranty of
//MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//GNU General Public License for more details.
//
//You should have received a copy of the GNU General Public License
//along with this program; if not, write to the Free Software
//Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
//
//To contact the maintainer, Edward Roper: edro+scrymud [at] wanfear.net
//

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/resource.h>

#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <fcntl.h>
#include <stdio.h>
#include <errno.h>
#include <syslog.h>

void daemonize(const char* log_name) {

    int                 fd_stdin, fd_stdout, fd_stderr;
    pid_t               pid;
    struct rlimit       rl_val;
    struct sigaction    sig_action;

    /* Sanitize the umask */
    umask(0);

    /* How many MAX FD's do we have? */
    if ( getrlimit(RLIMIT_NOFILE, &rl_val) < 0 ) {
        perror("getrlimit()");
        exit(1);
    }

    /* Fork */
    if ((pid = fork()) < 0) {
        perror("fork()");
        exit(1);
    } else if (pid != 0) { /* parent */
        exit(0);
    }

    /* Become a session leader */
    setsid();

    /* Ignore SIGHUP */
    sig_action.sa_handler = SIG_IGN;
    sigemptyset(&sig_action.sa_mask);
    sig_action.sa_flags = 0;
    if (sigaction(SIGHUP, &sig_action, NULL) < 0 ) {
        perror("sigaction()");
        exit(1);
    }

    /* Fork again to avoid reacquiring a controlling TTY on SYS-V when
     * a TTY is possibly opened in the future */
    if ((pid = fork()) < 0) {
        perror("fork()");
        exit(1);
    } else if (pid != 0) { /* parent */
        exit(0);
    }

    /* Don't hold the filesystem open - NOT SAFE FOR SCRYMUD YET
    if (chdir("/") < 0) {
        perror("chdir");
        exit(1);
    } */

    /* Close all open FDs. */
    if ( rl_val.rlim_max == RLIM_INFINITY ) {
        rl_val.rlim_max = 1024;
    }
    for (unsigned int i=0;i<rl_val.rlim_max;i++) {
        close(i);
    }

    /* Attach /dev/null to STDIN, STDOUT & STDERR */
    fd_stdin = open("/dev/null", O_RDWR);
    fd_stdout = dup(0);
    fd_stderr = dup(0);

    /* Attach to syslog */
    openlog(log_name, LOG_PID, LOG_DAEMON);

    /* Make sure STDIN, STDOUT & STDERR are sane, now that we can log */
    if ( fd_stdin != 0 || fd_stdout != 1 || fd_stderr != 2 ) {
        syslog(LOG_ERR, "bad file descriptors %d %d %d",
                fd_stdin, fd_stdout, fd_stderr);
    }

}/* daemonize() */
