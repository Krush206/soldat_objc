/* FileClient — C port of client/FileClient.pas */

#ifndef FILECLIENT_H
#define FILECLIENT_H

#include "Classes.h"
#include "Sha1.h"

#define MAX_DL_SIZE 150000000

typedef struct TDownloadThread TDownloadThread;

TDownloadThread *TDownloadThread_Create(const char *download_url, const char *name, TSHA1Digest checksum);
void TDownloadThread_Free(TDownloadThread *thread);
void TDownloadThread_CancelDownload(TDownloadThread *thread);
void TDownloadThread_Execute(TDownloadThread *thread);

extern unsigned char DownloadRetry;

#endif /* FILECLIENT_H */