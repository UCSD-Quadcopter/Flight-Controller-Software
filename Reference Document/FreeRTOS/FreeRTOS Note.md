Note Author: Hongtao Zhang

Reference: 161204_Mastering_the_FreeRTOS_Real_Time_Kernel-A_Hands-On_Tutorial_Guide.pdf



## Heap (Chapter 2 of Mastering the FreeRTOS Real Time Kernel A Hands-On Tutorial Guide)

* Different **implementations** of FreeRTOS's malloc & free are located in heap_1.c, heap_2.c, ...., heap_5.c

  * *"FreeRTOS now treats memory allocation as part of the portable layer (as opposed to part of the core code base).  This is in recognition of the fact that different embedded systems have varying dynamic memory allocation and timing requirements, so a single dynamic memory allocation algorithm will only ever be appropriate for a subset of applications.  
  Also, removing dynamic memory allocation from the core code base enables application writer’s to provide their own specific implementations, when appropriate"* 
    - chapter 2 page 27
  
* **heap_4**.c or **heap_5**.c are highly recommended for newer FreeRTOS versions

  * Heap is essential to run the FreeRTOS kernel: runtime stack/heap, TCP control block
  * heap_4 allocates virtual heap memory from a **single** static array (Let's name it the **base array**). 
    * the array is statically declared & dimensioned by the micro "configTOTAL_HEAP_SIZE"
  * heap_5 can allocate memory from **multiple** separated memory space, e.g. external RAM, SD card, Flash drive
  * Both works by subdividing the **base array** into smaller blocks using the first fit algorithm
    * A detailed example of this algorithm is given in Chapeter 2 page 33
  * Both not instruction time deterministic, meaning the time to allocate can vary, but still much faster than malloc()/free() 

* Usage:

  * APIs:

    - ```c
    void *pvPortMalloc(size_t xWantedSize); // similar to malloc()
      ```

    - ```c
      void vPortFree(void *pv); // similar to free()
      ```
  
    - An example
  
    ```c
      //An example, usage nearly identical to malloc & free
    int *p = (int*) pvPortMalloc((int)num_elem * sizeof(int));
      /* ... */
      vPortFree(p);
      ```

    - Some utility functions:

      - ```c
      size_t xPortGetFreeHeapSize( void );
        ```
      
      - ```c
        size_t xPortGetMinimumEverFreeHeapSize( void );
        ```
      
      - ```c
      void vApplicationMallocFailedHook( void );
        ```

  * Memory details (heap_1, heap2, heap_3 are outdated, thus omitted):  

    * **heap_4**:

      * Default: The **Base** **Array**'s start address is set automatically by the linker.
      * constant "**configAPPLICATION_ALLOCATED_HEAP**" set to 0
      * Customize the starting address for the **Base Array**:
        * Set micro **configAPPLICATION_ALLOCATED_HEAP** to 1
        * Declare the **Base** **Array** by the following declaration: (compiler dependent syntax)
          * uint8_t **ucHeap**[configTOTAL_HEAP_SIZE] \_attribute\_ ( (section(".my_heap")) );    // [GCC Syntax (-supported on both Atollic Truestudio & System Workbench-), placing the array in the memory section named .my_heap]
          * uint8_t **ucHeap**[configTOTAL_HEAP_SIZE] @ 0x2000000  // IAR syntax, placing the array at the absolute address 0x2000,0000 (just an example)
  
    * **heap_5**:

      * Unlike heap_4, heap_5 must be explicitly initialized before calling **pvPortMalloc()** by calling:

        * **vPortDefineHeapRegions**(const **HeapRegion_t** *const pxHeapRegions);  // must be called before any kernel object being created if using heap_5
  
        * **HeapRegion_t** is defined by [staring address + length]:
  
          ```c
        typedef struct HeapRegion {     
              /* The start address of a block of memory that will be part of the heap.*/     
            uint8_t *pucStartAddress; 
              /* The size of the block of memory in bytes. */     	size_t xSizeInBytes; 
        }HeapRegion_t; 
          ```
  
        * Parameter "pxHeapRegions"
  
          * A pointer to the start address of **an array of** HeapRegion_t objects(struct), it is aimed to initialize the used regions all at once.
          * This **array** of type "HeapRegion_t" must be ordered by the starting address (ascending order), i.e. 
            * MemRegion with the Lowest start address must be the first obj in the array
            * MemRegion with the Highest start address must be the last in the array
          * The end of the array is marked by a HeapRegion_t structure that has its pucStartAddress member set to NULL. 
  
        * An example:
  
        <img src="C:\Users\zhang\Documents\GitHub\Flight-Controller-Software\Reference Document\FreeRTOS\FreeRTOS Note Markdown Pics\Capture1.PNG" style="zoom:50%" />
  
        ```c
        /* Define the start address and size of the three RAM regions. */ 
        #define RAM1_START_ADDRESS    ( ( uint8_t * ) 0x00010000 ) #define RAM1_SIZE             ( 65 * 1024 ) 
         
        #define RAM2_START_ADDRESS    ( ( uint8_t * ) 0x00020000 ) #define RAM2_SIZE             ( 32 * 1024 ) 
         
        #define RAM3_START_ADDRESS    ( ( uint8_t * ) 0x00030000 ) #define RAM3_SIZE             ( 32 * 1024 ) 
         
        /* Create an array of HeapRegion_t definitions, with an index for each of the three RAM regions, and terminating the array with a NULL address.  The HeapRegion_t structures must appear in start address order, with the structure that contains the lowest start address appearing first. */ 
        const HeapRegion_t xHeapRegions[] = 
        {     
            { RAM1_START_ADDRESS, RAM1_SIZE },     
          { RAM2_START_ADDRESS, RAM2_SIZE },     
            { RAM3_START_ADDRESS, RAM3_SIZE },     
          { NULL,               0         }  /* Marks the end of the array. */ 
        }; 
       
        int main( void ) {  
          /* Initialize heap_5. */     
            vPortDefineHeapRegions( xHeapRegions ); 
       
            /* Add application code here. */ 
      } 
        ```
        
        **But** this example is not realistic because it allocates all RAM for the Heap required by RTOS kernel, leaving no memory for regular variables. 
  
        <img src="C:\Users\zhang\Documents\GitHub\Flight-Controller-Software\Reference Document\FreeRTOS\FreeRTOS Note Markdown Pics\Capture3.PNG" style="zoom:50%" />
  
        ![](C:\Users\zhang\Documents\GitHub\Flight-Controller-Software\Reference Document\FreeRTOS\FreeRTOS Note Markdown Pics\Capture2.PNG)
  
        * As an alternative, a more recommended approach is given as another example below:
  
          <img src="C:\Users\zhang\Documents\GitHub\Flight-Controller-Software\Reference Document\FreeRTOS\FreeRTOS Note Markdown Pics\Capture4.PNG" style="zoom:50%" />
  
          ```c
    /* Define the start address and size of the two RAM regions not used by the  linker. */ 
          #define RAM2_START_ADDRESS    ( ( uint8_t * ) 0x00020000 ) #define RAM2_SIZE             ( 32 * 1024 ) 
           
          #define RAM3_START_ADDRESS    ( ( uint8_t * ) 0x00030000 ) #define RAM3_SIZE             ( 32 * 1024 ) 
           
        /* Declare an array that will be part of the heap used by heap_5.  The array will be placed in RAM1 by the linker. */ 
          #define RAM1_HEAP_SIZE ( 30 * 1024 ) 
        static uint8_t ucHeap[ RAM1_HEAP_SIZE ]; 
           
        /* Create an array of HeapRegion_t definitions.  Whereas in Listing 6 the first entry described all of RAM1, so heap_5 will have used all of RAM1, this time the first entry only describes the ucHeap array, so heap_5 will only use the part of RAM1 that contains the ucHeap array.  The HeapRegion_t structures must still appear in start address order, with the structure that contains the lowest start address appearing first. */ 
          const HeapRegion_t xHeapRegions[] = {     
            { ucHeap,             RAM1_HEAP_SIZE },     
              { RAM2_START_ADDRESS, RAM2_SIZE },     
            { RAM3_START_ADDRESS, RAM3_SIZE },     
              { NULL,               0         }  /* Marks the end of the array. */ 
          }; 
          ```
        
          ![](C:\Users\zhang\Documents\GitHub\Flight-Controller-Software\Reference Document\FreeRTOS\FreeRTOS Note Markdown Pics\Capture5.PNG)
  
          ![](C:\Users\zhang\Documents\GitHub\Flight-Controller-Software\Reference Document\FreeRTOS\FreeRTOS Note Markdown Pics\Capture6.PNG)
  
  
  
  # Task
  
  ### Top Level Task States
  
  * Single core processor runs only one task one at a time. A task has two states: *Running* and *Not Running*: Running => Not Runing [switched/swapped **in**], Running <= Not Runing [switched/swapped **out**] 
  
  * API:
  
    ```c
    BaseType_t xTaskCreate( 
        TaskFunction_t pvTaskCode, 
        
        const char * const pcName,                       
        
        uint16_t usStackDepth,   
        
        void *pvParameters,
        
        UBaseType_t uxPriority,
        
        TaskHandle_t *pxCreatedTask ); 
    ```
  
    * const char* **pcName**;  
      * Name for the task for debugging purpose, it is **not** used by FreeRTOS kernel in anyway.
      * Maximum char length is determined by configMAX_TASK_NAME_LEN including NULL terminator.
    * uint16_t usStackDepth; 
      * Telling the kernel how large the stack should be.
      * Each task has its own unique stack.
      * *usStack**Depth*** specifies the number of **words** for the stack size. 
        * For 32-bit system: passing [*usStackDepth* = 100] would result in (100 * 4 bytes = 400 bytes), here 4 is the stack **width**  
        * **Width** * **Depth** must **<= ** **$2^{16}-1$**. [Max # for uint16_t]
        * configMINIMAL_STACK_SIZE determines the size of the stack used by the **idle task**. The value for this constant in the demo application is a recommended value for any task in general.
        * Section 12.3 of "Mastering the FreeRTOS kernel" describes the situation of stack overflow.
    * void *pvParameters;
      * **Formal parameter** to be passed to the task function.
      * Normally the writer would just **cast** it into the desired type within the task function. And it would be a pointer pointing to either an array or struct in case the application writer wants to pass in multiple params.
    * UBaseType_t uxPriority;
      * Defines the priority for this task.
      * Priority: **0 (Lowest) ~ ** ConfigMAX_PRIORITIES **- 1 (highest)**
      * Passing priority param with a value greater than ConfigMAX_PRIORITY would result in being capped silently to the value of ConfigMAX_PRIORITY 
    * TaskHandle_t ***** pxCreatedTask;
      * This is a "handle"
      * pxCreatedTask is assigned with a special pointer value internally in xTaskCreate for the purpose of keeping track *THIS* task and being passed as a parameter to another API function(external to xTaskCreate) for telling that API function which task it needs to handle.
      * e.g. This handle can be used to reference this task in other API calls, such as changing the priority or delete the task.
      * If not used, just pass a NULL pointer.
    * Return value:
      * pdPass: Task successfully created
      * pdFAIL: Task has not been created due to insufficient heap memory.
  
  *  
  
   