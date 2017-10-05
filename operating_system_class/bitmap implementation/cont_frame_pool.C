/*
 File: ContFramePool.C
 
 Author:
 Date  : 
 
 */

/*--------------------------------------------------------------------------*/
/* 
 POSSIBLE IMPLEMENTATION
 -----------------------

 The class SimpleFramePool in file "simple_frame_pool.H/C" describes an
 incomplete vanilla implementation of a frame pool that allocates 
 *single* frames at a time. Because it does allocate one frame at a time, 
 it does not guarantee that a sequence of frames is allocated contiguously.
 This can cause problems.
 
 The class ContFramePool has the ability to allocate either single frames,
 or sequences of contiguous frames. This affects how we manage the
 free frames. In SimpleFramePool it is sufficient to maintain the free 
 frames.
 In ContFramePool we need to maintain free *sequences* of frames.
 
 This can be done in many ways, ranging from extensions to bitmaps to 
 free-lists of frames etc.
 
 IMPLEMENTATION:
 
 One simple way to manage sequences of free frames is to add a minor
 extension to the bitmap idea of SimpleFramePool: Instead of maintaining
 whether a frame is FREE or ALLOCATED, which requires one bit per frame, 
 we maintain whether the frame is FREE, or ALLOCATED, or HEAD-OF-SEQUENCE.
 The meaning of FREE is the same as in SimpleFramePool. 
 If a frame is marked as HEAD-OF-SEQUENCE, this means that it is allocated
 and that it is the first such frame in a sequence of frames. Allocated
 frames that are not first in a sequence are marked as ALLOCATED.
 
 NOTE: If we use this scheme to allocate only single frames, then all 
 frames are marked as either FREE or HEAD-OF-SEQUENCE.
 
 NOTE: In SimpleFramePool we needed only one bit to store the state of 
 each frame. Now we need two bits. In a first implementation you can choose
 to use one char per frame. This will allow you to check for a given status
 without having to do bit manipulations. Once you get this to work, 
 revisit the implementation and change it to using two bits. You will get 
 an efficiency penalty if you use one char (i.e., 8 bits) per frame when
 two bits do the trick.
 
 DETAILED IMPLEMENTATION:
 
 How can we use the HEAD-OF-SEQUENCE state to implement a contiguous
 allocator? Let's look a the individual functions:
 
 
 
 Constructor: Initialize all frames to FREE, except for any frames that you 
 need for the management of the frame pool, if any.
 
 get_frames(_n_frames): Traverse the "bitmap" of states and look for a 
 sequence of at least _n_frames entries that are FREE. If you find one, 
 mark the first one as HEAD-OF-SEQUENCE and the remaining _n_frames-1 as
 ALLOCATED.

 release_frames(_first_frame_no): Check whether the first frame is marked as
 HEAD-OF-SEQUENCE. If not, something went wrong. If it is, mark it as FREE.
 Traverse the subsequent frames until you reach one that is FREE or 
 HEAD-OF-SEQUENCE. Until then, mark the frames that you traverse as FREE.
 
 mark_inaccessible(_base_frame_no, _n_frames): This is no different than
 get_frames, without having to search for the free sequence. You tell the
 allocator exactly which frame to mark as HEAD-OF-SEQUENCE and how many
 frames after that to mark as ALLOCATED.
 
 needed_info_frames(_n_frames): This depends on how many bits you need 
 to store the state of each frame. If you use a char to represent the state
 of a frame, then you need one info frame for each FRAME_SIZE frames.
 
 
 
 
 
 
 A WORD ABOUT RELEASE_FRAMES():
 
 When we releae a frame, we only know its frame number. At the time
 of a frame's release, we don't know necessarily which pool it came
 from. Therefore, the function "release_frame" is static, i.e., 
 not associated with a particular frame pool.
 
 This problem is related to the lack of a so-called "placement delete" in
 C++. For a discussion of this see Stroustrup's FAQ:
 http://www.stroustrup.com/bs_faq2.html#placement-delete
 
 */
/*--------------------------------------------------------------------------*/


/*--------------------------------------------------------------------------*/
/* DEFINES */
/*--------------------------------------------------------------------------*/

/* -- (none) -- */

/*--------------------------------------------------------------------------*/
/* INCLUDES */
/*--------------------------------------------------------------------------*/

#include "cont_frame_pool.H"
#include "console.H"
#include "utils.H"
#include "assert.H"

/*--------------------------------------------------------------------------*/
/* DATA STRUCTURES */



/*--------------------------------------------------------------------------*/



