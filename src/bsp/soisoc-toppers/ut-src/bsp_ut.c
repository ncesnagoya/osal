/******************************************************************************
** File:  bsp_ut.c
**
**
**      This is governed by the NASA Open Source Agreement and may be used,
**      distributed and modified only pursuant to the terms of that agreement.
**
**      Copyright (c) 2004-2015, United States government as represented by the
**      administrator of the National Aeronautics Space Administration.
**      All rights reserved.
**
**
** Purpose:
**   BSP unit test implementation functions.
**
** History:
**   Created on: Feb 10, 2015
**
******************************************************************************/

/*
 * NOTE - This entire source file is only relevant for unit testing.
 * It should not be included in a "normal" BSP build.
 */

#define _USING_TOPPERS_INCLUDES_

/*
**  Include Files
*/
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <errno.h>
//#include <rtems.h>
//#include <rtems/mkrootfs.h>
//#include <rtems/bdbuf.h>
//#include <rtems/blkdev.h>
//#include <rtems/diskdevs.h>
//#include <rtems/error.h>
//#include <rtems/ramdisk.h>
//#include <rtems/dosfs.h>
//#include <rtems/fsmount.h>
//#include <rtems/shell.h>
#include "kernel.h" /* TOPPERS */
#include "itron.h"
#include "syssvc/serial.h"
#include "syssvc/syslog.h"

#include "osconfig.h"
#include "utbsp.h"
#include "uttest.h"

/*
**  External Declarations
*/
void OS_Application_Startup(void);

//extern rtems_status_code rtems_ide_part_table_initialize (const char* );


#define RTEMS_NUMBER_OF_RAMDISKS 1


/*
** Global variables
*/

/*
 * The RAM Disk configuration.
 */
//rtems_ramdisk_config rtems_ramdisk_configuration[RTEMS_NUMBER_OF_RAMDISKS];

/*
 * The number of RAM Disk configurations.
*/
//size_t rtems_ramdisk_configuration_size = RTEMS_NUMBER_OF_RAMDISKS;

/*
** RAM Disk IO op table.
*/
//rtems_driver_address_table rtems_ramdisk_io_ops =
//{
//        .initialization_entry = ramdisk_initialize,
//        .open_entry =           rtems_blkdev_generic_open,
//        .close_entry =          rtems_blkdev_generic_close,
//        .read_entry =           rtems_blkdev_generic_read,
//        .write_entry =          rtems_blkdev_generic_write,
//        .control_entry =        rtems_blkdev_generic_ioctl
//};

/*
 * Under RTEMS there is no notion of command-line arguments like in pc-linux,
 * so it is not as easy to change this value at runtime.  For now the default
 * will show all messages except debug.
 *
 * It may be possible to set this value using the shell...
 */
uint32 CurrVerbosity = (2 << UTASSERT_CASETYPE_PASS) - 1;

/*
 * The RTEMS shell needs a function to check the validity of a login username/password
 * This is just a stub that always passes.
 */
//bool BSP_Login_Check(const char *user, const char *passphrase)
//{
//   return TRUE;
//}


void UT_BSP_Setup(const char *Name)
{
    int status;

    syslog(LOG_NOTICE, "\n\n*** RTEMS Info ***\n" );
    syslog(LOG_NOTICE, "%s", Name );
    //printf("%s", _Copyright_Notice );
    //printf("%s\n\n", _RTEMS_version );
    //printf(" Stack size=%d\n", (int)Configuration.stack_space_size );
    //printf(" Workspace size=%d\n",   (int) Configuration.work_space_size );
    syslog(LOG_NOTICE, "\n");
    syslog(LOG_NOTICE, "*** End RTEMS info ***\n\n" );

#if 0
    /*
    ** Create the RTEMS Root file system
    */
    status = rtems_create_root_fs();
    if (status != RTEMS_SUCCESSFUL)
    {
        printf("Creating Root file system failed: %s\n",rtems_status_text(status));
    }

    /*
    ** create the directory mountpoints
    */
    status = mkdir("/ram", S_IFDIR |S_IRWXU | S_IRWXG | S_IRWXO); /* For ramdisk mountpoint */
    if (status != RTEMS_SUCCESSFUL)
    {
        printf("mkdir failed: %s\n", strerror (errno));
    }

    status = mkdir("/cf", S_IFDIR |S_IRWXU | S_IRWXG | S_IRWXO); /* For EEPROM mountpoint */
    if (status != RTEMS_SUCCESSFUL)
    {
        printf("mkdir failed: %s\n", strerror (errno));
        return;
    }

    /*
     * Register the IDE partition table.
     */
    status = rtems_ide_part_table_initialize ("/dev/hda");
    if (status != RTEMS_SUCCESSFUL)
    {
      printf ("error: ide partition table not found: %s / %s\n",
              rtems_status_text (status),strerror(errno));
    }

    status = mount("/dev/hda1", "/cf",
          RTEMS_FILESYSTEM_TYPE_DOSFS,
          RTEMS_FILESYSTEM_READ_WRITE,
          NULL);
    if (status < 0)
    {
      printf ("mount failed: %s\n", strerror (errno));
    }

    status = rtems_shell_init("SHLL", RTEMS_MINIMUM_STACK_SIZE * 4, 100, "/dev/console", false, false, BSP_Login_Check);
    if (status < 0)
    {
      printf ("shell init failed: %s\n", strerror (errno));
    }
#endif

    UT_BSP_DoText(UTASSERT_CASETYPE_BEGIN, Name);
}


