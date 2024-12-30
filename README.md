shitty terminal based text editor that uses basically only ANSI escape codes to move the cursor and do that stuff  <br />

planned features: <br />

--add modal mode <br />
--add status bar displaying some info like current file and line count <br />
--add command bar for VERY basic vim commands (ex w wq q e) <br />
--add some vim <leader> imitation for faster commands (things like <leader>zz) <br />
--add the "anatomy of a motion" so i can do things like 10j 10k dw db ETC <br />
--this will likely mean a rework of the normal mode quick commands since i will need to add a proper buffer system to create a full command across inputs <br />

commit #1 <br />
FIX THE ERROR OF TWO KEY PRESSES AT ONCE, look up "cannonical terminal input mode"  <br />
first commit is with 1 buffer for entire file with in-out range <br />
cant add newlines to the text file  <br />

commit #2 <br />
added future support for horizontal text scrolling for long ass lines  <br />
added vertical scroll support <br />
going to add proper implementation for \n \t text insert and backspace <br />

commit #3 <br />
improved line snapping feature <br />
lines are dynamically allocated instead of bein set length <br />
still need to fix typing too fast <br />
why are these lines getting synced with each other <br />
oh okay its because now i am basically writing into unallocated memory so undefined behavior <br />
this is a side effect of dynamically set lines <br />
fixed flickering issue on scroll <br />     
         
commit #4 <br />   
now can scroll horizontally <br />

commit #5 <br />
NOW IS PROPERLY SPAM PROOF <br />
there is still some weirdness in the get_curs_pos function where spamming can cause the cursor to fall behind <br />
color support <br />
there is also weirdness with rightside horizontal scrolling <br />--mimic common vim motions (ex hjkl wW bB hjkl dd i oO <ESC> f) <br />

commit #6 <br />
horizontal scrolling is 100% working now <br />
afaik the line snapping is working perfectly too <br />
added easy logging functionality too <br />

commit #7 <br />
added deleting and creating new lines, still need to implement the cursor moving to the correct place though <br />
scrolling works perfectly <br />
more color support on the window being draw <br />
need to add command line and make it modal now <br />

commit #8 <br />
12/27/2024 <br />
bug fix creating newline if cursor at rend_HEIGHT wouldnt work <br />

commit #9 <br />
fixed a bug with scrolling <br />
now shows line numbers and ~ for empty lines <br />
going on plane so in case i fuck anything up i have a good backup <br />
need to fix delete \n cursor movement <br />
RIGHTSIDE SCROLLING OCCURS TOO EARLY <br />

commit #10 <br />
i think all of the general cusor movement is basically completed <br />
added some basic modal stuff for Normal and Insert <br />
added hjkl q d w b in normal mode <br />
hit <ESC> to switch from Insert to Normal mode <br />
typing ./editor without a filename will now open a default blank buffer with a greeting message <br />
moving cursor now wraps lines <br />


.

