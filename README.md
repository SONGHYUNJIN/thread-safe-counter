# thread-safe-counter
It confirms that mutex is faster than semaphore by using a time command.
It's because mutex is that only one thread is in its critical section at any given time.
Mutex is capable of counting to 1, while semaphore able to count from 0 to N.
