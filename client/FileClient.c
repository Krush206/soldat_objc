/* FileClient — C port of client/FileClient.pas */
#include "FileClient.h"
#include "Classes.h"
#include "Sha1.h"
#include <stdlib.h>

unsigned char DownloadRetry = 0;

struct TDownloadThread {
    Thread *base;
    char *FURL;
    char *FFilename;
    TSHA1Digest FChecksum;
    char *FErrorMsg;
    unsigned char FStatus;
    unsigned char FProgress;
    long long FDownloadPos;
    long long FDownloadSize;
    void *Client;
};

TDownloadThread *TDownloadThread_Create(const char *download_url, const char *name, TSHA1Digest checksum) {
    (void)download_url; (void)name; (void)checksum;
    TDownloadThread *thread = calloc(1, sizeof(TDownloadThread));
    if (thread) {
        thread->base = Thread_Create();
    }
    return thread;
}

void TDownloadThread_Free(TDownloadThread *thread) {
    if (!thread) return;
    if (thread->base) Thread_Free(thread->base);
    free(thread->FURL);
    free(thread->FFilename);
    free(thread->FErrorMsg);
    free(thread);
}

void TDownloadThread_CancelDownload(TDownloadThread *thread) {
    (void)thread;
}

void TDownloadThread_Execute(TDownloadThread *thread) {
    (void)thread;
}