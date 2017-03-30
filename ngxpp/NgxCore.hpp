// Copyright (c) 2015-2017
// Author: Chrono Law
#ifndef _NGX_CORE_HPP
#define _NGX_CORE_HPP

// namespace define
#define NGX_NAMESPACE_BEGIN         namespace ngx {
#define NGX_NAMESPACE_END           }
#define USING_NGX_NAMESPACE         using namespace ngx;

NGX_NAMESPACE_BEGIN

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
#include "NgxChain.hpp"
#include "NgxKeyValue.hpp"
//#include "NgxFile.hpp"

#include "NgxConfig.hpp"

#include "NgxDigest.hpp"
#include "NgxTimer.hpp"

#include "NgxEvent.hpp"
#include "NgxConnection.hpp"
#include "NgxGlobal.hpp"

#if (NGX_THREADS)
#include "NgxThread.hpp"
#endif  //NGX_THREADS

NGX_NAMESPACE_END

// open namespace ngx
USING_NGX_NAMESPACE

#endif  //_NGX_CORE_HPP
