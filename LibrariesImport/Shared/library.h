#ifndef SHARED_LIBRARY_H
#define SHARED_LIBRARY_H

#ifdef __cplusplus
extern "C" {
#endif

__declspec( dllexport ) int dynamicPow(int, int);

#ifdef __cplusplus
}
#endif

#endif //SHARED_LIBRARY_H
