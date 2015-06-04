/* Miles A. Curry
 * currymi@onid.orst.edu
 * CS344-001
 * Assignment #4 
 */

#define SERVER_CONFIRM "CONFIRMED"

static void exitCmd(void);
int cdCmd(message_t msg, int reader_fd);
int dirCmd(int reader_fd);
int pwdCmd(int reader_fd);
int homeCmd(int reader_fd);

static void *reciveMsg(void *);
int recivePID(void);
int init_posMsq(void);
static void onexit_function(void);
static void signal_handler(int signum);

typedef struct message_pid {
    pid_t pid;
} message_pid;
