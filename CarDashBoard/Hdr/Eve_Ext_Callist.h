/**
 * @file Eve_Ext_Callist.h
 * @brief Calllist Extension
 *
 * @author Bridgetek
 *
 * @date 2022
 *
 * Copyright (c) Bridgetek Pte Ltd.
 * Software License Agreement
 *
 * This code is provided as an example only and is not guaranteed by Bridgetek.
 * BRIDGETEK accept no responsibility for any issues resulting from its use.
 * The developer of the final application incorporating any parts of this
 * sample project is responsible for ensuring its safe and correct operation
 * and for any consequences resulting from its use.
 */
#ifndef _EVE_EXT_CALLIST_H__
#define _EVE_EXT_CALLIST_H__

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "EVE_Platform.h"
#include "EVE_CoCmd.h"

// ------ Extension ID and version -----------------------------------------------
#define EVE_EXT_CALLLIST_ENABLE 1
#define EVE_EXT_CALLLIST_HEADER_VERSIONING 1 // 1
// ------ Extension Internal type ------------------------------------------------
#define EVE_EXT_CALLLIST_HEADER_SIZE (2*4)//bytes
#define EVE_EXT_CALLLIST_ENTRY_SIZE  (2*4)//bytes

#define MSG(M, ...)             printf("Info: " M "\r\n", ##__VA_ARGS__)
#define ERR(M, ...)             printf("Error: " M "\r\n", ##__VA_ARGS__)
#define WRN(M, ...)             printf("Warning: " M "\r\n", ##__VA_ARGS__)

// ------ Extension External type ------------------------------------------------
typedef struct _EVE_Ext_CallList
{
    uint32_t entryOffset;
    uint32_t entrySize;
} t_EVE_Ext_CallList_Entry;

typedef struct
{
    // data written into binary
    uint32_t headerVersionNumber;
    uint32_t ramGOffset;
    uint32_t entryNumber;
    
    // data not in binary
    char *path_bin_file;
    uint32_t wpStart;
    bool isStopped;
    bool isExist;

    t_EVE_Ext_CallList_Entry entries[1024];
} t_EVE_Ext_CallList;

// ------ Extension External function --------------------------------------------
t_EVE_Ext_CallList* EVE_Ext_CallList_Instance_Init(EVE_HalContext* phost, uint32_t ramGOffset, char *path_bin_file);
t_EVE_Ext_CallList* EVE_Ext_CallList_Instance_Init_Exising(EVE_HalContext* phost, uint32_t ramGOffset, char* path_bin_file);
void EVE_Ext_CallList_Instance_DeInit(t_EVE_Ext_CallList** instance);
void EVE_Ext_CallList_Instance_Save(EVE_HalContext *phost, t_EVE_Ext_CallList *instance);
void EVE_Ext_CallList_Record_Start(EVE_HalContext* phost, t_EVE_Ext_CallList* instance, uint32_t ramGOffset);
void EVE_Ext_CallList_Record_Stop(EVE_HalContext* phost);
void EVE_Ext_CallList_Record_Entry(EVE_HalContext* phost, t_EVE_Ext_CallList* instance, uint32_t offset, uint32_t size);
void EVE_Ext_CallList_Record_Entry_CoCmd(EVE_HalContext* phost, t_EVE_Ext_CallList* instance, uint32_t size);
void EVE_Ext_CallList_Record_Entry_DlCmd(EVE_HalContext* phost, t_EVE_Ext_CallList* instance);
void EVE_Ext_CallList_Replay_Start(EVE_HalContext* phost, t_EVE_Ext_CallList* instance);
void EVE_Ext_CallList_Replay_Repeat(EVE_HalContext* phost, t_EVE_Ext_CallList* instance) ;
void EVE_Ext_CallList_Replay_Entry(EVE_HalContext* phost, t_EVE_Ext_CallList* instance, int nth, char* data);
void EVE_Ext_CallList_Replay_Entry_SPACE(EVE_HalContext* phost, t_EVE_Ext_CallList* instance, int nth, char* data);
void EVE_Ext_CallList_Replay_Entry_FILL0(EVE_HalContext* phost, t_EVE_Ext_CallList* instance, int nth, char* data);
#endif // _EVE_EXT_CALLIST_H__
