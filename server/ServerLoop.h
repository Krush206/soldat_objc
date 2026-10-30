#ifndef SERVERLOOP_H
#define SERVERLOOP_H

#ifdef __cplusplus
extern "C" {
#endif

/* Main server loop functions */
void AppOnIdle(void);
void UpdateFrame(void);

#ifdef __cplusplus
}
#endif

#endif /* SERVERLOOP_H */