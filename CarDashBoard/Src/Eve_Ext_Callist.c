/**
 * @file Eve_Ext_Callist.c
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
#ifndef INCLUDE_CALLIST
#define INCLUDE_CALLIST
#include "Eve_Ext_Callist.h"
#include "common.h"

// ------ Extension Internal variable --------------------------------------------
static t_EVE_Ext_CallList *_internal_EVE_Ext_CallList_instance;

// ------ Extension External variable --------------------------------------------
// None

// ------ Extension Internal function --------------------------------------------
static uint32_t _EVE_Ext_CallList_Get_Offset(
    EVE_HalContext *phost, t_EVE_Ext_CallList *instance)
{
    if (!instance) {
        return 0;
    }
    uint16_t wp = EVE_Cmd_wp(phost);
    uint16_t offset = (wp - instance->wpStart) & EVE_CMD_FIFO_MASK;
    return offset;
}
static uint32_t _EVE_Ext_CallList_Read(t_EVE_Ext_CallList *instance) {
    uint32_t size = FileIO_File_Open(instance->path_bin_file, FILEIO_E_FOPEN_READ);
    if (size) do {
        instance->isExist = 1;
        MSG("Opening an existing binary");
        FileIO_File_Read(&instance->headerVersionNumber, sizeof(instance->headerVersionNumber));
        FileIO_File_Read(&instance->entryNumber, sizeof(instance->entryNumber));
        if (!instance->entryNumber) {
            break;
        }

        for (int i = 0; i < instance->entryNumber; i++) {
            FileIO_File_Read(&instance->entries[i].entryOffset, sizeof(instance->entries[i].entryOffset));
            FileIO_File_Read(&instance->entries[i].entrySize, sizeof(instance->entries[i].entrySize));
        }
    } while (0);
    FileIO_File_Close();
}

// ------ Extension External function --------------------------------------------
t_EVE_Ext_CallList* EVE_Ext_CallList_Instance_Init(EVE_HalContext* phost, uint32_t ramGOffset, char *path_bin_file)
{
    t_EVE_Ext_CallList *instance = malloc(sizeof(t_EVE_Ext_CallList));
    if (!instance) {
        ERR("malloc failed in %s", __FUNCTION__);
        return 0;
    }

    instance->isExist = 0;
    instance->path_bin_file = path_bin_file;
    instance->entryNumber = 0;
    instance->headerVersionNumber = EVE_EXT_CALLLIST_HEADER_VERSIONING;
    instance->ramGOffset = ramGOffset;
    instance->isStopped = 0;
    instance->wpStart = EVE_Cmd_wp(phost);
    return instance;
}

t_EVE_Ext_CallList* EVE_Ext_CallList_Instance_Init_Exising(EVE_HalContext* phost, uint32_t ramGOffset, char* path_bin_file){
    t_EVE_Ext_CallList* instance = EVE_Ext_CallList_Instance_Init( phost, ramGOffset, path_bin_file);
    _EVE_Ext_CallList_Read(instance);
    return instance;
}

void EVE_Ext_CallList_Instance_DeInit(t_EVE_Ext_CallList** instance) {
    if (!*instance) {
        return;
    }
    free(*instance);
    *instance = 0;
}

void EVE_Ext_CallList_Instance_Save(EVE_HalContext *phost, t_EVE_Ext_CallList *instance)
{
    if (!instance) {
        return;
    }
    if(instance->isStopped){
        return;
    }
    instance->isStopped = 1;

    FileIO_File_Open(instance->path_bin_file, FILEIO_E_FOPEN_WRITE);
    FileIO_File_Write(&instance->headerVersionNumber, sizeof(instance->headerVersionNumber));
    FileIO_File_Write(&instance->entryNumber, sizeof(instance->entryNumber));

    for(int i=0; i < instance->entryNumber; i++)
    {
        FileIO_File_Write(&instance->entries[i].entryOffset, sizeof(instance->entries[i].entryOffset));
        FileIO_File_Write(&instance->entries[i].entrySize, sizeof(instance->entries[i].entrySize));
    }

    uint32_t endAddr;
    EVE_CoCmd_getPtr(phost, &endAddr);
    EVE_Cmd_waitFlush(phost);

    char buff[1024 * 2];
    const int32_t chunk = 1024;
    int32_t readSize = endAddr - instance->ramGOffset;
    int32_t offset = 0;
    while (readSize > 0)
    {
        int32_t readNum = readSize > chunk ? chunk : readSize;
        Gpu_Hal_RdMem(phost, instance->ramGOffset + offset, buff, readNum);
        FileIO_File_Write(buff, readNum);
        readSize -=readNum;
        offset += readNum;
    }
    FileIO_File_Close();
}

void EVE_Ext_CallList_Record_Start(EVE_HalContext* phost, t_EVE_Ext_CallList* instance, uint32_t ramGOffset)
{
    EVE_CoCmd_newList(phost, ramGOffset);
    instance->wpStart = EVE_Cmd_wp(phost);
}

void EVE_Ext_CallList_Record_Stop(EVE_HalContext* phost) {
        EVE_CoCmd_endList(phost);
        EVE_Cmd_waitFlush(phost);
}

void EVE_Ext_CallList_Record_Entry(EVE_HalContext* phost, t_EVE_Ext_CallList* instance, 
        uint32_t offset, uint32_t size)
{
    instance->entries[instance->entryNumber].entryOffset = _EVE_Ext_CallList_Get_Offset(phost, instance) + offset;
    instance->entries[instance->entryNumber].entrySize = size;
    instance->entryNumber++;
}

void EVE_Ext_CallList_Record_Entry_CoCmd(EVE_HalContext* phost, t_EVE_Ext_CallList* instance, uint32_t size)
{
    return EVE_Ext_CallList_Record_Entry(phost, instance, 4, size);
}
void EVE_Ext_CallList_Record_Entry_DlCmd(EVE_HalContext* phost, t_EVE_Ext_CallList* instance) {
    return EVE_Ext_CallList_Record_Entry(phost, instance, 0, 4);
}

void EVE_Ext_CallList_Replay_Start(EVE_HalContext* phost, t_EVE_Ext_CallList* instance) {
    if (!instance) {
        WRN("Ignored replaying a null instance");
        return;
    }
    uint32_t readSize = FileIO_File_Open(instance->path_bin_file, FILEIO_E_FOPEN_READ);
    if (!readSize) {
        WRN("Ignored replaying an empty binary");
        return;
    }
    FileIO_File_Seek(EVE_EXT_CALLLIST_HEADER_SIZE + EVE_EXT_CALLLIST_ENTRY_SIZE * instance->entryNumber);
    readSize -= EVE_EXT_CALLLIST_HEADER_SIZE + EVE_EXT_CALLLIST_ENTRY_SIZE * instance->entryNumber;
    
    unsigned char buff[1024 * 2];
    const int32_t chunk = 1024;
    int32_t offset = 0;
    while (readSize > 0)
    {
        int32_t readNum = readSize > chunk ? chunk : readSize;
        int readFromFile = FileIO_File_Read(buff, readNum);
        EVE_Hal_wrMem(phost, instance->ramGOffset + offset, buff, readNum);
        readSize -=readNum;
        offset += readNum;
    }
    FileIO_File_Close();

    EVE_CoCmd_callList(phost, instance->ramGOffset);
	EVE_Cmd_waitFlush(phost);
}

void EVE_Ext_CallList_Replay_Repeat(EVE_HalContext* phost, t_EVE_Ext_CallList* instance) {
    EVE_CoCmd_callList(phost, instance->ramGOffset);
    EVE_CoCmd_swap(phost);
	EVE_Cmd_waitFlush(phost);
}

void EVE_Ext_CallList_Replay_Entry(EVE_HalContext* phost, t_EVE_Ext_CallList* instance, int nth, char* data) {
    if (nth >= instance->entryNumber) {
        return;
    }

    int startAddr = 0;
    for (; startAddr < instance->entries[nth].entrySize; startAddr++) {
        EVE_Hal_wr8(phost, instance->ramGOffset + instance->entries[nth].entryOffset + startAddr, data[startAddr]);
    }
}
void EVE_Ext_CallList_Replay_Entry_SPACE(EVE_HalContext* phost, t_EVE_Ext_CallList* instance, int nth, char* data) {
    EVE_Ext_CallList_Replay_Entry(phost, instance, nth, data);

    // fill space
    int addr = strlen(data);
    for (; addr < instance->entries[nth].entrySize; addr++) {
        EVE_Hal_wr8(phost, instance->ramGOffset + instance->entries[nth].entryOffset + addr, ' ');
    }
    // null terminate
    EVE_Hal_wr8(phost, instance->ramGOffset + instance->entries[nth].entryOffset + addr - 1, 0);
}

void EVE_Ext_CallList_Replay_Entry_FILL0(EVE_HalContext* phost, t_EVE_Ext_CallList* instance, int nth, char* data) {
    EVE_Ext_CallList_Replay_Entry(phost, instance, nth, data);

    // fill space
    int addr = strlen(data);
    for (; addr < instance->entries[nth].entrySize; addr++) {
        EVE_Hal_wr8(phost, instance->ramGOffset + instance->entries[nth].entryOffset + addr, 0);
    }
    // null terminate
    EVE_Hal_wr8(phost, instance->ramGOffset + instance->entries[nth].entryOffset + addr - 1, 0);
}
#endif // #define INCLUDE_CALLIST
