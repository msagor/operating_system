/*
 File: cont_frame_pool.H
 
 Author: R. Bettati
 Department of Computer Science
 Texas A&M University
 Date  : 17/02/04 
 
 Description: Management of the CONTIGUOUS Free-Frame Pool.
 
 As opposed to a non-contiguous free-frame pool, here we can allocate
 a sequence of CONTIGUOUS frames.
 
 */

#ifndef _CONT_FRAME_POOL_H_                   // include file only once
#define _CONT_FRAME_POOL_H_

/*--------------------------------------------------------------------------*/
/* DEFINES */
/*--------------------------------------------------------------------------*/

/* -- (none) -- */

/*--------------------------------------------------------------------------*/
/* INCLUDES */
/*--------------------------------------------------------------------------*/

#include "machine.H"

/*--------------------------------------------------------------------------*/
/* DATA STRUCTURES */

	
/*--------------------------------------------------------------------------*/

/* -- (none) -- */

/*--------------------------------------------------------------------------*/
/* C o n t F r a m e   P o o l  */
/*--------------------------------------------------------------------------*/

class ContFramePool {
    
private:
    /* -- DEFINE YOUR CONT FRAME POOL DATA STRUCTURE(s) HERE. */
    unsigned char * bitmap;       
    unsigned int    n_info_frames;  	
    unsigned long   base_frame_no; 
    unsigned long   nframes;       
    unsigned long   info_frame_no; 
    
    void mark_inaccessible(unsigned long _frame_no);
	
    
public:
	
	
	
    // The frame size is the same as the page size,   
    static const unsigned int FRAME_SIZE = Machine::PAGE_SIZE; 

    ContFramePool(unsigned long _base_frame_no,
		  unsigned long _nframes,
		  unsigned long _info_frame_no,
                  unsigned long _n_info_frames);
    /*
     Initializes the data structures needed for the management of this
     frame pool.
     _base_frame_no: Number of first frame managed by this frame pool.
     _n_frames: Size, in frames, of this frame pool.
     EXAMPLE: If _base_frame_no is 16 and _n_frames is 4, this frame pool manages
     physical frames numbered 16, 17, 18 and 19.
     _info_frame_no: Number of the first frame that should be used to store the
     management information for the frame pool.
     NOTE: If _info_frame_no is 0, the frame pool is free to
     choose any frames from the pool to store management information.
     _n_info_frames: If _info_frame_no is 0, this argument specifies the
     number of consecutive frames needed to store the management information
     for the frame pool.
     EXAMPLE: If _info_frame_no is 699 and _n_info_frames is 3,
     then Frames 699, 700, and 701 are used to store the management information
     for the frame pool.
     NOTE: This function must be called before the paging system
     is initialized.
     */
    
    unsigned long get_frame(unsigned long _n_info_frame);
   /* Allocates a frame from the frame pool. If successful, returns the frame
    * number of the frame. If fails, returns 0. */

   void mark_inaccessible(unsigned long _base_frame_no,
                          unsigned long _nframes);
   /* Mark the area of physical memory as inaccessible. The arguments have the
    * same semanticas as in the constructor.
    */

   static void release_frame(unsigned long _frame_no);
   /* Releases frame back to the given frame pool.
      The frame is identified by the frame number. 
      NOTE: This function is static because there may be more than one frame pool
      defined in the system, and it is unclear which one this frame belongs to.
      This function must first identify the correct frame pool and then call the frame
      pool's release_frame function. */
    
    //static unsigned long needed_info_frames(unsigned long _n_frames);
    /*	
     Returns the number of frames needed to manage a frame pool of size _n_frames.
     The number returned here depends on the implementation of the frame pool and
     on the frame size.
     EXAMPLE: For FRAME_SIZE = 4096 and a bitmap with a single bit per frame
     (not appropriate for contiguous allocation) one would need one frame to manage a
     frame pool with up to 8 * 4096 = 32k frames = 128MB of memory!
     This function would therefore return the following value:
     _n_frames / 32k + (_n_frames % 32k > 0 ? 1 : 0) (always round up!)
     Other implementations need a different number of info frames.
     The exact number is computed in this function.
     */

};
#endif
