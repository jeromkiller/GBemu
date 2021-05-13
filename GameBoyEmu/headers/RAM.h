#ifndef _RAM_h
#define _RAM_h

//defines
typedef unsigned char RAM;

//function prototypes
//initialize RAM
RAM* RAM_init(void);

//dispose of the ram stack
void RAM_dispose(RAM* RAM_ptr);
#endif // !RAM