/* -- (none) -- */

/*--------------------------------------------------------------------------*/
/* CONSTANTS */
/*--------------------------------------------------------------------------*/

/* -- (none) -- */

/*--------------------------------------------------------------------------*/
/* FORWARDS */
/*--------------------------------------------------------------------------*/

/* -- (none) -- */


struct BitMap * ContFramePool::bit = 0;   //pointer declaration initialized to 0 aka null
/*--------------------------------------------------------------------------*/
/* METHODS FOR CLASS   C o n t F r a m e P o o l */
/*--------------------------------------------------------------------------*/

ContFramePool::ContFramePool(unsigned long _base_frame_no,
                             unsigned long _n_frames,
                             unsigned long _info_frame_no,
                             unsigned long _n_info_frames)
{
    //MY IMPLEMENTATION
	base_frame_no = _base_frame_no;
        n_frames       = _n_frames;
        info_frame_no = _info_frame_no;
	n_info_frames = _n_info_frames;  //variables
	
       

	//check if the frame is stored internally or externally
	if(info_frame_no == 0) {   //internally
        ContFramePool::bit= (struct BitMap *) (base_frame_no * FRAME_SIZE);  //kernel/ bitmap stored at that byte
    } else {                   //externally
        ContFramePool::bit = (struct BitMap *) (info_frame_no * FRAME_SIZE);
    }
	
	//mark all bits as initially 'F' as free
	for(int i =0; i< 8000; i++){
        	ContFramePool::bit->xyz[i] = 'F';
    }
	
	
	
	//mark inaccessible to the following mb
	//1 - 4 mb = kernel 
	int i;for(i=0;i<1000;i++){
	   ContFramePool::bit->xyz[i] = 'A';
	}
	//15-16 mb allocated   15-16 mb is around 3700 to 4300 frame no
	//int j;for(j=3700; j<4300; j++ ){
	//   ContFramePool::bit->xyz[j] = 'A';
	//}
	//info_frame_no if used 2 frames are reserved
	if(info_frame_no != 0){
		ContFramePool::bit->xyz[info_frame_no -1 ] = 'A';
		ContFramePool::bit->xyz[info_frame_no]     = 'A';     
		//info_frame_no being allocated
	}
	
	Console::puts("\n");
	Console::puts("Frame Pool initialized.\n");
	Console::puts("\n");
	

}



unsigned long ContFramePool::get_frames(unsigned int _n_frames)
{
    //MY IMPLEMENTATION
	Console::puts("\n");
	Console::puts("get_frame has been called");
	Console::puts("\n");
	unsigned int needed_frames   = _n_frames; //this amount of frames has been requested
	unsigned long return_frame_no = 10000;  //this value will be updated with the frame_no we are looking for. 10000 is a magic number to indicate dummy state.
	int counter = 0;                       //a counter for computational purpose. when counter equals needed_frame_no, we have found the frames we are looking for
	
	int i;for(i=0; i<8000	-needed_frames; i++){     //find if an entry in the array is F(free)
		if(ContFramePool::bit->xyz[i] == 'F'){
			int j; for(j =i; j<i+needed_frames; j++){    // if an entry found as F, look for next needed_frames number of entries to be free
				if(ContFramePool::bit->xyz[j] == 'F'){
					counter++;							  //counter count how many free netries have been found
				}else{
					counter=0;
				}
			}
			if(counter == needed_frames){
				return_frame_no = i;
				break;
			}else{
				counter = 0;
			}
		}
	}
	
	if(return_frame_no == 10000){						//border cases to make sure iteration doesnt go out of bound
		counter =0;
		int i;for(i=8000-needed_frames; i<8000; i++){
			if(ContFramePool::bit->xyz[i] == 'F'){
				counter++;
			}
		}
		if(counter == needed_frames){
			return_frame_no = 8000 - needed_frames;  //got the desired frame no in byte
			
		}else{
			return_frame_no = 0;  //no frame empty 
			Console::puts("\n");
			Console::puts("NO free frame! returning 0!");
			Console::puts("\n");
		}
	}
	
	
	mark_used_after_get_frames(return_frame_no + 1 , needed_frames); //1 added due to 0 to 4095 indices
			//mark there frames as used(first one = HEAD, the rest = Allocated)
	Console::puts("\n");
	Console::puts(" return frame no is: ");
	Console::puti(return_frame_no + 1);
	Console::puts(" \n");
	Console::puts("\n");
	return (return_frame_no + 1);     
			//1 added to represent the frame no. because only return_frame_no tells the byte no in the bitmap coz bitmap index are 0 to 4095
	
		
}

