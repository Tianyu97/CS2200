#include "types.h"
#include "pagesim.h"
#include "paging.h"
#include "swapops.h"
#include "stats.h"
#include "util.h"

pfn_t select_victim_frame(void);


/*  --------------------------------- PROBLEM 7 --------------------------------------
    Checkout PDF section 7 for this problem
    
    Make a free frame for the system to use.

    You will first call the page replacement algorithm to identify an
    "available" frame in the system.

    In some cases, the replacement algorithm will return a frame that
    is in use by another page mapping. In these cases, you must "evict"
    the frame by using the frame table to find the original mapping and
    setting it to invalid. If the frame is dirty, write its data to swap!
 * ----------------------------------------------------------------------------------
 */
pfn_t free_frame(void) {
    pfn_t victim_pfn;

    /* Call your function to find a frame to use, either one that is
       unused or has been selected as a "victim" to take from another
       mapping. */
    victim_pfn = select_victim_frame();

    /*
     * If victim frame is currently mapped, we must evict it:
     *
     * 1) Look up the corresponding page table entry
     * 2) If the entry is dirty, write it to disk with swap_write()
     * 3) Mark the original page table entry as invalid
     * 4) Unmap the corresponding frame table entry
     *
     */
    if (frame_table[victim_pfn].mapped == 1) {
        vpn_t victim_vpn = frame_table[victim_pfn].vpn;
        pcb_t *victim_pcb = frame_table[victim_pfn].process;
        //fte_t *victim_pt = frame_table + victim_pcb->saved_ptbr;
        pte_t *victim_pte = (pte_t*)(mem + victim_pcb->saved_ptbr * PAGE_SIZE) + victim_vpn;

        if (victim_pte->dirty == 1) {
            swap_write(victim_pte, mem + (victim_pfn * PAGE_SIZE));
            stats.writebacks = stats.writebacks + 1;
        }

        victim_pte->valid = 0;
        victim_pte->dirty = 0;
        frame_table[victim_pfn].mapped = 0;
    }


    /* Return the pfn */
    return victim_pfn;
}



/*  --------------------------------- PROBLEM 9 --------------------------------------
    Checkout PDF section 7, 9, and 11 for this problem

    Finds a free physical frame. If none are available, uses either a
    randomized or clocksweep algorithm to find a used frame for
    eviction.

    Return:
        The physical frame number of a free (or evictable) frame.

    HINTS: Use the global variables MEM_SIZE and PAGE_SIZE to calculate
    the number of entries in the frame table.
    ----------------------------------------------------------------------------------
*/
pfn_t select_victim_frame() {
    /* Pointer to the frame chosen on the last iteration of clocksweep */
    static pfn_t clocksweep_pointer = 0;

    /* See if there are any free frames first */
    size_t num_entries = MEM_SIZE / PAGE_SIZE;
    for (size_t i = 0; i < num_entries; i++) {
        if (!frame_table[i].protected && !frame_table[i].mapped) {
            return i;
        }
    }

    if (replacement == RANDOM) {
        /* Play Russian Roulette to decide which frame to evict */
        pfn_t last_unprotected = NUM_FRAMES;
        for (pfn_t i = 0; i < num_entries; i++) {
            if (!frame_table[i].protected) {
                last_unprotected = i;
                if (prng_rand() % 2) {
                    return i;
                }
            }
        }
        /* If no victim found yet take the last unprotected frame
           seen */
        if (last_unprotected < NUM_FRAMES) {
            return last_unprotected;
        }
    } else if (replacement == CLOCKSWEEP) {
        /* Implement a clocksweep page replacement algorithm here */
        for (pfn_t i = clocksweep_pointer; i < NUM_FRAMES; i++) {
            if (!frame_table[i].protected) {
                // don't select referenced frames
                if (frame_table[i].referenced) {
                    // if referenced bit is set, clear it, but don't choose as victim
                    frame_table[i].referenced = 0;
                } else {
                    // update the clocksweep pointer to point to next frame after victim
                    if (i == NUM_FRAMES - 1) {
                        // if the victim frame is the last available frame, need to start the pointer at the beginning
                        clocksweep_pointer = 0;
                    } else {
                        clocksweep_pointer = i + 1;
                    }
                    return i;
                }
            } 
            if (i == NUM_FRAMES - 1) {
                // if we are currently indexing the last frame, loop back around
                i = 0;
            }
        }
    }

    /* If every frame is protected, give up. This should never happen
       on the traces we provide you. */
    panic("System ran out of memory\n");
    exit(1);
}
