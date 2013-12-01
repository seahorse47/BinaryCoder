#ifndef BINARYCODER_STDHEADERS_H_
#define BINARYCODER_STDHEADERS_H_

#include <stdarg.h>
#include <assert.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <float.h>

#include <string>
#include <vector>
#include <list>

#ifdef WIN32
/* VS 2010 (_MSC_VER 1600) has stdint.h */
#   if defined (_MSC_VER) && _MSC_VER < 1600
typedef     signed __int8       int8_t;
typedef     unsigned __int8     uint8_t;
typedef     signed __int16      int16_t;
typedef     unsigned __int16    uint16_t;
typedef     signed __int32      int32_t;
typedef     unsigned __int32    uint32_t;
typedef     signed __int64      int64_t;
typedef     unsigned __int64    uint64_t;
#   else
#   include <stdint.h>
#   endif
#else
#   include <stdint.h>
#endif

using std::string;

#endif /* end of BINARYCODER_STDHEADERS_H_ */
