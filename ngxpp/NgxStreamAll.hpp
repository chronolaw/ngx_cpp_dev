// Copyright (c) 2016
// Author: Chrono Law
#ifndef _NGX_STREAM_ALL_HPP
#define _NGX_STREAM_ALL_HPP

#include "NginxStream.hpp"

#include "NgxWrapper.hpp"

#include "NgxValue.hpp"
#include "NgxException.hpp"
#include "NgxPool.hpp"
#include "NgxAllocator.hpp"
#include "NgxString.hpp"
#include "NgxLog.hpp"
#include "NgxClock.hpp"
//#include "NgxDatetime.hpp"

#include "NgxArray.hpp"
#include "NgxList.hpp"
#include "NgxQueue.hpp"
#include "NgxRbtree.hpp"
#include "NgxBuf.hpp"
#include "NgxChain.hpp"
#include "NgxKeyValue.hpp"
//#include "NgxFile.hpp"

#include "NgxConfig.hpp"

#include "NgxStreamModule.hpp"
#include "NgxStreamSession.hpp"

#include "NgxDigest.hpp"
#include "NgxTimer.hpp"

#if (nginx_version >= 1011002)
#include "NgxVariable.hpp"
//#include "NgxScript.hpp"
#endif

#include "NgxEvent.hpp"
#include "NgxConnection.hpp"
#include "NgxGlobal.hpp"

#if (NGX_THREADS)
#include "NgxThread.hpp"
#endif  //NGX_THREADS

#endif  // _NGX_STREAM_ALL_HPP

