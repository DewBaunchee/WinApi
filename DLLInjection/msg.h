#ifndef DLLINJECTION_MSG_H
#define DLLINJECTION_MSG_H

#ifdef DLL_EXPORT
#define DECLDIR __declspec(dllexport)
#else
#define DECLDIR __declspec(dllimport)
#endif

#ifdef __cplusplus
extern "C"
{
#endif

DECLDIR void Share();

#ifdef __cplusplus
}
#endif

#endif
