// Copyright (c) 2015-2016
// Author: Chrono Law
#ifndef _NGX_ALL_HPP
#define _NGX_ALL_HPP

#include "Nginx.hpp"

#include "NgxWrapper.hpp"

#include "NgxValue.hpp"
#include "NgxException.hpp"
#include "NgxPool.hpp"
#include "NgxAllocator.hpp"
#include "NgxString.hpp"
#include "NgxLog.hpp"
#include "NgxClock.hpp"
#include "NgxDatetime.hpp"

#include "NgxArray.hpp"
#include "NgxList.hpp"
#include "NgxQueue.hpp"
#include "NgxRbtree.hpp"
#include "NgxBuf.hpp"
#include "NgxKeyValue.hpp"
//#include "NgxFile.hpp"

#include "NgxConfig.hpp"

#include "NgxModule.hpp"
#include "NgxHttpModule.hpp"

#include "NgxRequest.hpp"
#include "NgxFilter.hpp"
#include "NgxUpstream.hpp"
#include "NgxLoadBalance.hpp"
#include "NgxSubRequest.hpp"

#include "NgxDigest.hpp"
#include "NgxTimer.hpp"
#include "NgxVariable.hpp"
#include "NgxScript.hpp"

#include "NgxEvent.hpp"
#include "NgxGlobal.hpp"

#if (NGX_THREADS)
#include "NgxThread.hpp"
#endif  //NGX_THREADS

// nginx 1.9.0 has stream module
// must patch auto/modules to add macro NGX_STREAM
#if ((nginx_version >= 1009000) && (NGX_STREAM))

#include "NgxStreamModule.hpp"

#endif  // nginx 1.9.0

#endif  //_NGX_ALL_HPP