void ContFramePool::mark_used_after_get_frames(unsigned long _base_frame_no, unsigned long _n_frames)
{
	//MY IMPLEMENTATION
	unsigned int base_frame_no = _base_frame_no - 1;  //1 less in bitmap
	unsigned int n_frames 	   = _n_frames;
	
	int i;for(i= base_frame_no; i<=(base_frame_no+n_frames-1); i++){
		if(i == base_frame_no){
			ContFramePool::bit->xyz[i] = 'H';  //the head is marked as H
		}else{
			ContFramePool::bit->xyz[i] = 'A';  //the rests are marked as A
		}
	}	
}						   

void ContFramePool::mark_inaccessible(unsigned long _base_frame_no,
                                      unsigned long _n_frames) 
									  
{
	Console::puts("\n");
	Console::puts("mark_inaccessibel has been called");
	Console::puts("\n");
	//MY IMPLEMENTATION
	unsigned int base_frame_no = _base_frame_no - 1;  //1 less in bitmap coz indices from 0 to 4095
	unsigned int n_frames 	   = _n_frames;
	int counter;
	
	//check if the asked frames are being used or not..must be all free
	int i;for(int i=base_frame_no;i< (base_frame_no+n_frames-1); i++){
		if(ContFramePool::bit->xyz[i]=='F'){
			counter =0;
		}else{
			counter =1;
		}
	}
	
	//if counter =0, means frames are free to mark inaccessible
	if(counter == 0){
		int i;for(i= base_frame_no; i<=(base_frame_no+n_frames-1); i++){
		     if(i == base_frame_no){
				ContFramePool::bit->xyz[i] = 'H';  //the head is marked as H
		     }else{
				ContFramePool::bit->xyz[i] = 'A';  //the rests are marked as A
		     }
		
	    }
		Console::puts("\n");
		Console::puts("frame no starting from ");
		Console::puti(base_frame_no + 1);
		Console::puts(" upto ");
		Console::puti(n_frames);
		Console::puts(" many frames has been marked inaccessible!.");
		Console::puts("\n");	
	}else{
		Console::puts("\n");
		Console::puts("Mark inaccessible not done. frames being used.");
		Console::puts("\n");
	}
}

void ContFramePool::release_frames(unsigned long _first_frame_no)
{
    //MY IMPLEMENTATION
	Console::puts("\n");
	Console::puts("release_frame has been called");
	Console::puts("\n");
	unsigned int first_frame_no = _first_frame_no - 1;//1 reduced for bitmap
	int cursor = 0;									//variable that keeps track
	if(first_frame_no < (8000 -1)){       //if the asked frame is before 4095 
		if(ContFramePool::bit->xyz[first_frame_no] == 'H'){
			int temp_fffn = first_frame_no;
			while(cursor == 0){
				int temp_ffn = first_frame_no;
				ContFramePool::bit->xyz[first_frame_no] = 'F';
				cursor = 0;
				if(ContFramePool::bit->xyz[first_frame_no+1] == 'A'){
					first_frame_no++;
					cursor = 0;
				}else{   //reached H or A
					cursor = 100;
				}
				ContFramePool::bit->xyz[temp_ffn] = 'F';
			}
			Console::puts("\n");
			Console::puts("Frames released with head frame being: ");
			Console::puti(temp_fffn + 1);
			Console::puts(" until ");
			Console::puti(first_frame_no + 1);
			Console::puts("\n");
			
		}else{
			Console::puts("\n");
			Console::puts("Frame release failed. Not a head frame!");
			Console::puts("\n");
		}
	}else if(first_frame_no == (8000 - 1)){
		ContFramePool::bit->xyz[first_frame_no] ='F';
		Console::puts("\n");
		Console::puts("Frame released at head being : ");
		Console::puti(first_frame_no);
		Console::puts("\n");
	}
}

unsigned long ContFramePool::needed_info_frames(unsigned long _n_frames)
{
    //MY IMPLEMENTATION
	Console::puts("\n");
	Console::puts("needed_info_frame has been called");
	Console::puts("\n");
	unsigned long frame_size = 4000;
	unsigned long n_frames = _n_frames;
	unsigned long answer = 0;
	unsigned long total_no_of_frames = n_frames + 1000;   //7000 + 1000 = 8000
	
		answer = ( 8000/ 4096); // 8000 frames divided by 4096
		answer = answer +1; 
		return answer;
	
}


