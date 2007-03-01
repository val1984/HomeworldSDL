/*=============================================================================
    File.h:  Definitions for file I/O.  This includes .BIG files and regular
        files.

    Created June 1997 by Luke Moloney
=============================================================================*/

#ifndef ___FILE_H
#define ___FILE_H

#include <stdio.h>
#include "BigFile.h"

/*=============================================================================
    Switches
=============================================================================*/
#ifdef HW_BUILD_FOR_DEBUGGING

#define FILE_VERBOSE_LEVEL      1               //control level of verbose info
#define FILE_ERROR_CHECKING     1               //control error checking
#define FILE_OPEN_LOGGING       1               //display file names as they are opened
#define FILE_SEEK_WARNING       1               //display warnings as seeks are required
#define FILE_TEST               0               //test the file module
#define FILE_PREPEND_PATH       1               //prepend a fixed path to the start of all file open requests

#else

#define FILE_VERBOSE_LEVEL      0               //control level of verbose info
#define FILE_ERROR_CHECKING     0               //control error checking
#define FILE_OPEN_LOGGING       0               //display file names as they are opened
#define FILE_SEEK_WARNING       0               //display warnings as seeks are required
#define FILE_TEST               0               //test the file module
#define FILE_PREPEND_PATH       1               //prepend a fixed path to the start of all file open requests

#endif

// If not already defined (such as through a configure setting), define
// whether or not case-insensitive searches for files should be performed
// based on the target platform.
#ifndef FILE_CASE_INSENSITIVE_SEARCH

#if defined (_WIN32) || defined (_MACOSX)
#define FILE_CASE_INSENSITIVE_SEARCH 0
#else
#define FILE_CASE_INSENSITIVE_SEARCH 1
#endif  // defined (_WIN32) || defined (_MACOSX)

#endif  // !defined (FILE_CASE_INSENSITIVE_SEARCH)

/*=============================================================================
    Definitions:
=============================================================================*/

//file seek locations
#define FS_Start                SEEK_SET
#define FS_Current              SEEK_CUR
#define FS_End                  SEEK_END

//results of file stream operations
#define FR_EndOfFile            -12             // some unlikely number to indicate end of file

//flags for opening files
#define FF_TextMode             0x0001          // open file in text mode
#define FF_IgnoreDisk           0x0002          // don't search on disk, look straight in the .BIG file
#define FF_DontUse0             0x0004          // These bits are reserved for flag compatability with memory module
#define FF_DontUse1             0x0008
#define FF_WriteMode            0x0010          // open file for writing to
#define FF_AppendMode           0x0020          // open file for appending to
#define FF_ReturnNULLOnFail     0x0040          // open file, but if fail return NULL instead of doing Fatal Error
#define FF_CDROM                0x0080          // open from CD-ROM
#define FF_IgnoreBIG            0x0100          // don't look in .BIG file, only try to load from disk
#define FF_IgnorePrepend        0x0200          // don't environment root path to beginning of file names
#define FF_UserSettingsPath     0x0400          // use user configuration root as the base path
#define FF_HomeworldRootPath    0x0800          // Homeworld's root directory

//names of log files
#define FN_OpenLog              "open.log"

// for /logFileLoads option
#define FILELOADSLOG            "FileLoads.log"

//.BIG file definitions
#define FBF_AllBIGFiles         -1

//length of file path fragments
#define FL_Path                 256             //max length of a full path

#define MAX_FILES_OPEN       32

/*=============================================================================
    Type definitions:
=============================================================================*/
typedef sdword  filehandle;

typedef struct {
    sdword inUse;
    char path[PATH_MAX];    // full path name of open file
    sdword usingBigfile;    // true if this has been "opened" within the bigfile instead of on disk
    FILE *fileP;            // valid if we've opened directly from disk
    // valid when usingBigfile:
    FILE *bigFP;            // could be using either the main
    bigTOC *bigTOC;         //  or updated bigfile
    sdword textMode;        // true/false
    long offsetStart;       // start of file in bigfile
    char *decompBuf;        // If necessary, a compressed file will be decompressed to this buf (upon opening it) to fake streamed i/o.
                            // So if this is non-NULL, offsetVirtual is a pointer into this buffer.
                            // This will be NULL if we're just re-directing the stream to the bigfile (ie, for an uncompressed file).
                            // If possible this will either point to an existing workspace that persists between file loads
                            //  (to avoid extra allocs/deallocs), or else to a newly allocated buffer that will be freed upon
                            //  "closing" the current file).
    long offsetVirtual;     // this tracks the virtual stream offset (from offsetStart), like stdio (0 = start, etc.)
    long length;            // length of file in bigfile (uncompressed length)
} fileOpenInfo;

/*=============================================================================
    Functions:
=============================================================================*/

//specify a path for all file open requests
#if FILE_PREPEND_PATH
void filePrependPathSet(char *path);
char *filePathPrepend(char *path, udword flags);
void fileCDROMPathSet(char *path);
void fileUserSettingsPathSet(char *path);
#else
#define filePrependPathSet(p)
#define filePathPrepend(p,f)
#define fileCDROMPathSet(p)
#define fileUserSettingsPathSet(p)
#endif

extern char filePrependPath[];
extern char fileHomeworldRootPath[];
extern char fileCDROMPath[];
extern char fileUserSettingsPath[];

//load files directly into memory
sdword fileLoadAlloc(char *fileName, void **address, udword flags);
sdword fileLoad(char *fileName, void *address, udword flags);

//save files, if you want stream saving, use the ANSI C stream functions
sdword fileSave(char *fileName, void *address, sdword length);

//delete a file
void fileDelete(char *fileName);

//load files like ANSI C streams (fopen, fread etc)
filehandle fileOpen(char *fileName, udword flags);
void fileClose(filehandle handle);
sdword fileSeek(filehandle handle, sdword offset, sdword whence);
sdword fileBlockRead(filehandle handle, void *dest, sdword nBytes);
sdword fileBlockReadNoError(filehandle handle, void *dest, sdword nBytes);
sdword fileLineRead(filehandle handle, char *dest, sdword nChars);
sdword fileCharRead(filehandle handle);

// for using regular underlying i/o stream (filesystem, not bigfile)
sdword fileUsingBigfile(filehandle handle);
FILE *fileStream(filehandle handle);

//utility functions
bool8 fileMakeDirectory(const char *directoryName);
bool8 fileMakeDestinationDirectory(const char *fileName);
sdword fileExistsInBigFile(char *_fileName);
sdword fileExists(char *fileName, udword flags);
sdword fileSizeGet(char *fileName, udword flags);
sdword fileSizeRemaining(filehandle handle);
sdword fileLocation(filehandle handle);
sdword fileEndoOfFile(filehandle handle);

void logfileClear(char *logfile);
void logfileLog(char *logfile,char *str);
void logfileLogf(char *logfile,char *format, ...);

#endif
