#include <time.h>

/* NOTE: get rid of the 2 void*
 * find a protocol to send the data over the network 
 * efficiently 
 */

typedef struct {
  int status; /* 0, 1, 2 */
  void *data; /* we do not know the data */
  time_t T_send;
  time_t T_pending;
  time_t T_rulare;
  void *resursa; /* ce resursa se cere */
} work;
