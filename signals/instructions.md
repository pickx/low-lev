system programing for self-pace learners - משכורות בהייטקס's profile photo
system programing for self-pace learners - משכורות בהייטקס‎
unread,
Aug 4, 2026, 6:59:05 PM (7 days ago) 
to system programing for self-pace learners - משכורות בהייטקס‎
exercise 2: time-limited questions (this is the reminder from last week):

    the program has a list of questions that it asks the user, with
    yes/no answers.
    terminal input, with a timer to limit the amount of time the user has for
    answering.
    the program also emits a beep every second, to make the user aware of the
    time passing, by printing the bell character ('\a').

exercise 3: using real-time signals for a poor-man's IPC: (this is the new exercise):

    write two programs - a sender and a receiver.
    1. the sender - gets one parameter - the PID of the reciever (which should
       be launched first). runs a loop of:
         - waits for user input - which should be a count, and a real-time
           signal number between SIGRTMIN and SIGRTMIN+2.
         - sends the matching signal to the receiver 'count' number of times
           in quick succession.
         - if the input is the letter 'e' -  the sender process terminates.
    2. the receiver - initializes a global int counter, and waits for any signal.
         - if the signal is SIGRTMIN - increases the counter by 1.
         - if the signal is SIGRTMIN+1 - decreases the counter by 1.
         - if the signal is SIGRTMIN+2 - emits the value of the counter to the
           screen.

    testing should include:
    1. sending with a very large count
       1.a. what is the maximum count that can be sent without signal loss?
            find an official document (not just an 'AI answer') describing
            this - and attempt to prove this via testing.
    2. running multiple 'sender' processes, all of which send signals to
       the same receiver. check scenarios where they all use a positive count,
       or when some user a positive count and some use a negative count (i.e.
       SIGRTMIN+1), simultaneously.

once you have one of them working, please send me an e-mail with the code, so i'll be able to review and provide feedback.

thanks,
--guy
