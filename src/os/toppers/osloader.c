/*
** File   : osloader.c
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
** Purpose: This file contains the module loader and symbol lookup functions for the OSAL.
**          Currently, we are using RTEMS with two different loaders: 
**          1. The CEXP loader 
**          2. An internally developed static module loader ( which should be open source soon ).
*/

/****************************************************************************************
                                    INCLUDE FILES
****************************************************************************************/

//#define _USING_RTEMS_INCLUDES_

//#include <stdio.h>
//#include <unistd.h> /* close() */
//#include <string.h> /* memset() */
//#include <stdlib.h>
//#include <fcntl.h>

//#include <rtems.h>
#include "common_types.h"
#include "osapi.h"

/*
** If OS_INCLUDE_MODULE_LOADER is not defined, then skip the module 
*/
#ifdef OS_INCLUDE_MODULE_LOADER
/*
** Select the static or dynamic loader
*/
#ifdef OS_STATIC_LOADER
   #include "loadstaticloadfile.h"
#else
   #include <cexp.h>
#endif

/****************************************************************************************
                                     TYPEDEFS
****************************************************************************************/

typedef struct
{
    char    SymbolName[OS_MAX_SYM_LEN];
    uint32  SymbolAddress;
} SymbolRecord_t;

/****************************************************************************************
                                     DEFINES
****************************************************************************************/
#define OS_SYMBOL_RECORD_SIZE sizeof(SymbolRecord_t)

#undef OS_DEBUG_PRINTF 

#define OSAL_TABLE_MUTEX_ATTRIBS \
 (RTEMS_PRIORITY | RTEMS_BINARY_SEMAPHORE | \
  RTEMS_INHERIT_PRIORITY | RTEMS_NO_PRIORITY_CEILING | RTEMS_LOCAL)

/****************************************************************************************
                                   GLOBAL DATA
****************************************************************************************/

/*
** Need to define the OS Module table here. 
** osconfig.h will have the maximum number of loadable modules defined.
*/
OS_module_record_t OS_module_table[OS_MAX_MODULES];

/*
** The Mutex for protecting the above table
*/
rtems_id     OS_module_table_sem;


#ifdef OS_STATIC_LOADER
   /*
   ** In addition to the module table, this is the static loader specific data.
   ** It is a mini symbol table with all of the information for the static loaded modules.
   */
   static_load_file_header_t OS_symbol_table[OS_MAX_MODULES];
#endif


/****************************************************************************************
                                INITIALIZATION FUNCTION
****************************************************************************************/

int32  OS_ModuleTableInit ( void )
{
   return(OS_SUCCESS);
}

/****************************************************************************************
                                    Symbol table API
****************************************************************************************/
/*--------------------------------------------------------------------------------------
    Name: OS_SymbolLookup
    
    Purpose: Find the Address of a Symbol 

    Parameters: 

    Returns: OS_ERROR if the symbol could not be found
             OS_SUCCESS if the symbol is found 
             OS_INVALID_POINTER if one of the pointers passed in are NULL 
---------------------------------------------------------------------------------------*/
int32 OS_SymbolLookup( uint32 *SymbolAddress, char *SymbolName )
{
   return(OS_SUCCESS);
}/* end OS_SymbolLookup */

/*--------------------------------------------------------------------------------------
    Name: OS_SymbolTableDump
    
    Purpose: Dumps the system symbol table to a file

    Parameters: 

    Returns: OS_ERROR if the symbol table could not be read or dumped
             OS_FS_ERR_PATH_INVALID  if the file and/or path is invalid 
             OS_SUCCESS if the file is written correctly 
---------------------------------------------------------------------------------------*/
int32 OS_SymbolTableDump ( char *filename, uint32 SizeLimit )
{
   return(OS_SUCCESS);
}/* end OS_SymbolTableDump */

/****************************************************************************************
                                    Module Loader API
****************************************************************************************/

/*--------------------------------------------------------------------------------------
    Name: OS_ModuleLoad
    
    Purpose: Loads an ELF object file into the running operating system

    Parameters: 

    Returns: OS_ERROR if the module cannot be loaded
             OS_INVALID_POINTER if one of the parameters is NULL
             OS_ERR_NO_FREE_IDS if the module table is full
             OS_ERR_NAME_TAKEN if the name is in use
             OS_SUCCESS if the module is loaded successfuly 
---------------------------------------------------------------------------------------*/
int32 OS_ModuleLoad ( uint32 *module_id, char *module_name, char *filename )
{
   return(OS_SUCCESS);
}/* end OS_ModuleLoad */

/*--------------------------------------------------------------------------------------
    Name: OS_ModuleUnload
    
    Purpose: Unloads the module file from the running operating system

    Parameters: 

    Returns: OS_ERROR if the module is invalid or cannot be unloaded
             OS_SUCCESS if the module was unloaded successfuly 
---------------------------------------------------------------------------------------*/
int32 OS_ModuleUnload ( uint32 module_id )
{

   return(OS_SUCCESS);
   
}/* end OS_ModuleUnload */

/*--------------------------------------------------------------------------------------
    Name: OS_ModuleInfo
    
    Purpose: Returns information about the loadable module

    Parameters: 

    Returns: OS_INVALID_POINTER if the pointer to the ModuleInfo structure is NULL 
             OS_ERR_INVALID_ID if the module ID is not valid
             OS_SUCCESS if the module info was filled out successfuly 
---------------------------------------------------------------------------------------*/
int32 OS_ModuleInfo ( uint32 module_id, OS_module_record_t *module_info )
{

   /*
   ** Check the parameter
   */
   if ( module_info == NULL )
   {
      return(OS_INVALID_POINTER);
   }

   /*
   ** Check the module_id
   */
   if ( module_id >= OS_MAX_MODULES || OS_module_table[module_id].free == TRUE )
   {
      return(OS_ERR_INVALID_ID);
   }


  
   return(OS_SUCCESS);

}/* end OS_ModuleInfo */

#endif /* OS_INCLUDE_MODULE_LOADER */
