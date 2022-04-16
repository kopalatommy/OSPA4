/*
 * File: pager-lru.c
 * Author:       Andy Sayler
 *               http://www.andysayler.com
 * Adopted From: Dr. Alva Couch
 *               http://www.cs.tufts.edu/~couch/
 *
 * Project: CSCI 3753 Programming Assignment 4
 * Create Date: Unknown
 * Modify Date: 2012/04/03
 * Description:
 * 	This file contains an lru pageit
 *      implmentation.
 */

#include <stdio.h> 
#include <stdlib.h>

#include "simulator.h"

void pageit_basic(Pentry q[MAXPROCESSES])
{
	/* Local vars */
    int proc;
    int pc; // program counter
    int page; 
    int oldpage; 
	// pagenumber = pc / page size
	static int calls = 0;
	static int pageFaults = 0;

    /* Trivial paging strategy */
    /* Select first active process */ 
    for(proc=0; proc<MAXPROCESSES; proc++)
	{ 
		/* Is process active? */
		if(q[proc].active)
		{
			/* Dedicate all work to first active process*/ 
			pc = q[proc].pc; 		        // program counter for process
			page = pc/PAGESIZE; 		// page the program counter needs
			/* Is page swaped-out? */
			if(!q[proc].pages[page])
			{
				pageFaults++;
				/* Try to swap in */
				if(!pagein(proc,page))
				{
					/* If swapping fails, swap out another page */
					for(oldpage=0; oldpage < q[proc].npages; oldpage++)
					{
						/* Make sure page isn't one I want */
						if(oldpage != page)
						{
							/* Try to swap-out */
							if(pageout(proc,oldpage))
							{
								/* Break loop once swap-out starts*/
								break;
							} 
						}
					}
				}
			}
			/* Break loop after finding first active process */
			break;
		}
    }
	calls++;
	printf("Page fault rate: %i / %i\n", pageFaults, calls);
}

void pageit(Pentry q[MAXPROCESSES])
{
	static char isInitialized = 0;
	static int time = 1;
	static int timestamps[MAXPROCESSES][MAXPROCPAGES];
	static uint pageFaults = 0;
	static uint calls = 0;
	
	int i, j;
	int curPage;
	int oldest;

	if(!isInitialized)
	{
		isInitialized = 1;
		for(i = 0; i < MAXPROCESSES; i++)
		{
			for(j = 0; j < MAXPROCPAGES; j++)
			{
				timestamps[i][j] = 0;
			}
		}
	}

	calls++;

	for(i = 0; i < MAXPROCESSES; i++)
	{
		if(q[i].active)
		{
			curPage = q[i].pc/PAGESIZE;
			printf("Proc %i requsting %i\n", i, curPage);

			// The current page is not active, need to swap in
			if(!q[i].pages[curPage])
			{
				pageFaults++;
				printf("Page fault rate: %u / %u\n", pageFaults, calls);

				oldest = 0;
				//for(j = 0; j < q[i].npages; j++)
				for(j = 0; j < MAXPROCESSES; j++)
				{
					// Cannot swap out the page trying to be put in
					if(j == curPage)
						continue;
					if(timestamps[i][j] == 0)
					{
						oldest = j;
						break;
					}
					else if(timestamps[i][j] < timestamps[i][oldest])
						oldest = j;
				}

				if(timestamps[i][oldest] == 0)
				{
					if(!pagein(i, curPage))
					{
						printf("Failed to page in when unallocated\n");
						exit(EXIT_FAILURE);
					}
				}
				else
				{
					if(!pageout(i, oldest))
					{
						printf("Failed to page out when unallocated\n");
						exit(EXIT_FAILURE);
					}
				}
			}
			timestamps[i][curPage] = time++;
			break;
		}
	}
}


