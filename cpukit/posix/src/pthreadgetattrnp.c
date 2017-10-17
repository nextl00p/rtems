/**
 * @file
 *
 * @brief Pthread Get Attribute
 * @ingroup POSIXAPI
 */

/*
 *  COPYRIGHT (c) 2014.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#if HAVE_CONFIG_H
#include "config.h"
#endif

#define  _GNU_SOURCE
#include <pthread.h>
#include <errno.h>
#include <string.h>

#include <rtems/posix/pthreadimpl.h>
#include <rtems/posix/pthreadattrimpl.h>
#include <rtems/posix/priorityimpl.h>
#include <rtems/score/schedulerimpl.h>
#include <rtems/score/threadimpl.h>

int pthread_getattr_np(
  pthread_t       thread,
  pthread_attr_t *attr
)
{
  Thread_Control          *the_thread;
  ISR_lock_Context         lock_context;
  POSIX_API_Control       *api;
  const Scheduler_Control *scheduler;
  bool                     ok;

  if ( attr == NULL ) {
    return EINVAL;
  }

  attr = memset( attr, 0, sizeof( *attr ) );

  the_thread = _Thread_Get( thread, &lock_context );

  if ( the_thread == NULL ) {
    return ESRCH;
  }

  _Thread_State_acquire_critical( the_thread, &lock_context );

  api = the_thread->API_Extensions[ THREAD_API_POSIX ];

  attr->is_initialized = true;
  attr->stackaddr = the_thread->Start.Initial_stack.area;
  attr->stacksize = the_thread->Start.Initial_stack.size;
  attr->contentionscope = PTHREAD_SCOPE_PROCESS;

  if ( api->created_with_explicit_scheduler ) {
    attr->inheritsched = PTHREAD_EXPLICIT_SCHED;
  } else {
    attr->inheritsched = PTHREAD_INHERIT_SCHED;
  }

  attr->schedpolicy = api->schedpolicy;

  scheduler = _Thread_Scheduler_get_home( the_thread );
  attr->schedparam.sched_priority = _POSIX_Priority_From_core(
    scheduler,
    _Thread_Get_priority( the_thread )
  );
  _POSIX_Threads_Get_sched_param_sporadic(
    the_thread,
    api,
    scheduler,
    &attr->schedparam
  );
  attr->cputime_clock_allowed = 1;

  if ( _Thread_Is_joinable( the_thread ) ) {
    attr->detachstate = PTHREAD_CREATE_JOINABLE;
  } else {
    attr->detachstate = PTHREAD_CREATE_DETACHED;
  }

  attr->affinityset = &attr->affinitysetpreallocated;
  attr->affinitysetsize = sizeof( attr->affinitysetpreallocated );
  ok = _Scheduler_Get_affinity(
    the_thread,
    attr->affinitysetsize,
    attr->affinityset
  );

  _Thread_State_release( the_thread, &lock_context );
  return ok ? 0 : EINVAL;
}
