#ifndef _DEBUG_HPP_
#define _DEBUG_HPP_

//#define _DEBUG_CONSOLE_

#ifdef _DEBUG_CONSOLE_
#define DBGPRINT(...) fprintf (stdout, __VA_ARGS__)
#else
#define DBGPRINT(...) ;
#endif

#endif

