#ifndef __COMM_H_
#define __COMM_H_

#define err(mess) { fprintf(stderr,"Error: %s.", mess); exit(1); }

void comm_open();
void comm_write(char *, ...);
void comm_close();

#endif
