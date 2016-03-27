#ifndef COMMON_HPP__
#define COMMON_HPP__ 1

#define R_CLASS( ... )
#define R_STRUCT( ... )
#define R_METHOD( ... )
#define R_EMIT( ... )
#define R_ENUM( ... )
#define R_END( ... )

#if defined( _MSC_VER )
#define NOEXCEPT
#define ATTRIBUTE( ... )
#define _CRT_SECURE_NO_WARNINGS
#else
#define NOEXCEPT noexcept
#define ATTRIBUTE __attribute__
#endif

#endif
