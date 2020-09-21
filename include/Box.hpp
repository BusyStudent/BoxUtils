//包含所有组件
#ifndef _BOX_HPP_
#define _BOX_HPP_
#include <Box/backtrace.hpp>
#include <Box/assert.hpp>
#include <Box/buffer.hpp>

#include <Box/sync/channal.hpp>
#include <Box/sync/pipe.hpp>
#include <Box/sync/sem.hpp>

#include <Box/base64.hpp>

#include <Box/dataguard.hpp>
#include <Box/refptr.hpp>
//Json
#include <Box/json.hpp>
//Xml
#include <Box/xml.hpp>
#include <Box/sax.hpp>
#include <Box/xpath.hpp>

#include <Box/timer.hpp>
#include <Box/threadpool.hpp>
//Curl
#include <Box/curl/easy.hpp>
#include <Box/curl/multi.hpp>
#include <Box/curl/share.hpp>
#include <Box/curl/factory.hpp>
#include <Box/curl/headers.hpp>
#include <Box/curl/exception.hpp>
#include <Box/curl/curl.hpp>
//Net
#include <Box/net/socket.hpp>
#include <Box/net/poll.hpp>
#endif