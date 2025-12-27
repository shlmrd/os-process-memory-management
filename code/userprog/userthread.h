#ifdef CHANGED

#ifndef USERTHREAD_H
#define USERTHREAD_H

extern int do_ThreadCreate(int f, int arg);
extern void do_ThreadExit();
extern int do_ForkExec(const char *s);

#endif // USERTHREAD_H
#endif //CHANGED