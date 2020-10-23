//
// Author: Vladimir Migashko <migashko@gmail.com>, (C) 2020
//
// Copyright: See COPYING file that comes with this distribution
//

#pragma once

#ifndef BTP_DISABLE_LOG

#include <wlog/logging.hpp>

#define BTP_LOG_ERROR(X)    WLOG_LOG_ERROR( "BTP", X )
#define BTP_LOG_WARNING(X)  WLOG_LOG_WARNING( "BTP", X )
#define BTP_LOG_MESSAGE(X)  WLOG_LOG_MESSAGE( "BTP", X )
#define BTP_LOG_FATAL(X)    WLOG_LOG_FATAL( "BTP", X )
#define BTP_LOG_BEGIN(X)    WLOG_LOG_BEGIN( "BTP", X )
#define BTP_LOG_END(X)      WLOG_LOG_END( "BTP", X )
#define BTP_LOG_DEBUG(X)    WLOG_LOG_DEBUG( "BTP", X )
#define BTP_LOG_TRACE(X)    WLOG_LOG_TRACE( "BTP", X )
#define BTP_LOG_PROGRESS(X) WLOG_LOG_PROGRESS( "BTP", X )

#else

#define BTP_LOG_ERROR(X)    
#define BTP_LOG_WARNING(X)  
#define BTP_LOG_MESSAGE(X)  
#define BTP_LOG_FATAL(X)    
#define BTP_LOG_BEGIN(X)    
#define BTP_LOG_END(X)      
#define BTP_LOG_DEBUG(X)    
#define BTP_LOG_TRACE(X)    
#define BTP_LOG_PROGRESS(X) 

#endif
