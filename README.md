shitty terminal based text editor that uses basically only ANSI escape codes to move the cursor and do that stuff  <br />

planned features: <br />

FIX THE FILENAME NOT WORKING CORRECTLY, MALLOC dest->filename INSTEAD OF STRCPY IT
--made status bar display current line # and character # <br />
--ADD CTRL+D CTRL+U CTRL+V <br />
--add proper \t support <br />
--**reoraganize the buffer rendering into a struct **<br />
  -works sort of... can do further restructuring but this is a pretty good start <br />
  -struct Buffer with char **, ypos, xpos, etc <br />
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

commit #11 <br />
added baby statusbar and better color support <br />
added saveas command <br />

commit #12 <br />
added w q q! wq commands <br />
added better status bar support <br />
can finally create a new file by typing ./editor [new filename] and it will create a blank file <br />


commit #13 <br />
changed colorscheme <br />
added a x s in normal mode <br />
changed project name and reorganized files <br />
removed cursor wrapping lines for now <br />
changed some error messages <br />
fixed status bar \t and \033 errors <br />
going to finally add \t support and maybe a smart indent system <br />
cleaned up some redundant code and removed some functions <br /> 
can now delete line 0 and can hit 'q' more smoothly <br />
next commit, add oO and fF <br />
.

commit #14 <br />
added fF oO <br />
reformatted normal mode controls for readability <br />
ADD :e next commit <br />
ADD \t next commit <br />
ADD CTRL+U and CTRL+D and zz next commit <br />
start work on a proper system for doing things like dw 2j 5k dfn etc etc <br />


commit #15 <br />
fF now scrolls properly <br />
bug fixes <br />
added :e <br>
reformatted some things <br />
ADD \t CTRL U CTRL D and zz <br />

commit #16 <br />
small bug fixes <br />
add \t CTRL U CTRL D and zz next commit <br />
going to do a big overhaul of the structuring to allow for easier maintenence and better runtime <br />


change #17 <br />
working on adding syntax highlighting <br />
working on cleaning up code... not going so well.. <br />
weird behavior when cursor is at ends of line <br />

commit #18 <br />
big changes to code structuring to allow for better scaling <br />
slight changes to scrolling, now happens with a padding <br />
no more borders <br />
i forgot to add padding when scrolling in -x direction oops <br />
also it seems like there is more flickering that there used to be ? not sure why.. <br />
only seems to happen when typing too quickly <br />
