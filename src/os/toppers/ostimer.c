/*
** File   : ostimer.c
**
**      Copyright (c) 2004-2006, United States government as represented by the 
**      administrator of the National Aeronautics Space Administration.  
**      All rights reserved. This software was created at NASAs Goddard 
**      Space Flight Center pursuant to government contracts.
**
**      This is governed by the NASA Open Source Agreement and may be used, 
**      distributed and modified only pursuant to the terms of that agreement.
**
** Author : Alan Cudmore
**
** Purpose: This file contains the OSAL Timer API for RTEMS
*/

/****************************************************************************************
                                    INCLUDE FILES
****************************************************************************************/
#define _USING_TOPPERS_INCLUDES_

#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <stdio.h>

#include "kernel.h" /* TOPPERS */
#include "itron.h"

#include "common_types.h"
#include "osapi.h"
/****************************************************************************************
                                EXTERNAL FUNCTION PROTOTYPES
****************************************************************************************/

uint32 OS_FindCreator(void);


/****************************************************************************************
                                     DEFINES
****************************************************************************************/

#define UNINITIALIZED 0

#define OSAL_TABLE_MUTEX_ATTRIBS \
 (RTEMS_PRIORITY | RTEMS_BINARY_SEMAPHORE | \
  RTEMS_INHERIT_PRIORITY | RTEMS_NO_PRIORITY_CEILING | RTEMS_LOCAL)

/****************************************************************************************
                                    LOCAL TYPEDEFS 
****************************************************************************************/

typedef struct 
{
   uint32              free;
   char                name[OS_MAX_API_NAME];
   uint32              creator;
   uint32              start_time;
   uint32              interval_time;
   uint32              accuracy;
   OS_TimerCallback_t  callback_ptr;
   ID                  host_timerid;

} OS_timer_record_t;

/****************************************************************************************
                                   GLOBAL DATA
****************************************************************************************/

OS_timer_record_t OS_timer_table[OS_MAX_TIMERS];
uint32            os_clock_accuracy;

/*
** The Mutex for protecting the above table
*/
ID          OS_timer_table_sem;

/****************************************************************************************
                                INITIALIZATION FUNCTION
****************************************************************************************/
int32  OS_TimerAPIInit ( void )
{
   int               i;
   int32             return_code = OS_SUCCESS;
   
   /*
   ** Mark all timers as available
   */
   for ( i = 0; i < OS_MAX_TIMERS; i++ )
   {
      OS_timer_table[i].free      = TRUE;
      OS_timer_table[i].creator   = UNINITIALIZED;
      strcpy(OS_timer_table[i].name,"");
   }
	
   /*
   ** Store the clock accuracy for 1 tick.
   */
   //OS_TicksToUsecs(1, &os_clock_accuracy);

   /*
   ** Create the Timer Table semaphore
   */
#if 0 
/* 内部で使用するミューテックスは以下の静的APIを用いて事前に生成する */
KERNEL_DOMAIN {
   CRE_MTX(OSAL_TIMER_TABLE_MTX, {TA_CEILING, TMIN_TPRI});
}
   
   OS_timer_table_sem      = OSAL_TIMER_TABLE_MTX;
#endif
   
   return(return_code);
   
}



/****************************************************************************************
                                   Timer API
****************************************************************************************/

