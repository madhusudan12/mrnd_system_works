#pragma once

#ifdef MEMORYMANAGER_EXPORTS
#define MEMORYMANAGER_API __declspec(dllexport)
#else
#define MEMORYMANAGER_API __declspec(dllimport)
#endif


extern "C" MEMORYMANAGER_API void* ms_malloc(size_t size);

extern "C" MEMORYMANAGER_API void ms_free(void *ptr);

extern "C" MEMORYMANAGER_API void ms_debug();