shitty terminal based text editor using basically only ansi escape sequences to move the cursor  <br />
and do that stuff  <br />
FIX THE ERROR OF TWO KEY PRESSES AT ONCE, look up "cannonical terminal input mode"  <br />
first commit is with 1 buffer for entire file with in-out range <br />
cant add newlines to the text file  <br />




added future support for horizontal text scrolling for long ass lines  <br />
added vertical scroll support <br />
going to add proper implementation for \n \t text insert and backspace <br />


improved line snapping feature <br />
lines are dynamically allocated instead of bein set length <br />
still need to fix typing too fast <br />
why are these lines getting synced with each other <br />
oh okay its because now i am basically writing into unallocated memory so undefined behavior <br />
this is a side effect of dynamically set lines <br />
fixed flickering issue on scroll <br />     
            
now can scroll horizontally <br />


NOW IS PROPERLY SPAM PROOF <br />
there is still some weirdness in the get_curs_pos function where spamming can cause the cursor to fall behind <br /> 
color support
there is also weirdness with rightside horizontal scrolling 





.
