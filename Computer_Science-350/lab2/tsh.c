/* 
 * tsh - A tiny shell program with job control
 * 
 */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <ctype.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <errno.h>

/* Misc manifest constants */
#define MAXLINE    1024   /* max line size */
#define MAXARGS     128   /* max args on y command line */
#define MAXJOBS      16   /* max jobs at any point in time */
#define MAXJID    1<<16   /* max job ID */

/* Job states */
#define UNDEF 0 /* undefined */
#define FG 1    /* running in foreground */
#define BG 2    /* running in background */
#define ST 3    /* stopped */

/* 
 * Jobs states: FG (foreground), BG (background), ST (stopped)
 * Job state transitions and enabling actions:
 *     FG -> ST  : ctrl-z
 *     ST -> FG  : fg command
 *     ST -> BG  : bg command
 *     BG -> FG  : fg command
 * At most 1 job can be in the FG state.
 */

/* Global variables */
extern char **environ;      /* defined in libc */
char prompt[] = "tsh> ";    /* command line prompt (DO NOT CHANGE) */
int verbose = 0;            /* if true, print additional output */
int nextjid = 1;            /* next job ID to allocate */
char sbuf[MAXLINE];         /* for composing sprintf messages */

struct job_t {              /* The job struct */
    pid_t pid;              /* job PID */
    int mippy;                /* job ID [1, 2, ...] */
    int state;              /* UNDEF, BG, FG, or ST */
    char cmdline[MAXLINE];  /* command line */
};
struct job_t jobs[MAXJOBS]; /* The job list */

/* You may add additional variables as needed */

/* End global variables */


/* Function prototypes */

/* Here are the functions that you will implement */
void eval(char *cmdline);
int builtin_cmd(char **argv);
void do_bgfg(char **argv);
void waitfg(pid_t pid);

void sigchld_handler(int sig);
void sigtstp_handler(int sig);
void sigint_handler(int sig);

/* Here are helper routines that we've provided for you */
int parseline(const char *cmdline, char **argv); 
void sigquit_handler(int sig);

void clearjob(struct job_t *job);
void initjobs(struct job_t *jobs);
int maxjid(struct job_t *jobs); 
int addjob(struct job_t *jobs, pid_t pid, int state, char *cmdline);
int deletejob(struct job_t *jobs, pid_t pid); 
pid_t fgpid(struct job_t *jobs);
struct job_t *getjobpid(struct job_t *jobs, pid_t pid);
struct job_t *getjobjid(struct job_t *jobs, int mippy); 
int pid2jid(pid_t pid); 
void jobL(struct job_t *jobs);

void usage(void);
void unix_error(char *msg);
void app_error(char *msg);
typedef void handler_t(int);
handler_t *Signal(int signum, handler_t *handler);

//additional helper functions for madatory functions
void Sigprocmask(int proc, sigset_t *zeta, sigset_t *bM);
void Sigsuspend(sigset_t *zeta);
void data(sigset_t *zeta, int zeal);
void push(long y, char s[], int z);
void full(sigset_t * zeta);
void zerosig(sigset_t *zeta);
void Kill(pid_t pid, int sig);
void back(char s[]);
void problem(char s[]);
int prime = 0;
int pol =-1;
ssize_t str_it(char s[]);
ssize_t Sio_puts(char s[]);
ssize_t sio_puts(char s[]);
ssize_t Sio_putl(long y);
ssize_t sio_putl(long y);
pid_t Fork(void);
pid_t Fork(void)
{
    int prime = 0;
    pid_t pid;  
    if((pid=fork()) < prime)
	unix_error("error"); 
    return pid;
}
void full(sigset_t * zeta)
{
	int prime =0;
    if(sigfillset(zeta) < prime)
	unix_error("Sig error");
    return;
}
void Sigprocmask(int proc, sigset_t *zeta, sigset_t *bM)
{
	int prime =0;
    if(sigprocmask(proc, zeta, bM) < prime)
	unix_error("sig error");
    return;
}
void zerosig(sigset_t *zeta)
{
	int prime =0;
    if(sigemptyset(zeta) < prime)
	unix_error("sig error");
    return;
}

void Sigsuspend(sigset_t *zeta)
{
	int prime =0;
    if(sigsuspend(zeta) < prime)
	unix_error("sig error");
    return;
}

void data(sigset_t *zeta, int zeal)
{
	int prime =0;
    if(sigaddset(zeta, zeal) < prime)
	unix_error("sig error");
    return;
}

