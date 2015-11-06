
#define	TRACEINIT()
#define	TRACETERM()

#ifndef TRACE

#define	TRACEOUT(a)

#else

#ifdef	__cplusplus
extern "C" {
#endif

void trace_fmt(const char *str, ...);

#define	TRACEOUT(arg)	trace_fmt arg

#ifdef	__cplusplus
}
#endif

#endif

