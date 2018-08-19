// MemManager.cpp : Defines the exported functions for the DLL application.
//

#include "stdafx.h"
#include <utility>
#include <limits.h>
#include<string.h>
#include<Windows.h>
#include "Manager.h"
#define PAGE_SIZE 2048


struct Page
{
	unsigned short int size;
	char *address;
	struct Page *nextpage;
	struct Page *prevpage;
};

struct Block
{
	char *address;
	struct Page *firstpage;
	struct Block *nextblock;
};

/*
struct Page* createPage(int size)
{
	struct Page* page = (struct Page*)malloc(sizeof(struct Page));
	page->size = size;
	page->nextpage = NULL;
}
*/

struct Block *firstblock = NULL;


char* available(struct Block *currentblock,int size)
{
	struct Page *currentpage;
	char *memory = NULL;
	short int *pagesize;
	currentpage = currentblock->firstpage;
	if (currentpage == NULL)
		return NULL;
	while (currentpage != NULL)
	{
		if (currentpage->size >= size + 2)
			break;
		currentpage = currentpage->nextpage;
	}
	if (currentpage != NULL)
	{
		pagesize = (short*)currentpage->address;
		*pagesize = size;
		memory = &currentpage->address[2];
		currentpage->size = size - 2;
		currentpage->address = &currentpage->address[size + 2];
	}
	if (currentpage == currentblock->firstpage&&currentpage->size == 0)
	{
		currentblock->firstpage = currentpage->nextpage;
	}
	if (currentpage->size == 0)
	{
		struct Page *page;
		page = currentpage->prevpage;
		page->nextpage = currentpage->nextpage;
	}
	return memory;
}


struct Page* createPage(int size)
{
	struct Page *newpage= (struct Page*)malloc(sizeof(struct Page));
	newpage->nextpage = NULL;
	newpage->address = NULL;
	newpage->prevpage = NULL;
	newpage->size = size;
	return newpage;
}

struct Block* createBlock()
{
	struct Block *newblock = (struct Block*)malloc(sizeof(struct Block));
	newblock->address = (char*)malloc(PAGE_SIZE);
	newblock->firstpage = createPage(PAGE_SIZE);
	newblock->nextblock = NULL;
	newblock->firstpage->address = newblock->address;
	return newblock;
}


void* ms_malloc(size_t size)
{
	char *memory=NULL;
	Block *currentblock;
	if (firstblock == NULL)
	{
		firstblock = createBlock();
	}
	currentblock = firstblock;
	while (currentblock != NULL)
	{
		memory = available(currentblock, size);
		if (memory == NULL)
		{
			break;
		}
		currentblock = currentblock->nextblock;
	}
	if (memory == NULL)
	{
		currentblock = firstblock;
		while (currentblock->nextblock != NULL)
		{
			currentblock = currentblock->nextblock;
		}
		currentblock->nextblock = createBlock();
		memory = available(currentblock, size);
	}
	return memory;
	
}



void mergePages(struct Page *page)
{
	struct Page *currentpage;
	while (page->nextpage != NULL)
	{
		if (page->address + page->size == page->nextpage->address)
		{
			page->size = page->size + page->nextpage->size;
			currentpage = page->nextpage;
			page->nextpage = page->nextpage->nextpage;
		}
		else
			page = page->nextpage;
	}

}


void insert(struct Block* block,char* ptr)
{
	struct Page *newpage;
	struct Page *currentpage = block->firstpage;
	short int *size;
	size = (short*)ptr - 2;
	newpage = createPage(*size);
	newpage->address = ptr - 2;
	if (currentpage->address > ptr + 2)
	{
		newpage->nextpage = currentpage;
		currentpage->prevpage = newpage;
		block->firstpage = newpage;
	}
	else
	{
		while (currentpage->nextpage != NULL)
		{
			if (currentpage->nextpage->address > ptr - 2)
				break;
			currentpage = currentpage->nextpage;
		}

		if (currentpage->nextpage == NULL)
		{
			currentpage->nextpage = newpage;
			newpage->prevpage = currentpage;
		}
		else
		{
			newpage->nextpage = currentpage->nextpage;
			currentpage->nextpage->prevpage = newpage;
			newpage->prevpage = currentpage;
			currentpage->nextpage = newpage;
		}
	}
	mergePages(block->firstpage);
}



void ms_free(void *ptr)
{
	struct Block *currentblock=firstblock;
	while (currentblock != NULL)
	{
		if (currentblock->address <= (char*)ptr && (currentblock->address + PAGE_SIZE - 1) > (char*)ptr)
			break;
		currentblock = currentblock->nextblock;
	}
	if (currentblock == NULL)
	{
		printf("Error");
		return;
	}
	insert(currentblock, (char*)ptr);

}