void UT_BSP_StartTestSegment(uint32 SegmentNumber, const char *SegmentName)
{
    char ReportBuffer[128];
    uint_t i;
    char c;

    //snprintf(ReportBuffer,sizeof(ReportBuffer), "%02u %s", (unsigned int)SegmentNumber, SegmentName);
    //if( )
    syslog(LOG_EMERG, "[UT_BSP] %02u %s\n",(unsigned int)SegmentNumber,SegmentName);
    //syslog_printf("%s", SegmentName, ReportBuffer);
    //UT_BSP_DoText(UTASSERT_CASETYPE_BEGIN, ReportBuffer);
}

void UT_BSP_DoText(uint8 MessageType, const char *OutputMessage)
{
   const char *Prefix;

   if ((CurrVerbosity >> MessageType) & 1)
   {
      switch(MessageType)
      {
      case UTASSERT_CASETYPE_ABORT:
         Prefix = "ABORT";
         break;
      case UTASSERT_CASETYPE_FAILURE:
         Prefix = "FAIL";
         break;
      case UTASSERT_CASETYPE_MIR:
         Prefix = "MIR";
         break;
      case UTASSERT_CASETYPE_TSF:
         Prefix = "TSF";
         break;
      case UTASSERT_CASETYPE_TTF:
          Prefix = "TTF";
          break;
      case UTASSERT_CASETYPE_NA:
         Prefix = "N/A";
         break;
      case UTASSERT_CASETYPE_BEGIN:
         //printf("\n"); /* add a bit of extra whitespace between tests */
         Prefix = "BEGIN";
         break;
      case UTASSERT_CASETYPE_END:
         Prefix = "END";
         break;
      case UTASSERT_CASETYPE_PASS:
         Prefix = "PASS";
         break;
      case UTASSERT_CASETYPE_INFO:
         Prefix = "INFO";
         break;
      case UTASSERT_CASETYPE_DEBUG:
         Prefix = "DEBUG";
         break;
      default:
         Prefix = "OTHER";
         break;
      }
      //syslog(LOG_ALERT, "[%5s] %s\n",Prefix,OutputMessage);
   }

   /*
    * If any ABORT (major failure) message is thrown,
    * then actually call abort() to stop the test and dump a core
    */
   if (MessageType == UTASSERT_CASETYPE_ABORT)
   {
       //abort();
      ext_ker();
   }
}

void UT_BSP_DoReport(const char *File, uint32 LineNum, uint32 SegmentNum, uint32 TestSeq, uint8 MessageType, const char *SubsysName, const char *ShortDesc)
{
    uint32 FileLen;
    const char *BasePtr;
    char ReportBuffer[128];

    FileLen = strlen(File);
    BasePtr = File + FileLen;
    while (FileLen > 0)
    {
        --BasePtr;
        --FileLen;
        if (*BasePtr == '/' || *BasePtr == '\\')
        {
            ++BasePtr;
            break;
        }
    }

    //snprintf(ReportBuffer,sizeof(ReportBuffer), "%02u.%03u %s:%u - %s",
    //        (unsigned int)SegmentNum, (unsigned int)TestSeq,
    //        BasePtr, (unsigned int)LineNum, ShortDesc);

    UT_BSP_DoText(MessageType, ReportBuffer);
}

void UT_BSP_DoTestSegmentReport(const char *SegmentName, const UtAssert_TestCounter_t *TestCounters)
{
    char ReportBuffer[128];

//    snprintf(ReportBuffer,sizeof(ReportBuffer),
    syslog(LOG_NOTICE,
            "%02u %-20s TOTAL::%-4u  PASS::%-4u  FAIL::%-4u   MIR::%-4u   TSF::%-4u   N/A::%-4u\n",
            (unsigned int)TestCounters->TestSegmentCount,
            SegmentName,
            (unsigned int)TestCounters->TotalTestCases,
            (unsigned int)TestCounters->CaseCount[UTASSERT_CASETYPE_PASS],
            (unsigned int)TestCounters->CaseCount[UTASSERT_CASETYPE_FAILURE],
            (unsigned int)TestCounters->CaseCount[UTASSERT_CASETYPE_MIR],
            (unsigned int)TestCounters->CaseCount[UTASSERT_CASETYPE_TSF],
            (unsigned int)TestCounters->CaseCount[UTASSERT_CASETYPE_NA]);


    UT_BSP_DoText(UTASSERT_CASETYPE_END, ReportBuffer);
}

