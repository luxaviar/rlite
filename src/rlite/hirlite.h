#ifndef __HIRLITE_H
#define __HIRLITE_H
#include <stdio.h> /* for size_t */
#include <stdarg.h> /* for va_list */
#include "rlite/port/time.h" /* for struct timeval */

#include "rlite.h"

#ifdef _MSC_VER
#   ifdef _BUILDING_HIRLITE_DLL
#       define RLITE_API __declspec(dllexport)
#   else
#       ifdef _USE_HIRLITE_STATIC
#           define RLITE_API 
#       else
#           define RLITE_API __declspec(dllimport)
#       endif
#   endif
#else
#   define RLITE_API 
#endif

#define HIRLITE_MAJOR 0
#define HIRLITE_MINOR 1
#define HIRLITE_PATCH 1

#define RLITE_ERR -1
#define RLITE_OK 0

/* When an error occurs, the err flag in a context is set to hold the type of
 * error that occured. RLITE_ERR_IO means there was an I/O error and you
 * should use the "errno" variable to find out what is wrong.
 * For other values, the "errstr" field will hold a description. */
#define RLITE_ERR_IO 1 /* Error in read or write */
#define RLITE_ERR_EOF 3 /* End of file */
#define RLITE_ERR_PROTOCOL 4 /* Protocol error */
#define RLITE_ERR_OOM 5 /* Out of memory */
#define RLITE_ERR_OTHER 2 /* Everything else... */

 /* Connection type can be blocking or non-blocking and is set in the
  * least significant bit of the flags field in rliteContext. */
#define RLITE_BLOCK 0x1

  /* Connection may be disconnected before being free'd. The second bit
   * in the flags field is set when the context is connected. */
#define RLITE_CONNECTED 0x2
#define RLITE_LUA_CLIENT (1<<8) /* This is a non connected client used by Lua */

   /* The async API might try to disconnect cleanly and flush the output
	* buffer and read all subsequent replies before disconnecting.
	* This flag means no new commands can come in and the connection
	* should be terminated once all replies have been read. */
#define RLITE_DISCONNECTING 0x4

	/* Flag specific to the async API which means that the context should be clean
	 * up as soon as possible. */
#define RLITE_FREEING 0x8

	 /* Flag that is set when an async callback is executed. */
#define RLITE_IN_CALLBACK 0x10

/* Flag that is set when the async context has one or more subscriptions. */
#define RLITE_SUBSCRIBED 0x20

/* Flag that is set when monitor mode is active */
#define RLITE_MONITORING 0x40

#define RLITE_REPLY_STRING 1
#define RLITE_REPLY_ARRAY 2
#define RLITE_REPLY_INTEGER 3
#define RLITE_REPLY_NIL 4
#define RLITE_REPLY_STATUS 5
#define RLITE_REPLY_ERROR 6

#define RLITE_READER_MAX_BUF (1024*16)  /* Default max unused reader buffer. */

#define RLITE_KEEPALIVE_INTERVAL 15 /* seconds */

#define RLITE_CMD_WRITE 1                   /* "w" flag */
#define RLITE_CMD_READONLY 2                /* "r" flag */
#define RLITE_CMD_DENYOOM 4                 /* "m" flag */
#define RLITE_CMD_NOT_USED_1 8              /* no longer used flag */
#define RLITE_CMD_ADMIN 16                  /* "a" flag */
#define RLITE_CMD_PUBSUB 32                 /* "p" flag */
#define RLITE_CMD_NOSCRIPT  64              /* "s" flag */
#define RLITE_CMD_RANDOM 128                /* "R" flag */
#define RLITE_CMD_SORT_FOR_SCRIPT 256       /* "S" flag */
#define RLITE_CMD_LOADING 512               /* "l" flag */
#define RLITE_CMD_STALE 1024                /* "t" flag */
#define RLITE_CMD_SKIP_MONITOR 2048         /* "M" flag */
#define RLITE_CMD_ASKING 4096               /* "k" flag */
#define RLITE_CMD_FAST 8192                 /* "F" flag */

