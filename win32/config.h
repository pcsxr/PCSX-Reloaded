//
// Copyright (c) 2008, Wei Mingzhi. All rights reserved.
//
// Use, redistribution and modification of this code is unrestricted as long as this
// notice is preserved.
//

#ifndef CONFIG_H
#define CONFIG_H

#ifndef __i386__
#define __i386__ 1
#endif

#ifndef MAXPATHLEN
#define MAXPATHLEN 256
#endif

#ifndef PACKAGE_VERSION
#define PACKAGE_VERSION ""
#endif

#ifndef PREFIX
#define PREFIX "./"
#endif

#ifdef _DEBUG
#define inline /* */
#else
#define inline __forceinline
#endif

#ifdef _MSC_VER
#pragma warning (disable:4133)
#pragma warning (disable:4244)
#pragma warning (disable:4996)
#endif

#endif