void UT_BSP_EndTest(const UtAssert_TestCounter_t *TestCounters)
{
   /*
    * Only output a "summary" if there is more than one test Segment.
    * Otherwise it is a duplicate of the report already given.
    */
   if (TestCounters->TestSegmentCount > 1)
   {
       UT_BSP_DoTestSegmentReport("SUMMARY", TestCounters);
   }

//   printf("COMPLETE: %u test segment(s) executed\n\n", (unsigned int)TestCounters->TestSegmentCount);
   syslog(LOG_NOTICE, "COMPLETE: %u test segment(s) executed\n\n", (unsigned int)TestCounters->TestSegmentCount);

   /*
    * Not calling exit() under RTEMS, this simply shuts down the executive,
    * forcing the user to reboot the system.
    *
    * Calling "pause()" in a loop causes execution to get stuck here, but the RTEMS
    * shell thread will still be active so the user can poke around, read results,
    * then use a shell command to reboot when ready.
    */
   while (TRUE)
   {
       //pause();
   }

}

// log_output.cからsyslog_printfを流用してvsnprintfを作成


/*
 *  数値を文字列に変換
 */
#define CONVERT_BUFLEN  ((sizeof(uintptr_t) * CHAR_BIT + 2) / 3)
                    /* uintptr_t型の数値の最大文字数 */
static int
convert(uintptr_t val, uint_t radix, const char *radchar,
      uint_t width, bool_t minus, bool_t padzero, char *str, uint_t strlen )
{
  char  buf[CONVERT_BUFLEN];
  uint_t  i, j;

  i = 0U;
  do {
    str[i++] = radchar[val % radix];
    val /= radix;

  } while (i < CONVERT_BUFLEN && val != 0);
  str[i] = '\0';
//  } while (i < buflen && val != 0);
    syslog(LOG_NOTICE, "convert, str[%s]:%x i[%d]", str, str, i);

/*  if (minus && width > 0) {
    width -= 1;
  }
  if (minus && padzero) {
    //(*putc)('-');
    buf[i++] = '-';
  }
  for (j = i; j < width; j++) {
    //(*putc)(padzero ? '0' : ' ');
    buf[i++] = padzero ? '0' : ' ';
  }
  if (minus && !padzero) {
    //(*putc)('-');
    buf[i++] = '-';
  }
  //while (i > 0U) {
  //  (*putc)(buf[--i]);
  //}
  //syslog(LOG_NOTICE, "convert, buf[%s] i[%d]", buf[0], i);
*/
  return(i);
}

/*
 *  文字列整形出力
 */
static const char raddec[] = "0123456789";
static const char radhex[] = "0123456789abcdef";
static const char radHEX[] = "0123456789ABCDEF";

int ut_snprintf(char *str, size_t strlen, const char *format, ...)
{
  va_list arg;

  va_start(arg, format);
  ut_vsnprintf(str, strlen, format, arg);
  va_end(arg);
}

int ut_vsnprintf(char *str, size_t strlen, const char *format, va_list ap)
{
  char      c;
  int       len;
  int       size;
  uint_t    width;
  bool_t    padzero;
  intptr_t  val;
  const char *s;

  s = str;

  len = 0;

  while ( ((c = *format++) != '\0') && (len < strlen) ) {
    if (c != '%') {
      *(str++) = c;
      len++;
      continue;
    }
    //syslog(LOG_NOTICE, "str[%s] strlen[%d] s[%s] len[%d]", str, strlen, s, len);

    width = 0U;
    padzero = false;
    if ((c = *format++) == '0') {
      padzero = true;
      c = *format++;
    }
    while ('0' <= c && c <= '9') {
      width = width * 10U + c - '0';
      c = *format++;
    }
    if (c == 'l') {
      c = *format++;
    }
    switch (c) {
    case 'd':
      val = (intptr_t)va_arg(ap, signed long);
      if (val >= 0) {
    syslog(LOG_NOTICE, "str[%s]:%x strlen[%d] s[%s]:%x len[%d]", str, str, strlen, s, s,len);
    //syslog(LOG_ALERT, "printf d, val[%d] width[%d] padzero[%d] str[%s] len[%d] stradd[%x]", val, width, padzero, str, len, *str);
        size = convert((uintptr_t) val, 10U, raddec,
                    width, false, padzero, str, strlen - len);
      //syslog(LOG_ALERT, "[%s]", str);
      }
      else {
        size = convert((uintptr_t)(-val), 10U, raddec,
                    width, true, padzero, str, sizeof(str)-len);
      }
      break;
    case 'u':
      val = (intptr_t)va_arg(ap, signed long);
      size = convert((uintptr_t) val, 10U, raddec, width, false, padzero, str, sizeof(str)-len);
      break;
    case 'x':
    case 'p':
      val = (intptr_t)va_arg(ap, unsigned long);
      size = convert((uintptr_t) val, 16U, radhex, width, false, padzero, str, sizeof(str)-len);
      break;
    case 'X':
      val = (intptr_t)va_arg(ap, unsigned long);
      size = convert((uintptr_t) val, 16U, radHEX, width, false, padzero, str, sizeof(str)-len);
      break;
    case 'c':
      str = (char)va_arg(ap, int);
      size = 1;
      break;
    case 's':
      s = (const char *)va_arg(ap, char*);
      size = 0;
      while (((c = *s++) != '\0') && (len+size < strlen) ) {
        *str++ = c;
        size++;
      }
      size = 0;
      break;
    default:

      break;
    }
    len += size;
  }
  *str = '\0';
}