#ifdef __cplusplus
extern "C" {
#endif

	struct rliteClient;

	/* This is the reply object returned by rliteCommand() */
	typedef struct RLITE_API rliteReply {
		int type; /* RLITE_REPLY_* */
		int64_t integer; /* The integer when type is RLITE_REPLY_INTEGER */
		int len; /* Length of string */
		char* str; /* Used for both RLITE_REPLY_ERROR and RLITE_REPLY_STRING */
		size_t elements; /* number of elements, for RLITE_REPLY_ARRAY */
		struct rliteReply** element; /* elements vector for RLITE_REPLY_ARRAY */
	} rliteReply;

	/* Function to free the reply objects hirlite returns by default. */
	RLITE_API void rliteFreeReplyObject(void* reply);

	/* Functions to format a command according to the protocol. */
	RLITE_API int rlitevFormatCommand(struct rliteClient* client, const char* format, va_list ap);
	RLITE_API int rliteFormatCommand(struct rliteClient* client, const char* format, ...);
	RLITE_API int rliteFormatCommandArgv(struct rliteClient* client, int argc, char** argv, size_t* argvlen);

	struct rliteClient;

	/* Context for a connection to Redis */
	typedef struct RLITE_API rliteContext {
		int err; /* Error flags, 0 when there is no error */
		char errstr[128]; /* String representation of error when applicable */
		char* path;
		rliteReply** replies;
		int replyPosition; // avoid reallocing the pointer after removing an element, keep a reference of the next position
		int replyLength;
		int replyAlloc;
		rlite* db;
		int debugSkiplist;
		int cluster_enabled;
		size_t hashtableLimitEntries;
		size_t hashtableLimitValue;
		int inLuaScript;
		void (*writeCommand)(int dbid, int argc, char** argv, size_t* argvlen);

		short inTransaction;
		short transactionFailed;
		size_t watchedKeysAlloc;
		size_t watchedKeysLength;
		struct watched_key** watchedKeys;
		size_t enqueuedCommandsAlloc;
		size_t enqueuedCommandsLength;
		struct rliteClient** enqueuedCommands;
	} rliteContext;

	RLITE_API rliteContext* rliteConnect(const char* ip, int port);
	RLITE_API rliteContext* rliteConnectWithTimeout(const char* ip, int port, const struct timeval tv);
	RLITE_API rliteContext* rliteConnectNonBlock(const char* ip, int port);
	RLITE_API rliteContext* rliteConnectBindNonBlock(const char* ip, int port, const char* source_addr);
	RLITE_API rliteContext* rliteConnectUnix(const char* path);
	RLITE_API rliteContext* rliteConnectUnixWithTimeout(const char* path, const struct timeval tv);
	RLITE_API rliteContext* rliteConnectUnixNonBlock(const char* path);
	RLITE_API rliteContext* rliteConnectFd(int fd);
	RLITE_API int rliteSetTimeout(rliteContext* c, const struct timeval tv);
	RLITE_API int rliteEnableKeepAlive(rliteContext* c);
	RLITE_API void rliteFree(rliteContext* c);
	RLITE_API int rliteFreeKeepFd(rliteContext* c);
	RLITE_API int rliteBufferRead(rliteContext* c);
	RLITE_API int rliteBufferWrite(rliteContext* c, int* done);

	/* In a blocking context, this function first checks if there are unconsumed
	 * replies to return and returns one if so. Otherwise, it flushes the output
	 * buffer to the socket and reads until it has a reply. In a non-blocking
	 * context, it will return unconsumed replies until there are no more. */
	RLITE_API int rliteGetReply(rliteContext* c, void** reply);
	RLITE_API int rliteGetReplyFromReader(rliteContext* c, void** reply);

	/* Write a formatted command to the output buffer. Use these functions in blocking mode
	 * to get a pipeline of commands. */
	RLITE_API int rliteAppendFormattedCommand(rliteContext* c, const char* cmd, size_t len);

	/* Write a command to the output buffer. Use these functions in blocking mode
	 * to get a pipeline of commands. */
	RLITE_API int rlitevAppendCommand(rliteContext* c, const char* format, va_list ap);
	RLITE_API int rliteAppendCommand(rliteContext* c, const char* format, ...);
	RLITE_API int rliteAppendCommandArgv(rliteContext* c, int argc, char** argv, size_t* argvlen);
	RLITE_API int rliteAppendCommandClient(struct rliteClient* client);

	/* Issue a command to Redis. In a blocking context, it is identical to calling
	 * rliteAppendCommand, followed by rliteGetReply. The function will return
	 * NULL if there was an error in performing the request, otherwise it will
	 * return the reply. In a non-blocking context, it is identical to calling
	 * only rliteAppendCommand and will always return NULL. */
	 // when using %b, the numbers must be size_t (not int)
	RLITE_API void* rlitevCommand(rliteContext* c, const char* format, va_list ap);
	RLITE_API void* rliteCommand(rliteContext* c, const char* format, ...);
	RLITE_API void* rliteCommandArgv(rliteContext* c, int argc, char** argv, size_t* argvlen);

	RLITE_API struct rliteCommand* rliteLookupCommand(const char* name, size_t len);
	RLITE_API int rliteCommandHasFlag(struct rliteCommand* cmd, int flag);

	RLITE_API int getLongLongFromObjectOrReply(struct rliteClient* c, const char* o, size_t len, int64_t* target, const char* msg);
	RLITE_API rliteReply* createArrayObject(size_t size);
	RLITE_API rliteReply* createStringTypeObject(int type, const char* str, const int len);
	RLITE_API rliteReply* createStringObject(const char* str, const int len);
	RLITE_API rliteReply* createCStringObject(const char* str);
	RLITE_API rliteReply* createErrorObject(const char* str);
	RLITE_API rliteReply* createStatusObject(const char* str);
	RLITE_API rliteReply* createDoubleObject(double d);
	RLITE_API rliteReply* createLongLongObject(int64_t value);
	RLITE_API rliteReply* createNullReplyObject();

	typedef struct RLITE_API rliteClient {
		int argc;
		char** argv;
		size_t* argvlen;
		rliteReply* reply;
		rliteContext* context;
		int flags;
	} rliteClient;

	typedef void rliteCommandProc(rliteClient* c);

	struct RLITE_API rliteCommand {
		char* name;
		rliteCommandProc* proc;
		int arity;
		char* sflags; /* Flags as string representation, one char per flag. */
		int flags;    /* The actual flags, obtained from the 'sflags' field. */
		/* Use a function to determine keys arguments in a command line.
		 * Used for Redis Cluster redirect. */
		 // rliteGetKeysProc *getkeys_proc;
		 /* What keys should be loaded in background when calling this command? */
		int firstkey; /* The first argument that's a key (0 = no keys) */
		int lastkey;  /* The last argument that's a key */
		int keystep;  /* The step between first and last key */
		int64_t microseconds, calls;
	};

#ifdef __cplusplus
}
#endif

#endif
