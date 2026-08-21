the functions (to point you to the man pages):

pthread_create (with null attrs) to create thread
pthread_join to wait for a thread to terminate

pthread_mutex_init to initialize a mutex
pthread_mutex_destroy to destroy the mutex

pthread_mutex_lock to lock the mutex (may block if the mutext is currently locked)
pthread_mutex_unlock - to unlock the mutex

to end the current thread - just return from the thread's start function.

exercise:
1. producer-consumer with busy-wait.
   create a 'queue' struct, that has an init/destroy method, a push and a pop.
   - the push receives a pointer to a queue entry, and adds it to the tail
     of the queue.
   - the pop removes the first entry from the queue and returns it to the
     caller, or returns NULL if there's no queued entry.

   each entry in the queue will have a pointer to 'next', and an int value:
   struct queue_entry {
       struct queue_entry *next;
       int value;
   };

   the queue protects itself using a mutex.

   the main program will create a queue (a local variable) and launch two
   threads - producer and consumer - passing the queue to each thread function
   as the sole argument. the main program then will wait for both
   threads to finish execution, and destroy the queue and exit.

   the producer thread chooses a random count (using the 'random' function),
   generates positive random numbers in this count, allocates queue entries
   with these numbers, and pushes them into the queue.

   the consumer thread reads entries from the queue, emits the
   random numbers into a file, and frees those entries. if it got a NULL
   from the pop (i.e. the queue is empty), it sleeps for a fraction of
   a second and tries again, until there is something to read from the queue.

   when the producer finished producing all its random numbers, it'll emit
   a queue entry with the value of '0', and exit. when the consumer receives
   an entry with a value of '0', it'll free it and exit.

   when debugging, make the producer emit the count, and the random numbers,
   to stdout, and after the program terminates, verify the contents of the file
   contains all the generated numbers. during initial debugging, make the count
   be no more then 10, in order to ease debugging.
2. producer-consumer with blocking wait and limited memory consumption:

   modify the queue from the previous exercise to use condition variables
   in the 'pop' method, to make it block in case the queue is empty.

   modify the 'push' method to block in case the queue has more then X entries
   (X should be a parameter provided during queue initialization).

   everything else should remain the same - and should still work.

3. producer-consumer with multiple consumers. all consumers try to read
   from the same queue, so each entry will go to one of the consumers.

   in order to terminate all the consumers, the queue should have an
   'is_active' member, and queue_pop should return an indication that
   the queue is no longer active, to allow all consumers to terminate.

   add api to the queue called queue_finish that sets is_active to 'false'.

   note: all consumers should emit the output to the same file, using the
         'append' mode. for this to work correctly, you should use
         the 'open' system call API with O_APPEND mode (+ 'write' with short
         buffers) to make it work correctly.

         this also implies it will be easier to open the file
         from 'main', and pass the resulting file descriptor, together with
         the queue, to all consumers.

         bonus points: keep the open inside each consumer, but make
                       sure they do not over-write each others file.

   note 2: when checking the correctness and completeness of the data in the
           output file, since the order of the numbers stored inside the file is
           not guaranteed any longer, you should match the list of numbers
           produced, to the list of numbers in the file, and make sure each
           produced number also appears in the file, and each number in
           the file was also produced. one way to do this is to sort
           both sets of data and compare them. use 'sort -n' to sort
           the file by numeric order.

note about the memory consumption measurement for exercise 2 (vs last week's exercise 1):

in order to check the maximum VM memory consumer, in the 'main' program, right before it exits,
you should open the file /proc/self/status, and look for the line starting with 'VmPeak'