#if 0
/*
** A simple entry point to start from the loader
*/
rtems_task Init(
  rtems_task_argument ignored
)
{
   UT_BSP_Setup("PC-RTEMS UNIT TEST");

   /*
   ** Call application specific entry point.
   ** This is supposed to call OS_API_Init()
   */
   OS_Application_Startup();

   /*
   ** In unit test mode, call the UtTest_Run function (part of UT Assert library)
   */
   UtTest_Run();
   UT_BSP_EndTest(UtAssert_GetCounters());

}
#endif

/* configuration information */

/*
** RTEMS OS Configuration defintions
*/
#define TASK_INTLEVEL 0
#define CONFIGURE_INIT
#define CONFIGURE_INIT_TASK_ATTRIBUTES  (RTEMS_FLOATING_POINT | RTEMS_PREEMPT | RTEMS_NO_TIMESLICE | RTEMS_ASR | RTEMS_INTERRUPT_LEVEL(TASK_INTLEVEL))
#define CONFIGURE_INIT_TASK_STACK_SIZE  (20*1024)
#define CONFIGURE_INIT_TASK_PRIORITY    120

/*
 * Note that these resources are shared with RTEMS itself (e.g. the shell)
 * so they should be allocated slightly higher than the limits in osconfig.h
 */
#define CONFIGURE_MAXIMUM_TASKS                      (OS_MAX_TASKS + 4)
#define CONFIGURE_MAXIMUM_TIMERS                     (OS_MAX_TIMERS + 2)
#define CONFIGURE_MAXIMUM_SEMAPHORES                 (OS_MAX_BIN_SEMAPHORES + OS_MAX_COUNT_SEMAPHORES + OS_MAX_MUTEXES + 4)
#define CONFIGURE_MAXIMUM_MESSAGE_QUEUES             (OS_MAX_QUEUES + 4)


#define CONFIGURE_EXECUTIVE_RAM_SIZE    (1024*1024)

#define CONFIGURE_RTEMS_INIT_TASKS_TABLE
#define CONFIGURE_APPLICATION_NEEDS_CONSOLE_DRIVER
#define CONFIGURE_APPLICATION_NEEDS_CLOCK_DRIVER

#define CONFIGURE_USE_IMFS_AS_BASE_FILESYSTEM
#define CONFIGURE_LIBIO_MAXIMUM_FILE_DESCRIPTORS     100

#define CONFIGURE_FILESYSTEM_RFS
#define CONFIGURE_FILESYSTEM_IMFS
#define CONFIGURE_FILESYSTEM_DOSFS
#define CONFIGURE_FILESYSTEM_DEVFS

#define CONFIGURE_APPLICATION_NEEDS_LIBBLOCK

#define CONFIGURE_MICROSECONDS_PER_TICK              10000

#define CONFIGURE_MAXIMUM_DRIVERS                   10

#define CONFIGURE_APPLICATION_NEEDS_IDE_DRIVER
#define CONFIGURE_APPLICATION_NEEDS_ATA_DRIVER
#define CONFIGURE_ATA_DRIVER_TASK_PRIORITY         9

#define CONFIGURE_MAXIMUM_POSIX_KEYS               4

//#include <rtems/confdefs.h>

#define CONFIGURE_SHELL_COMMANDS_INIT
#define CONFIGURE_SHELL_COMMANDS_ALL
#define CONFIGURE_SHELL_MOUNT_MSDOS

//#include <rtems/shellconfig.h>


