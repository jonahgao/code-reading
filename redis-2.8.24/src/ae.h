/* A simple event-driven programming library. Originally I wrote this code
 * for the Jim's event-loop (Jim is a Tcl interpreter) but later translated
 * it in form of a library for easy reuse.
 *
 * Copyright (c) 2006-2012, Salvatore Sanfilippo <antirez at gmail dot com>
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 *   * Redistributions of source code must retain the above copyright notice,
 *     this list of conditions and the following disclaimer.
 *   * Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in the
 *     documentation and/or other materials provided with the distribution.
 *   * Neither the name of Redis nor the names of its contributors may be used
 *     to endorse or promote products derived from this software without
 *     specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef __AE_H__
#define __AE_H__

#define AE_OK 0
#define AE_ERR -1

#define AE_NONE 0
#define AE_READABLE 1
#define AE_WRITABLE 2

#define AE_FILE_EVENTS 1
#define AE_TIME_EVENTS 2
#define AE_ALL_EVENTS (AE_FILE_EVENTS|AE_TIME_EVENTS)
#define AE_DONT_WAIT 4

#define AE_NOMORE -1

/* Macros */
#define AE_NOTUSED(V) ((void) V)

struct aeEventLoop;

/* Types and data structures */
typedef void aeFileProc(struct aeEventLoop *eventLoop, int fd, void *clientData, int mask);
typedef int aeTimeProc(struct aeEventLoop *eventLoop, long long id, void *clientData);
typedef void aeEventFinalizerProc(struct aeEventLoop *eventLoop, void *clientData);
typedef void aeBeforeSleepProc(struct aeEventLoop *eventLoop);

/* File event structure */
typedef struct aeFileEvent {
    int mask; /* one of AE_(READABLE|WRITABLE) */   // 检测该fd的什么事件（可读可写）。创建时初始值 AE_NONE，表示该fd没有注册，
    aeFileProc *rfileProc;      // 可读时的回调
    aeFileProc *wfileProc;      // 可写时的回调
    void *clientData;           // 回调参数
} aeFileEvent;

/* Time event structure */
typedef struct aeTimeEvent {
    long long id; /* time event identifier. */
    long when_sec; /* seconds */    // deadline,绝对时间点
    long when_ms; /* milliseconds */    // deadline, 绝对时间点
    aeTimeProc *timeProc;   // 超时回调
    aeEventFinalizerProc *finalizerProc;    // 删除时的回调
    void *clientData;
    struct aeTimeEvent *next;
} aeTimeEvent;

/* A fired event */
/* epoll_wait 返回的新事件结构，mask表示新事件类型可读还是可写 */
typedef struct aeFiredEvent {
    int fd;
    int mask;
} aeFiredEvent;


/* loop中 fd 的流程：
 * epoll_wait返回的新事件保存到 fired 数组中，fired元素的mask表示新事件类型
 * 根据fired元素的fd去索引events，再查找fd对应的读写回调
 */
/* State of an event based program */
typedef struct aeEventLoop {
    int maxfd;   /* highest file descriptor currently registered */  // 当前放fd集合中最大的fd，增删fd时更新；用途：安全reSize
    int setsize; /* max number of file descriptors tracked*/    // fd集合的最大长度，初始时给定，配置修改时更新
    long long timeEventNextId;  // 递增的定时器id
    time_t lastTime;     /* Used to detect system clock skew */ // 监测时间跳跃
    aeFileEvent *events; /* Registered events */                // 长度为setsize的数组, 索引就是fd
    aeFiredEvent *fired; /* Fired events */                     // 最大长度为setsize的数组，保存待处理的新事件
    aeTimeEvent *timeEventHead;  // 定时器单链表
    int stop;   // 停止标记，aeStop时置为1                                                
    void *apidata; /* This is used for polling API specific data */  // 平台相关的eventloop实现，epoll对应aeApiState结构
    aeBeforeSleepProc *beforesleep;
} aeEventLoop;

/* Prototypes */
aeEventLoop *aeCreateEventLoop(int setsize);
void aeDeleteEventLoop(aeEventLoop *eventLoop);
void aeStop(aeEventLoop *eventLoop);
int aeCreateFileEvent(aeEventLoop *eventLoop, int fd, int mask,
        aeFileProc *proc, void *clientData);                        // 新增fd
void aeDeleteFileEvent(aeEventLoop *eventLoop, int fd, int mask);   // 删除修改fd监控事件
int aeGetFileEvents(aeEventLoop *eventLoop, int fd);                // 返回该fd注册监测了哪些事件(mask)
long long aeCreateTimeEvent(aeEventLoop *eventLoop, long long milliseconds,
        aeTimeProc *proc, void *clientData,
        aeEventFinalizerProc *finalizerProc);
int aeDeleteTimeEvent(aeEventLoop *eventLoop, long long id);
int aeProcessEvents(aeEventLoop *eventLoop, int flags);
int aeWait(int fd, int mask, long long milliseconds);
void aeMain(aeEventLoop *eventLoop);
char *aeGetApiName(void);
void aeSetBeforeSleepProc(aeEventLoop *eventLoop, aeBeforeSleepProc *beforesleep);
int aeGetSetSize(aeEventLoop *eventLoop);
int aeResizeSetSize(aeEventLoop *eventLoop, int setsize);

#endif
