/*
 * R Jesse Chaney
 * chaneyr@eecs.orst.edu
 * CS344-001
 * Oregon State University
 * School of EECS
 */

/*
 * $Author: chaneyr $
 * $Date: 2015/03/02 08:28:30 $
 * $RCSfile: hostdb.h,v $
 * $Revision: 1.6 $
 */


#ifndef _HOSTDB_H_
# define _HOSTDB_H_

// I don't think I'd load anywhere near this number, but
// you never know...
# define MAX_ROWS 100

// This could probably be a bit smaller.
# define NAME_SIZE 75

# define SHARED_MEM_PERMISSIONS (S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH)
# define PSHARED_THREADED     0
# define PSHARED_MULTIPROCESS 1

// This is what I use for one row in the shared memory
// database.
typedef struct host_row_s {
  int in_use;
  sem_t host_lock;
  char host_name[NAME_SIZE];
  char host_address_ipv4[NAME_SIZE]; // IPv4 address name
  char host_address_ipv6[NAME_SIZE]; // IPv6 address name
  time_t time_when_fetched;
} host_row_t;

// A unique name for each user's shared memory object.
# define SHARED_MEM_BASE "/HOSTDB"
# define SHARED_MEM_NAME(_BUF_) sprintf(_BUF_,"%s__%s" \
		   , SHARED_MEM_BASE, getenv("LOGNAME"));

// The default prompt.
# define PROMPT "--> "

// The commands.
# define CMD_HELP         "help"          // Show the list of commands.
# define CMD_EXIT         "exit"          // Exit the application.
# define CMD_INSERT       "insert "       // Insert a new hostname/ip-address
# define CMD_UPDATE       "update "       // Refresh addresses for a host
# define CMD_SELECT       "select"        // Show a host or all hosts.
# define CMD_DELETE       "delete"        // Remove a host or all hosts from db.
# define CMD_COUNT        "count"         // Count the number of active rows in db - EXTRA CREDIT

# define CMD_SAVE         "save "         // Save hostnames to file.
# define CMD_LOAD         "load "         // Load hostnames from file.

# define CMD_DROP_DB      "drop_database" // Delete the entire database (aka unlink).

# define CMD_LOCK_DB      "lock_db"       // Lock the entire database.
# define CMD_UNLOCK_DB    "unlock_db"     // Unlock the database.
# define CMD_LOCK_ROW     "lock_row "     // Lock a specific host
# define CMD_UNLOCK_ROW   "unlock_row"    // Unlock a locked host.
# define CMD_LOCKS        "locks"         // Show all locks - EXTRA CREDIT

#endif // _HOSTDB_H_