/******************************************************************************
**  Function:  OS_TimerCreate
**
**  Purpose:  Create a new OSAL Timer
**
**  Arguments:
**
**  Return:
*/
int32 OS_TimerCreate(uint32 *timer_id,       const char         *timer_name, 
                     uint32 *clock_accuracy, OS_TimerCallback_t  callback_ptr)
{
   uint32            possible_tid;
   int32             i;

   if ( timer_id == NULL || timer_name == NULL || clock_accuracy == NULL )
   {
        return OS_INVALID_POINTER;
   }

   /* 
   ** we don't want to allow names too long
   ** if truncated, two names might be the same 
   */
   if (strlen(timer_name) > OS_MAX_API_NAME)
   {
      return OS_ERR_NAME_TOO_LONG;
   }

   /* 
   ** Check Parameters 
   */
   loc_mtx(OS_timer_table_sem);

   for(possible_tid = 0; possible_tid < OS_MAX_TIMERS; possible_tid++)
   {
      if (OS_timer_table[possible_tid].free == TRUE)
         break;
   }

   if( possible_tid >= OS_MAX_TIMERS || OS_timer_table[possible_tid].free != TRUE)
   {
        unl_mtx(OS_timer_table_sem);
        return OS_ERR_NO_FREE_IDS;
   }

   /* 
   ** Check to see if the name is already taken 
   */
   for (i = 0; i < OS_MAX_TIMERS; i++)
   {
       if ((OS_timer_table[i].free == FALSE) &&
            strcmp ((char*) timer_name, OS_timer_table[i].name) == 0)
       {
            unl_mtx(OS_timer_table_sem);
            return OS_ERR_NAME_TAKEN;
       }
   }

   /*
   ** Verify callback parameter
   */
   if (callback_ptr == NULL ) 
   {
      unl_mtx(OS_timer_table_sem);
      return OS_TIMER_ERR_INVALID_ARGS;
   }    

   /* 
   ** Set the possible timer Id to not free so that
   ** no other task can try to use it 
   */
   OS_timer_table[possible_tid].free = FALSE;
   unl_mtx(OS_timer_table_sem);

   OS_timer_table[possible_tid].creator = OS_FindCreator();
   strncpy(OS_timer_table[possible_tid].name, timer_name, OS_MAX_API_NAME);
   OS_timer_table[possible_tid].start_time = 0;
   OS_timer_table[possible_tid].interval_time = 0;
   OS_timer_table[possible_tid].callback_ptr = callback_ptr;
  
   /* 
   ** Create an interval timer -> Move to OS_TimerSet()
   */

   /*
   ** Return the clock accuracy to the user
   */
   *clock_accuracy = os_clock_accuracy;

   /*
   ** Return timer ID 
   */
   *timer_id = possible_tid;

   return OS_SUCCESS;
}

/******************************************************************************
**  Function:  OS_TimerSet
**
**  Purpose:  
**
**  Arguments:
**    (none)
**
**  Return:
**    (none)
*/
int32 OS_TimerSet(uint32 timer_id, uint32 start_time, uint32 interval_time)
{
   ER status;
   T_CCYC             ccyc;
   
   /* 
   ** Check to see if the timer_id given is valid 
   */
   if (timer_id >= OS_MAX_TIMERS || OS_timer_table[timer_id].free == TRUE)
   {
      return OS_ERR_INVALID_ID;
   }

   /*
   ** Round up the accuracy of the start time and interval times.
   ** Still want to preserve zero, since that has a special meaning. 
   */
   if (( start_time > 0 ) && (start_time < os_clock_accuracy))
   {
      start_time = os_clock_accuracy;
   }
 
   if ((interval_time > 0) && (interval_time < os_clock_accuracy ))
   {
      interval_time = os_clock_accuracy;
   }

   /*
   ** Save the start and interval times 
   */
   OS_timer_table[timer_id].start_time = start_time;
   OS_timer_table[timer_id].interval_time = interval_time;

   /*
   ** The defined behavior is to not arm the timer if the start time is zero
   ** If the interval time is zero, then the timer will not be re-armed.
   */
   if ( start_time > 0 )
   {
      ccyc.cycatr = TA_STA;
      ccyc.exinf = 0;
      ccyc.cychdr = (ALMHDR)OS_timer_table[timer_id].callback_ptr;
      ccyc.cyctim = OS_timer_table[timer_id].interval_time / 1000;
      ccyc.cycphs = OS_timer_table[timer_id].start_time / 1000;

      status = acre_cyc(&ccyc);
      if ( status != E_OK )
      {
        OS_timer_table[timer_id].free = TRUE;
        return ( OS_TIMER_ERR_INTERNAL);
      }
      OS_timer_table[timer_id].host_timerid = status;
    }
  return OS_SUCCESS;
}