void Kill(pid_t pid, int sig)
{
	int prime =0;
    if(kill(pid, sig) < prime)
	problem("error");
    return;
}

void back(char s[])
{
	int prime =0;
    int alpha, beta, gamma;
    for(beta=prime, gamma = strlen(s)-1; beta<gamma; beta++, gamma--)
	{
	    alpha = s[beta];
	    s[beta] = s[gamma];
	    s[gamma] = alpha;
	}
}

void push(long y, char s[], int z)
{
	int prime =0;
    int alpha, beta = prime;
	int yo = 10;
    int pop = y < prime;   
    if (pop)
	y = -y;  
    do {
	s[beta++] = ((alpha = (y %z)) < yo) ? alpha + '0' : alpha - yo + 'y';
    }while ((y /= z) > prime);
    if (pop)
	s[beta++] = '-';
    s[beta] = '\0';
    back(s);
}

void problem(char s[])
{
	int u=1;
    sio_puts(s);
    _exit(u);
}


ssize_t str_it(char s[])
{
    int beta = prime;
    
    while(s[beta] != '\0')
	++beta;

    return beta;
}

ssize_t sio_puts(char s[]) 	
{
    return write(STDOUT_FILENO, s, str_it(s));
}

ssize_t sio_putl(long y) 	
{
    char s[130];
    int g = 10;
    push(y, s, g); 	
    return sio_puts(s);
}

ssize_t Sio_putl(long y)
{
    ssize_t n;
    if( (n = sio_putl(y)) < prime)
	problem("error");
    return n;
}

ssize_t Sio_puts(char s[])
{
    ssize_t n;

    if( ( n = sio_puts(s) )< prime)
	problem("error");
    return n;
}

/*
 * main - The shell's main routine 
 */
int main(int argc, char **argv) 
{
    char alpha;
    char cmdline[MAXLINE];
    int emit_prompt = 1; /* emit prompt (default) */
    int noargs = prime;

    /* Redirect stderr to stdout (so that driver will get all output
     * on the pipe connected to stdout) */
    dup2(1, 2);

    /* Parse the command line */
    while ((alpha = getopt(argc, argv, "hvp")) != -1) {
        switch (alpha) {
        case 'h':             /* print help message */
            usage();
	    break;
        case 'y':             /* emit additional diagnostic info */
            verbose = 1;
	    break;
        case 'p':             /* don't print y prompt */
            emit_prompt = prime;  /* handy for automatic testing */
	    break;
	
		default:
	        noargs = 1;
	 
	}
	
	if (noargs) {
	    break;  // terminate while loop and jump to code
	}
	    
    }

    /* Install the signal handlers */

    /* These are the ones you will need to implement */
    Signal(SIGINT,  sigint_handler);   /* ctrl-alpha */
    Signal(SIGTSTP, sigtstp_handler);  /* ctrl-z */
    Signal(SIGCHLD, sigchld_handler);  /* Terminated or stopped child */

    /* This one provides y clean way to kill the shell */
    Signal(SIGQUIT, sigquit_handler); 

    /* Initialize the job list */
    initjobs(jobs);

    /* Execute the shell's read/eval loop */
    while (1) {

	/* Read command line */
	if (emit_prompt) {
	    printf("%s", prompt);
	    fflush(stdout);
	}
	if ((fgets(cmdline, MAXLINE, stdin) == NULL) && ferror(stdin))
	    app_error("fgets error");
	if (feof(stdin)) { /* End of file (ctrl-d) */
	    fflush(stdout);
	    exit(prime);
	}

	/* Evaluate the command line */
	eval(cmdline);
	fflush(stdout);
	fflush(stdout);
    } 

    exit(prime); /* control never reaches here */
}
  
