/* 
 *  COPYRIGHT (c) 1989, 1990, 1991, 1992, 1993, 1994.
 *  On-Line Applications Research Corporation (OAR).
 *  All rights assigned to U.S. Government, 1994.
 *
 *  This material may be reproduced by or for the U.S. Government pursuant
 *  to the copyright license under the clause at DFARS 252.227-7013.  This
 *  notice must appear in all copies of this file and its derivatives.
 *
 *  $Id$
 */

#define CONFIGURE_INIT
#include "system.h"
#include <sched.h>


void *POSIX_Init(
  void *argument
)
{
  int                 status;
  pthread_t           thread_id;
  pthread_condattr_t  attr;
  int                 pshared;
  pthread_cond_t      cond;
  struct timespec     timeout;

  puts( "\n\n*** POSIX TEST 10 ***" );

  puts( "Init: pthread_condattr_init" );
  status = pthread_condattr_init( &attr );
  assert( !status );

  puts( "Init: pthread_condattr_init - EINVAL" );
  status = pthread_condattr_init( NULL );
  if ( status != EINVAL )
    printf( "status = %d\n", status );
  assert( status == EINVAL );

  puts( "Init: pthread_condattr_destroy" );
  status = pthread_condattr_destroy( &attr );
  assert( !status );

  puts( "Init: pthread_condattr_destroy - EINVAL" );
  status = pthread_condattr_destroy( NULL );
  if ( status != EINVAL )
    printf( "status = %d\n", status );
  assert( status == EINVAL );

  puts( "Init: pthread_condattr_init" );
  status = pthread_condattr_init( &attr );
  assert( !status );

  puts( "Init: pthread_condattr_setpshared - PTHREAD_PROCESS_SHARED" );
  status = pthread_condattr_setpshared( &attr, PTHREAD_PROCESS_SHARED );
  assert( !status );

  puts( "Init: pthread_condattr_setpshared - PTHREAD_PROCESS_PRIVATE" );
  status = pthread_condattr_setpshared( &attr, PTHREAD_PROCESS_PRIVATE );
  assert( !status );

  status = pthread_condattr_setpshared( NULL, PTHREAD_PROCESS_PRIVATE );
  if ( status != EINVAL )
    printf( "status = %d\n", status );
  assert( status == EINVAL );
  puts( "Init: pthread_condattr_setpshared - EINVAL - attr" );

  status = pthread_condattr_setpshared( &attr, 0xFFFFFF );
  if ( status != EINVAL )
    printf( "status = %d\n", status );
  assert( status == EINVAL );
  puts( "Init: pthread_condattr_setpshared - EINVAL - pshared" );

  status = pthread_condattr_getpshared( &attr, &pshared );
  assert( !status );
  printf( "Init: pthread_condattr_getpshared - %d\n", pshared );

  status = pthread_condattr_getpshared( NULL, &pshared );
  if ( status != EINVAL )
    printf( "status = %d\n", status );
  assert( status == EINVAL );
  puts( "Init: pthread_condattr_getpshared - EINVAL" );

  puts( "Init: pthread_cond_init - NULL attr" );
  status = pthread_cond_init( &cond, NULL );
  assert( !status );

  puts( "Init: pthread_cond_destroy" );
  status = pthread_cond_destroy( &cond );
  assert( !status );

/* initiailize the attribute for the rest of the test */

  puts( "Init: pthread_cond_init - attr" );
  status = pthread_cond_init( &Cond1_id, &attr );
  assert( !status );

/* signal task1 with a condition variable */

  empty_line();

  status = pthread_create( &Task_id, NULL, Task_1, NULL );
  assert( !status );

/* switch to task1 to allow it to wait for a condition variable */

  puts( "Init: sleep to switch to Task_1" );
  sleep( 1 );

  puts( "Init: pthread_cond_signal" );
  status = pthread_cond_signal( &Cond1_id );
  assert( !status );

  empty_line();

  status = pthread_create( &Task2_id, NULL, Task_2, NULL );
  assert( !status );

/* switch to task1 and task2 to allow them to wait for broadcast signal */

  puts( "Init: sleep - switch to Task_1 and Task_2" );
  sleep( 1 );

/* broadcast a condition variable to task1 and task2 */

  puts( "Init: pthread_cond_broadcast" );
  status = pthread_cond_broadcast( &Cond1_id );
  assert( !status );

  puts( "Init: sleep - switch to Task_1" );
  sleep( 0 );

/* timedwait case - timeout */

  status = pthread_mutex_lock( &Mutex_id );
  assert( !status );

/* set timeout to 3 seconds */

  timeout.tv_sec = 3;

  puts( "Init: pthread_cond_timedwait for 3 seconds" );
  status = pthread_cond_timedwait( &Cond1_id, &Mutex_id, &timeout );
  if ( status != ETIMEDOUT )
    printf( "status = %d\n", status );
  assert( status == ETIMEDOUT );

  puts( "Init: timedout on pthread_cond_timedwait release mutex" );
  /* exit this thread */

  puts( "*** END OF POSIX TEST 5 ***" );
  exit( 0 );

  return NULL; /* just so the compiler thinks we returned something */
}
