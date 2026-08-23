the multi_consumer has a bug (and this is partially my fault, due to how i phrased the exercise). the producer might disable the queue before all the entries were consumed, and then the consumers would terminate prematurely.

right now, the only way i can see to solve this (without pushing '0' entries with the number of consumers) is to add yet-another condition variable, to make the producer wait until the queue is empty, before disabling it.

on Tuesday we'll discuss another solution to this problem.

one more important note: in your code, you perform a signal after every push, and a signal after every pop. this is not efficient. for the 'pop' part you could have a signal only if the queue moved from 'max capacity' to 'one less' (please justify why this cannot introduce a deadlock bug).

for the 'push' this is more problematic (because trying to optimize this in a similar fashion might cause all but one consumer to remain sleeping in certain scenarios - e.g. if several consumers are in the 'wait on cond variable' state). please try to think of a way to overcome this...
