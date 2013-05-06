#ifndef _PROVENANCE_H_
#define _PROVENANCE_H_

#include <stddef.h>

#ifdef __cplusplus
# define SPADE_CPP(x) x
#else
# define SPADE_CPP(x)
#endif

//============== interface for FusionFS ==============
SPADE_CPP(extern "C" {)

int spade_init();
int spade_close();
int spade_readlink(const char*, const char*, pid_t, int);
int spade_unlink(const char*, const char*, pid_t);
int spade_symlink(const char*, const char*, const char*, pid_t);
int spade_rename(const char*, const char*, const char*, pid_t, int, int);
int spade_link(const char*, const char*, const char*, pid_t);
int spade_read(const char*, const char*, pid_t, int, int);
int spade_write(const char*, const char*, pid_t, int, int);
int spade_create(const char*, const char*, pid_t);
int spade_push(const char*, const char*, pid_t, int);
// int spade_receivefile(const char*, const char*, const char*, const char*);
// int spade_sendfile(const char*, const char*, const char*);

SPADE_CPP(})


#endif
