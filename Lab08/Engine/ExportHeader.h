#ifndef EXPORTHEADER_H
#define EXPORTHEADER_H

#ifdef	ENGINE_EXPORTS
#define ENGINE_SHARED __declspec( dllexport )
#define ENGINE_TEMPLATE
#else
#define ENGINE_SHARED __declspec( dllimport )
#define ENGINE_TEMPLATE extern
#endif

#endif // EXPORTHEADER_H
