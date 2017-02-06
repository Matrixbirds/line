#ifndef __SIGNALS__
#define __SIGNALS__

#include <signal.h>

#define SIG "SIG"
#define signal_inspect(TYPE) SIG##TYPE

#define SERVER_TERMINATE signal_inspect(INT)
//#define SERVER_SIGUSR1
//#define SERVER_SIGUSR2

#endif