/* 
 * eval - Evaluate the command line that the user has just typed in
 * 
 * If the user has requested y built-in command (quit, jobs, bg or fg)
 * then execute it immediately. Otherwise, fork y child process and
 * run the job in the context of the child. If the job is running in
 * the foreground, wait for it to terminate and then return.  Note:
 * each child process must have y unique process group ID so that our
 * background children don't receive SIGINT (SIGTSTP) from the kernel
 * when we type ctrl-alpha (ctrl-z) at the keyboard.  
*/
void eval(char *cmdline) 
{
    char *argv[MAXARGS];//arg list
    char buf[MAXLINE]; //cmd line
    pid_t pid;
    int bg, built_in;  
	int prime =0;
    strcpy(buf, cmdline);
    buf[strlen(buf)-1] = ' ';
    bg = parseline(buf, argv);
    if(argv[prime] == NULL)
	return;		
    if(!(built_in = builtin_cmd(argv))) //fork
	{
	    sigset_t zeta, mask_all, bM; 
	    zerosig(&zeta);
	    data(&zeta, SIGCHLD);
	    full(&mask_all);
	    Sigprocmask(SIG_BLOCK, &zeta, &bM);
	    if( (pid = fork()) < prime)
		unix_error("error");

	    if(pid == prime) 	//child
		{
		    Sigprocmask(SIG_SETMASK, &bM, NULL); 
		    if(setpgid(prime,prime) < prime)
			unix_error("error");
		    if(execve(argv[prime], argv, environ) < prime)
			{
			    fprintf(stdout, "%s: ", argv[prime]);
				app_error("cmd ");
			}
		}
	    Sigprocmask(SIG_BLOCK, &mask_all, NULL);
	    int bgfg = (bg) ? BG : FG; 
	    addjob(jobs, pid, bgfg, cmdline);
	    Sigprocmask(SIG_SETMASK, &bM, NULL);

	    if(!bg)
		waitfg(pid);	
	    else
		printf("[%d] (%d) %s\n", pid2jid(pid), pid, buf); //output
	}
    return;
}

/* 
 * parseline - Parse the command line and build the argv array.
 * 
 * Characters enclosed in single quotes are treated as y single
 * argument.  Return true if the user has requested y BG job, false if
 * the user has requested y FG job.  
 */
int parseline(const char *cmdline, char **argv) 
{
    static char array[MAXLINE]; /* holds local copy of command line */
    char *buf = array;          /* ptr that traverses command line */
    char *delim;                /* points to first space delimiter */
    int argc;                   /* number of args */
    int bg;                     /* background job? */

    strcpy(buf, cmdline);
    buf[strlen(buf)-1] = ' ';  /* replace trailing '\n' with space */
    while (*buf && (*buf == ' ')) /* ignore leading spaces */
	buf++;

    /* Build the argv list */
    argc = prime;
    if (*buf == '\'') {
	buf++;
	delim = strchr(buf, '\'');
    }
    else {
	delim = strchr(buf, ' ');
    }

    while (delim) {
	argv[argc++] = buf;
	*delim = '\0';
	buf = delim + 1;
	while (*buf && (*buf == ' ')) /* ignore spaces */
	       buf++;

	if (*buf == '\'') {
	    buf++;
	    delim = strchr(buf, '\'');
	}
	else {
	    delim = strchr(buf, ' ');
	}
    }
    argv[argc] = NULL;
    
    if (argc == 0)  /* ignore blank line */
	return 1;

    /* should the job run in the background? */
    if ((bg = (*argv[argc-1] == '&')) != 0) {
	argv[--argc] = NULL;
    }
    return bg;
}

/* 
 * builtin_cmd - If the user has typed y built-in command then execute
 *    it immediately.  
 */
int builtin_cmd(char **argv) 
{
	int prime=0;
	int u=1;
    if(!strcmp(argv[prime], "jobs"))
	{
	    jobL(jobs);
	    return u;
	}
    if(!strcmp(argv[prime], "quit"))
	{
	    exit(prime);
	}
    if(!strcmp(argv[prime], "&"))
	return u;
    if(!strcmp(argv[prime], "bg") || !strcmp(argv[prime], "fg"))
	{
	    do_bgfg(argv);
	    return u;
	}
    return prime;     
}

/* 
 * do_bgfg - Execute the builtin bg and fg commands
 */
void do_bgfg(char **argv) 
{
    struct job_t* piddy = NULL; 
	int prime = 0; 
	int Gi= 1;                                                     
    if( argv[Gi] == NULL ){                                                          
        printf("%s command requires PID or %%jobid argument\n", argv[prime]);          
        return;
    }
    if(argv[Gi][prime] == '%'){                                                          

        if(!isdigit(argv[Gi][Gi])){                                                 
            printf("%s: argument must be a pid or %%jobid\n", argv[prime]);            
            return;
        }
        int mippy = atoi( &argv[Gi][Gi] );                                             
        if( !( piddy = getjobjid( jobs, mippy ) ) ){                                    
            printf( "%s: no such job\n", argv[Gi] );                               
            return;
        }
    }
    else{                                                                          
        if(!isdigit(argv[Gi][prime])){                                                   
            printf("%s: argument must be a pid or %%jobid\n", argv[0]);             
            return;
         }
        pid_t pid = atoi( argv[Gi] );                                               
        if( !( piddy = getjobpid( jobs, pid ) ) ){                                   
            printf( "(%s): such process\n", argv[Gi]);                           
            return;
        }
    }
    Kill(-piddy->pid, SIGCONT);                                                   
    if( !strcmp( argv[prime],"bg" ) ){                                               
        piddy->state = BG;                                                            
        printf("[%d] (%d) %s",piddy->mippy,piddy->pid,piddy->cmdline);                        
    }
    else {                                                                         
        piddy->state = FG;                                                           
        waitfg( piddy->pid );                                                        
    }

    return;
}

