#pragma once
// 该文件中字段定义对应于message_types.json，必须保证key完全一致（包括大小写）！

#include <stdint.h>

static const char* kMessageTypes	= "message_types";
static const char* kSyncBytes		= "sync_bytes";

static const char* kLoginReq		= "login_req";
static const char* kLoginRsp		= "login_rsp";

static const char* kHeartReq		= "heart_req";
static const char* kHeartRsp		= "heart_rsp";

static const char* kLogoutReq 		= "logout_req";
static const char* kLogoutRsp 		= "logout_rsp";

static const char* kGetDogStatusReq = "get_dogstatus_req";
static const char* kGetDogStatusRsp = "get_dogstatus_rsp";

static const char* kDogStatusNtf	= "dog_status_ntf";