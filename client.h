/* Miles A. Curry
 * currymi@onid.orst.edu
 * CS344-001
 * Assignment #5 - Client.h
 */

#define SERVER_CONFIRM "CONFIRMED"

/* RAD-ASS FLAG STRUCT */
typedef struct flags_s{ 
    unsigned int flag_v : 1;    // If 1 produces verbose output
}flags_t;



// *** FUNCTION PROTOTYPES *** //
/* SIGNAL HANDLERS */
static void signal_handler(int signum);
static void onexit_function(void);


/* CLIENTS COMMANDS */
int exitCmd(void);
int lpwd(void);
int lcd(void);
int ldir(void);
int lhome(void);
int help(void);
int put(void);
int get(void);


/* CREATE SOCKET */
int connectIP(int port, char ip[200]);
static void shell(void);
void sendMsg(void);
void showHelp(void);