/******************************************************************************
**  Function:  OS_TimerDelete
**
**  Purpose: 
**
**  Arguments:
**    (none)
**
**  Return:
**    (none)
*/
int32 OS_TimerDelete(uint32 timer_id)
{
   rtems_status_code status;

   /* 
   ** Check to see if the timer_id given is valid 
   */
   if (timer_id >= OS_MAX_TIMERS || OS_timer_table[timer_id].free == TRUE)
   {
      return OS_ERR_INVALID_ID;
   }

   status = rtems_semaphore_obtain (OS_timer_table_sem, RTEMS_WAIT, RTEMS_NO_TIMEOUT);
   OS_timer_table[timer_id].free = TRUE;
   status = rtems_semaphore_release (OS_timer_table_sem);

   /*
   ** Cancel the timer
   */
   status = rtems_timer_cancel(OS_timer_table[timer_id].host_timerid);
   if ( status != RTEMS_SUCCESSFUL )
   {
      return ( OS_TIMER_ERR_INTERNAL);
   }

   /*
   ** Delete the timer 
   */
   status = rtems_timer_delete(OS_timer_table[timer_id].host_timerid);
   if ( status != RTEMS_SUCCESSFUL )
   {
      return ( OS_TIMER_ERR_INTERNAL);
   }
	
   return OS_SUCCESS;
}

/***********************************************************************************
**
**    Name: OS_TimerGetIdByName
**
**    Purpose: This function tries to find a Timer Id given the name 
**             The id is returned through timer_id
**
**    Returns: OS_INVALID_POINTER if timer_id or timer_name are NULL pointers
**             OS_ERR_NAME_TOO_LONG if the name given is to long to have been stored
**             OS_ERR_NAME_NOT_FOUND if the name was not found in the table
**             OS_SUCCESS if success
**             
*/
int32 OS_TimerGetIdByName (uint32 *timer_id, const char *timer_name)
{
    uint32 i;

    if (timer_id == NULL || timer_name == NULL)
    {
        return OS_INVALID_POINTER;
    }

    /* 
    ** a name too long wouldn't have been allowed in the first place
    ** so we definitely won't find a name too long
    */
    if (strlen(timer_name) > OS_MAX_API_NAME)
    {
        return OS_ERR_NAME_TOO_LONG;
    }

    for (i = 0; i < OS_MAX_TIMERS; i++)
    {
        if (OS_timer_table[i].free != TRUE &&
                (strcmp (OS_timer_table[i].name , (char*) timer_name) == 0))
        {
            *timer_id = i;
            return OS_SUCCESS;
        }
    }
   
    /* 
    ** The name was not found in the table,
    **  or it was, and the sem_id isn't valid anymore 
    */
    return OS_ERR_NAME_NOT_FOUND;
    
}/* end OS_TimerGetIdByName */

/***************************************************************************************
**    Name: OS_TimerGetInfo
**
**    Purpose: This function will pass back a pointer to structure that contains 
**             all of the relevant info( name and creator) about the specified timer.
**             
**    Returns: OS_ERR_INVALID_ID if the id passed in is not a valid timer 
**             OS_INVALID_POINTER if the timer_prop pointer is null
**             OS_SUCCESS if success
*/
int32 OS_TimerGetInfo (uint32 timer_id, OS_timer_prop_t *timer_prop)  
{
   
    rtems_status_code status;

    /* 
    ** Check to see that the id given is valid 
    */
    if (timer_id >= OS_MAX_TIMERS || OS_timer_table[timer_id].free == TRUE)
    {
       return OS_ERR_INVALID_ID;
    }

    if (timer_prop == NULL)
    {
       return OS_INVALID_POINTER;
    }

    /* 
    ** put the info into the stucture 
    */
    status = rtems_semaphore_obtain (OS_timer_table_sem, RTEMS_WAIT, RTEMS_NO_TIMEOUT);

    timer_prop ->creator       = OS_timer_table[timer_id].creator;
    strcpy(timer_prop-> name, OS_timer_table[timer_id].name);
    timer_prop ->start_time    = OS_timer_table[timer_id].start_time;
    timer_prop ->interval_time = OS_timer_table[timer_id].interval_time;
    timer_prop ->accuracy      = OS_timer_table[timer_id].accuracy;
    
    status = rtems_semaphore_release (OS_timer_table_sem);

    return OS_SUCCESS;
    
} /* end OS_TimerGetInfo */
