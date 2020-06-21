#include "paging.h"
#include "pagesim.h"
#include "swapops.h"
#include "stats.h"

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"

/*  --------------------------------- PROBLEM 6 --------------------------------------
    Checkout PDF section 7 for this problem

    Page fault handler.

    When the CPU encounters an invalid address mapping in a page table,
    it invokes the OS via this handler.

    Your job is to put a mapping in place so that the translation can
    succeed. You can use free_frame() to make an available frame.
    Update the page table with the new frame, and don't forget
    to fill in the frame table.

    When you access a frame, you should update its referenced bit.

    Lastly, you must fill your newly-mapped page with data. If the page
    has never mapped before, just zero the memory out. Otherwise, the
    data will have been swapped to the disk when the page was
    evicted. Call swap_read() to pull the data back in.

    HINTS:
         - You will need to use the global variable current_process when
           setting the frame table entry.

    ----------------------------------------------------------------------------------
 */
void page_fault(vaddr_t address) {
    /* First, split the faulting address and locate the page table entry.
       Remember to keep a pointer to the entry so you can modify it later. */
   vpn_t vpn = vaddr_vpn(address);
   //fte_t *pt_fte = frame_table + PTBR; // address for start of page table
   pte_t *vpn_pte = (pte_t*)(mem + PTBR * PAGE_SIZE) + vpn; // address for specific entry
    
    /* It's a page fault, so the entry obviously won't be valid. Grab
       a frame to use by calling free_frame(). */
   pfn_t new_frame = free_frame();

    /* Update the page table entry. Make sure you set any relevant values. */
   vpn_pte->pfn = new_frame;
   vpn_pte->valid = 1;

    /* Update the frame table. Make sure you set any relevant values. */
   fte_t *pfn_fte = frame_table + new_frame;
   pfn_fte->process = current_process;
   pfn_fte->vpn = vpn;
   pfn_fte->mapped = 1;

    /* Initialize the page's memory. On a page fault, it is not enough
     * just to allocate a new frame. We must load in the old data from
     * disk into the frame. If there was no old data on disk, then
     * we need to clear out the memory (why?).
     *
     * 1) Get a pointer to the new frame in memory.
     * 2) If the page has swap set, then we need to load in data from memory
     *    using swap_read().
     * 3) Else, just zero the page's memory. If the page is later written
     *    back, swap_write() will automatically allocate a swap entry.
     */
   if(swap_exists(vpn_pte)) {
     swap_read(vpn_pte, mem + (new_frame * PAGE_SIZE));
   } else {
     // zero out the memory
     memset(mem + (new_frame * PAGE_SIZE), 0, PAGE_SIZE);
     vpn_pte->dirty = 0;
   }
}

#pragma GCC diagnostic pop
