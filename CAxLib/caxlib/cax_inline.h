#ifdef CAX_INLINE
#undef CAX_INLINE
#endif

#ifndef  CAX_STATIC_LIB
#define CAX_INLINE inline
#else
#define CAX_INLINE
#endif