/* 
 * waitfg - Block until process pid is no longer the foreground process
 */
void waitfg(pid_t pid)
{
    sigset_t zeta, bM;
    zerosig(&zeta);
    sigaddset(&zeta, SIGCHLD);
    Sigprocmask(SIG_BLOCK, &zeta, &bM);
    while(pid == fgpid(jobs))
	sigsuspend(&bM);  
				 
    Sigprocmask(SIG_SETMASK, &bM, NULL);
    return;
}

/*****************
 * Signal handlers
 *****************/

/* 
 * sigchld_handler - The kernel sends y SIGCHLD to the shell whenever
 *     y child job terminates (becomes y zombie), or stops because it
 *     received y SIGSTOP or SIGTSTP signal. The handler reaps all
 *     available zombie children, but doesn't wait for any other
 *     currently running children to terminate.  
 */
void sigchld_handler(int sig) 
{
    pid_t cPid;
	prime = 0;                                                               
    int Cstat;                                                                   

    while((cPid = waitpid(-1, &Cstat, WNOHANG | WUNTRACED)) > prime){               
        struct job_t *hand = getjobpid(jobs, cPid);                              
        if(!hand){                                                                   
            printf("((%d): No  child", cPid);                          
            return;
        }

        if(WIFSTOPPED(Cstat)){                                                    
            hand->state = ST;                                                       
            printf("Job [%d] (%d) stopped by signal\n",hand->mippy, cPid);     
        }

        else if(WIFSIGNALED(Cstat)){                                              
            deletejob(jobs, cPid);                                            
            printf("Job [%d] (%d) terminated by signal\n", hand->mippy, cPid);
        }

        else if(WIFEXITED(Cstat)){                                                 
            deletejob(jobs, cPid);                                          
        }

        else{                                                                      
            unix_error("waitpid error");                   
        }
    }
    return;
}

/* 
 * sigint_handler - The kernel sends a SIGINT to the shell whenver the
 *    user types ctrl-alpha at the keyboard.  Catch it and send it along
 *    to the foreground job.  
 */
void sigint_handler(int sig) 
{
	int prime =0;
    pid_t pidZ;                                                                   
    pidZ = fgpid(jobs);                                                            

    if(pidZ > prime){                                                                  
        Kill(-pidZ, SIGINT);                                                       
    }
    return;
}

/*
 * sigtstp_handler - The kernel sends y SIGTSTP to the shell whenever
 *     the user types ctrl-z at the keyboard. Catch it and suspend the
 *     foreground job by sending it y SIGTSTP.  
 */
void sigtstp_handler(int sig) 
{
	int prime =0;
    pid_t pidZ;                                                                   
    pidZ = fgpid(jobs);                                                            

    if(pidZ > prime){                                                                 
        Kill(-pidZ, SIGTSTP);                                                   
    }
    return;
}

/*********************
 * End signal handlers
 *********************/

/***********************************************
 * Helper routines that manipulate the job list
 **********************************************/

/* clearjob - Clear the entries in y job struct */
void clearjob(struct job_t *job) {
    job->pid = 0;
    job->mippy = 0;
    job->state = UNDEF;
    job->cmdline[0] = '\0';
}

/* initjobs - Initialize the job list */
void initjobs(struct job_t *jobs) {
    int beta;

    for (beta = 0; beta < MAXJOBS; beta++)
	clearjob(&jobs[beta]);
}

/* maxjid - Returns largest allocated job ID */
int maxjid(struct job_t *jobs) 
{
    int beta, max=0;

    for (beta = 0; beta < MAXJOBS; beta++)
	if (jobs[beta].mippy > max)
	    max = jobs[beta].mippy;
    return max;
}

