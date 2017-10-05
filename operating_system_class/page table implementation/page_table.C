#include "assert.H"
#include "exceptions.H"
#include "console.H"
#include "paging_low.H"
#include "page_table.H"
#include "cont_frame_pool.H"

PageTable * PageTable::current_page_table = NULL;
unsigned int PageTable::paging_enabled = 0;
ContFramePool * PageTable::kernel_mem_pool = NULL;
ContFramePool * PageTable::process_mem_pool = NULL;
unsigned long PageTable::shared_size = 0;



void PageTable::init_paging(ContFramePool * _kernel_mem_pool,
                            ContFramePool * _process_mem_pool,
                            const unsigned long _shared_size)
{
	
   kernel_mem_pool = _kernel_mem_pool;
   process_mem_pool = _process_mem_pool;
   shared_size = _shared_size;
   
}

PageTable::PageTable()
{
	//initialized the pointers to 0/null
	page_directory = NULL;
	page_table = NULL;

	//allocate one frame for the page directory.
	unsigned long frame_no_of_pag_dir = kernel_mem_pool->get_frame(1); 
	unsigned long frame_no_of_pag_tab = kernel_mem_pool->get_frame(1);  

	//store the page directory to the allocated physical frame;
	page_directory = (unsigned long *) (frame_no_of_pag_dir * 4096);  //stored the page directory to 			the "frame_no_of_pag_dir" frame address
	//store the page table right after page directory;
	page_table = (unsigned long *) (frame_no_of_pag_tab * 4096);     //stored the page table to the 		"frame_no_of_pag_tab" frame address
	
	
	///fill up the page table and flip the bits
	unsigned long every_4096 = 0;
	int j;for(j=0;j<1024;j++){
		page_table[j] = every_4096 | 3;
		every_4096= every_4096 + 4096; ;
	}  //initializes all page table entries to 0, 4096,...since its direct mapped
           //this page table entries are set as present since its our first page table.
	
	
	///fill up the page dir and flip the bits
	int i;for(i=0;i<1024;i++){
		if(i==0){
			page_directory[i] = ((unsigned long ) page_table) | 3;
			//page_directory[i] = page_directory[i] | 3;  //present
			 
			
		}else{
			
			page_directory[i] = 0 | 2;  //last 1023 entries in PD are set not present coz
						      //they dont have page tables
		}
	}
	
	
	/* Initializes a page table with a given location for the directory and the
     page table proper.
     NOTE: The PageTable object still needs to be stored somewhere! 
     Probably it is best to have it on the stack, as there is no 
     memory manager yet...
     NOTE2: It may also be simpler to create the first page table *before* 
     paging has been enabled.
	 
	 
  */

   
   Console::puts("Constructed Page Table object\n");


}


void PageTable::load()
{
	
	/* Makes the given page table the current table. This must be done once during
     system startup and whenever the address space is switched (e.g. during
     process switching). */
	 
   write_cr3((unsigned long) page_directory);
   current_page_table =  this;
   Console::puts("Loaded page table\n");
   
   
   
   }

void PageTable::enable_paging()
{  
   /* Enable paging on the CPU. Typically, a CPU start with paging disabled, and
     memory is accessed by addressing physical memory directly. After paging is
     enabled, memory is addressed logically. */
	 
   
   Console::puts("kernel is inside enable_paging\n");
   paging_enabled = 1;
   write_cr0(read_cr0() | 0x80000000); 
   Console::puts("Enabled paging!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n");
    
 

}


void PageTable::handle_fault(REGS * _r)
{
	
   Console::puts("cpu now inside handler function\n");
    //check the page directory
      //if the PDE is invalid aka 2,
        //get a frame from KMP and create a page table, or it all with 0 | 3, 
        //set the page table to the PDE and turn in 2
   //check the page table
     //if the PTE is invaild aka 2
       //get a frame from the PMP, set first byte to the PTE entry, turn it on by oring it with 3

    unsigned long address = read_cr2();
    unsigned long error_code = _r->err_code;
    unsigned long PDEN = (address>>22) ; //(unsigned long) (address>>22);
    unsigned long PTEN = (address <<10) >>22;//(unsigned long) (((unsigned long) (address <<10)) >>22);
    unsigned long * dir = (unsigned long*) read_cr3();
    unsigned long * tab = 0;
    
    if((error_code & 1) == 0){ //last bit of error code is 0 so its a page fault
	if((dir[PDEN] & 1)!=1){  //PDE fault coz last bit is 0 so result is not 1
             unsigned long new_frame_for_PT = PageTable::kernel_mem_pool->get_frame(1);
             tab = (unsigned long*) (4096*new_frame_for_PT); //new PT location
   	     dir[PDEN] = ((unsigned long) (tab)) | 3;
	     unsigned long new_page_for_new_PT = PageTable::process_mem_pool->get_frame(1);
	     int l;for(int l =0;l<1024;l++){
		tab[l] = ((unsigned long) (new_page_for_new_PT * 4096 +(4096*l)) ) | 3;
	     }

	}else{//PTE fault...PT exists, but one PTE is the culprit
	    unsigned long new_page_for_PTE = PageTable::process_mem_pool->get_frame(1);//get frame
	    //unsigned long * new_page_loc = (unsigned long*) (new_page_for_PTE * 4096);
            tab = (unsigned long*) current_page_table; //&(dir[1024]);
	    tab[PTEN] = ((unsigned long ) (4096 * new_page_for_PTE) ) | 3;
            //tab[PTEN] = tab[PTEN] | 3;
	}
    }
	
}














