#ifndef _DUMP_UTILS_H
#define _DUMP_UTILS_H

// ------------------ SERIAL CONFIG --------------------//
#define PRINTDEBUG true

#if PRINTDEBUG

#define SERIALDEBUG Serial
#define DUMPS(s)                                                               \
	{                                                                          \
		SERIALDEBUG.print(F(s));                                               \
	}
#define DUMPSLN(s)                                                             \
	{                                                                          \
		SERIALDEBUG.println(F(s));                                             \
	}
#define DUMPPRINTLN()                                                          \
	{                                                                          \
		SERIALDEBUG.println();                                                 \
	}
#define DUMPV(v)                                                               \
	{                                                                          \
		SERIALDEBUG.print(v);                                                  \
	}
#define DUMPLN(s, v)                                                           \
	{                                                                          \
		DUMPS(s);                                                              \
		DUMPV(v);                                                              \
		DUMPPRINTLN();                                                         \
	}
#define DUMP(s, v)                                                             \
	{                                                                          \
		DUMPS(s);                                                              \
		DUMPV(v);                                                              \
	}

#else
#define DUMPS(s)                                                               \
	{                                                                          \
	}
#define DUMPSLN(s)                                                             \
	{                                                                          \
	}
#define DUMPPRINTLN()                                                          \
	{                                                                          \
	}
#define DUMPV(v)                                                               \
	{                                                                          \
	}
#define DUMPLN(s, v)                                                           \
	{                                                                          \
	}
#define DUMP(s, v)                                                             \
	{                                                                          \
	}

#endif

#define DANGER_COLOR "#cc3333"
#define SUCCESS_COLOR "#33cc66"
#define PENDING_COLOR "#6633dd"
#define SELECTED_COLOR "#1165aa"

#endif