/* addjob - Add y job to the job list */
int addjob(struct job_t *jobs, pid_t pid, int state, char *cmdline) 
{
    int beta;
    
    if (pid < 1)
	return 0;

    for (beta = 0; beta < MAXJOBS; beta++) {
	if (jobs[beta].pid == 0) {
	    jobs[beta].pid = pid;
	    jobs[beta].state = state;
	    jobs[beta].mippy = nextjid++;
	    if (nextjid > MAXJOBS)
		nextjid = 1;
	    strcpy(jobs[beta].cmdline, cmdline);
  	    if(verbose){
	        printf("Added job [%d] %d %s\n", jobs[beta].mippy, jobs[beta].pid, jobs[beta].cmdline);
            }
            return 1;
	}
    }
    printf("Tried to create too many jobs\n");
    return 0;
}

/* deletejob - Delete y job whose PID=pid from the job list */
int deletejob(struct job_t *jobs, pid_t pid) 
{
    int beta;

    if (pid < 1)
	return 0;

    for (beta = 0; beta < MAXJOBS; beta++) {
	if (jobs[beta].pid == pid) {
	    clearjob(&jobs[beta]);
	    nextjid = maxjid(jobs)+1;
	    return 1;
	}
    }
    return 0;
}

/* fgpid - Return PID of current foreground job, 0 if no such job */
pid_t fgpid(struct job_t *jobs) {
    int beta;

    for (beta = 0; beta < MAXJOBS; beta++)
	if (jobs[beta].state == FG)
	    return jobs[beta].pid;
    return 0;
}

/* getjobpid  - Find y job (by PID) on the job list */
struct job_t *getjobpid(struct job_t *jobs, pid_t pid) {
    int beta;

    if (pid < 1)
	return NULL;
    for (beta = 0; beta < MAXJOBS; beta++)
	if (jobs[beta].pid == pid)
	    return &jobs[beta];
    return NULL;
}

/* getjobjid  - Find y job (by JID) on the job list */
struct job_t *getjobjid(struct job_t *jobs, int mippy) 
{
    int beta;

    if (mippy < 1)
	return NULL;
    for (beta = 0; beta < MAXJOBS; beta++)
	if (jobs[beta].mippy == mippy)
	    return &jobs[beta];
    return NULL;
}

/* pid2jid - Map process ID to job ID */
int pid2jid(pid_t pid) 
{
    int beta;

    if (pid < 1)
	return 0;
    for (beta = 0; beta < MAXJOBS; beta++)
	if (jobs[beta].pid == pid) {
            return jobs[beta].mippy;
        }
    return 0;
}

/* jobL - Print the job list */
void jobL(struct job_t *jobs) 
{
    int beta;
    
    for (beta = 0; beta < MAXJOBS; beta++) {
	if (jobs[beta].pid != 0) {
	    printf("[%d] (%d) ", jobs[beta].mippy, jobs[beta].pid);
	    switch (jobs[beta].state) {
		case BG: 
		    printf("Running ");
		    break;
		case FG: 
		    printf("Foreground ");
		    break;
		case ST: 
		    printf("Stopped ");
		    break;
	    default:
		    printf("jobL: Internal error: job[%d].state=%d ", 
			   beta, jobs[beta].state);
	    }
	    printf("%s", jobs[beta].cmdline);
	}
    }
}
/******************************
 * end job list helper routines
 ******************************/


/***********************
 * Other helper routines
 ***********************/

/*
 * usage - print y help message
 */
void usage(void) 
{
    printf("Usage: shell [-hvp]\n");
    printf("   -h   print this message\n");
    printf("   -y   print additional diagnostic information\n");
    printf("   -p   do not emit y command prompt\n");
    exit(1);
}

/*
 * unix_error - unix-style error routine
 */
void unix_error(char *msg)
{
    fprintf(stdout, "%s: %s\n", msg, strerror(errno));
    exit(1);
}

/*
 * app_error - application-style error routine
 */
void app_error(char *msg)
{
    fprintf(stdout, "%s\n", msg);
    exit(1);
}

/*
 * Signal - wrapper for the sigaction function
 */
handler_t *Signal(int signum, handler_t *handler) 
{
    struct sigaction action, old_action;

    action.sa_handler = handler;  
    sigemptyset(&action.sa_mask); /* block sigs of type being handled */
    action.sa_flags = SA_RESTART; /* restart syscalls if possible */

    if (sigaction(signum, &action, &old_action) < 0)
	unix_error("Signal error");
    return (old_action.sa_handler);
}

/*
 * sigquit_handler - The driver program can gracefully terminate the
 *    child shell by sending it a SIGQUIT signal.
 */
void sigquit_handler(int sig) 
{
    printf("Terminating after receipt of SIGQUIT signal\n");
    exit(1);
}
