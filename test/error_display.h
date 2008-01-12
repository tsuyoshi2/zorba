
#ifndef ERROR_DISPLAY_H
#define ERROR_DISPLAY_H

struct xqp::ZorbaAlert;
struct xqp::ZorbaError;
struct xqp::ZorbaWarning;
struct xqp::ZorbaNotify;
struct xqp::ZorbaAskUser;
struct xqp::ZorbaFnError;
struct xqp::ZorbaFnTrace;

extern void DisplayErrorListForCurrentThread();

extern void DisplayOneAlert(const xqp::ZorbaAlert *alert);
extern void DisplayError(const xqp::ZorbaError *err);
extern void DisplayWarning(const xqp::ZorbaWarning *warn);
extern void DisplayNotification(const xqp::ZorbaNotify *notif);
extern int DisplayAskUser(const xqp::ZorbaAskUser *askuser);
extern void DisplayFnUserError(const xqp::ZorbaFnError *fn_err);
extern void DisplayFnUserTrace(const xqp::ZorbaFnTrace *fn_trace);

#endif
