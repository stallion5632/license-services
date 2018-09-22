#pragma once

#if (!defined(ROCK_LIB) && !defined(ROCK_LINUX_PLATFORM))
// ROCK is used as DLL
    #define ROCK_DLLEXPORT __declspec(dllexport) 
    #define ROCK_DLLIMPORT __declspec(dllimport) 
#else 
    #define ROCK_DLLEXPORT 
    #define ROCK_DLLIMPORT
#endif

#ifdef ROCK_LOG_EXPORTS
    #define ROCK_LOG_API ROCK_DLLEXPORT  
#else 
    #define ROCK_LOG_API ROCK_DLLIMPORT 
#endif

#ifdef ROCK_NET_EXPORTS
    #define ROCK_NET_API  ROCK_DLLEXPORT  
#else 
    #define ROCK_NET_API  ROCK_DLLIMPORT 
#endif

#ifdef ROCK_DOG_EXPORTS
	#define ROCK_DOG_API ROCK_DLLEXPORT  
#else 
	#define ROCK_DOG_API ROCK_DLLIMPORT 
#endif


#ifdef ROCK_HANDLER_EXPORTS
    #define ROCK_HANDLER_API ROCK_DLLEXPORT  
#else 
    #define ROCK_HANDLER_API ROCK_DLLIMPORT 
#endif

#ifdef ROCK_JSONMESSAGE_EXPORTS
#define ROCK_JSONMESSAGE_API ROCK_DLLEXPORT  
#else 
#define ROCK_JSONMESSAGE_API ROCK_DLLIMPORT 
#endif
