terminal text editor that uses 0 libraries.. everything is written by me <br />

CONTROLS <br />
type '/' to open the command line <br />
type 'i' to enter insert mode <br />
type ESC to exit insert mode <br>
asdasd
o = move cursor down and start newline <br />
O = move cursor up and start new line <br />
hjkl or arrow keys move cursor <br />
HJKL to jump 24 lines and center on screen <br />
w to jump foreward until whitespace b to jump backward until whitespace <br />
x deleted currently character <br />
a moves cursor right then enters insert mode <br />
s deletes character and enters insert mode <br />
f[letter] = find [letter] in line <br />
F[letter] = find [letter] in line backwards <br />
c = center current line on screen <br />
u to undo (goes until session) <br />
r to redo (goes to newest change) <br />

CTRL q to quick exit (without saving) <br />
CTRL s to quick save <br />

/theme to change theme <br />
--themes: light dark default1 default2 default3 default4 deafult5 gptcolors retro_orange retro_green <br />
/w to save /q to quit /e to edit a file <br />

no copy paste yet <br />

planned features: <br />

--add some vim <leader> imitation for faster commands (things like <leader>zz) <br />
--add the "anatomy of a motion" so i can do things like 10j 10k dw db ETC <br />
--this will likely mean a rework of the normal mode quick commands since i will need to add a proper buffer system to create a full command across inputs <br />
--add CTRL f and CTRL F <br />
--add a keystrokes tracker
--made statusbar better,, in theory it can spillover to next line with a really really long filename <br />


note for 30 <br />
start work on visual line mode to copy <br />
start work on copy paste to zep-only buffer (vim yank) <br />
undo/redo history <br />
need to add ability to do dw db and relative line jumps <br />


commit #30 <br />
started work on undo/redo tree <br />
changed statusbar write display <br />
mBuf now is * mbuf on heap <br />
no memory leaks according to valgrind <br />
more accurate write size notification <br />
fixed problem with hitting s in normal mode <br />
starting work on having multiple buffers (and windows) open at once <br />
undo and redo is fully working afaik and is completely memory safe !!!! <br />
also added a quick CTRL Q to exit fast <br />
re-disabling the CTRL C to kill a program <br />
hitting CTRL S in insert mode does weird stuff.. fixed it i think.. keep an eye out  <br />

commit #29 <br />
fixed w L scroll bugs <br />
fixed typing at end of line bug <br />
now tracked how many bytes were added in most recent write <br />
now tracks time of most recent write <br />
can save with ctrl s  <br />
added a dark teme <br />
added default theme is light theme <br />
going to add a feature to auto toggle between light and dark theme at certain times of the day <br />


commit #28 <br />
fixed JK <br />
updated how statusbar is drawm <br />
added c to center screen <br />
JK both now center screen too <br />
fixed updateMode <br />
cleaned up normalmode checks,, can move entire portion to its own file soon to clean up textedit.c <br />

commit #27 <br />
fixed lots of memory leak issues, still has memory leaking from loadFile but idk how <br />
finally fixed yoffset bug, now there arent random -1's <br />
fixed oO putting you on wrong line <br />
fixed s crashed if done on empty line <br />
added H&L to jump to start/beginning of line <br />
added JK to jump up/down 20 lines -- WIP,, needs adjustment <br />

commit #26 <br />
fixed fF <br />

commit #25 <br />
added more default themes <br />
changed the command leader to '/' instead of ':' <br />
add more commands like "/b" to jump to bottom and "/t" to jump to top <br />
add ctrl f and CTRL F for find and find and replace <br />
add a /themes? command to list themes in a seperate window... <br />

commit #24 <br />
ffinally added /t support <br />
added /theme, themes are default1 default2 gptcolors <br />
reoranigzed themes for way better scalability <br />

commit #23 <br />
HEAVY rewrite of the drawbuffer function is maybe faster? now allows for the text to scroll horizontally with less memory <br />
still need to add tab ctrl u ctrl d <br />
need to add :theme <br />
highlight substr is off by one right now, i am adjusting by calling it with end-1,, investigate this <br />
add a :syntax command to quickly change what language i syntax highlight for <br />

commit #22 <br />
reduced tearing on scrolling <br />

commit #21 <br />
minor colorscheme changes <br />

commit #20 <br />
NOW HAS SYNTAX HIGHLIGHTING !!!! <br />
only support single line comments and C syntax <br />
maybe it dynamically set lang syntax by looking at file extension <br />
going to add :colorscheme command <br />
still doesnt support hitting tab
ADD HITTING TAB SUPPORT AND CTRL U CTRL D

commit #19 <br />
now has left padding on scroll <br />
much better colorscheme support <br />
less flickering on fast typing? <br />
fixed :w :wq :e :e! :saveas commands <br />
statusbar is now 2 high, will allow me to display more shit <br />
is more supportive for adding seperate windows in the future <br />
restructured some code <br />
weenor <br />

commit #18 <br />
big changes to code structuring to allow for better scaling <br />
slight changes to scrolling, now happens with a padding <br />
no more borders <br />
i forgot to add padding when scrolling in -x direction oops <br />
also it seems like there is more flickering that there used to be ? not sure why.. <br />
only seems to happen when typing too quickly <br />

commit #17 <br />
working on adding syntax highlighting <br />
working on cleaning up code... not going so well.. <br />
weird behavior when cursor is at ends of line <br />

commit #16 <br />
small bug fixes <br />
add \t CTRL U CTRL D and zz next commit <br />
going to do a big overhaul of the structuring to allow for easier maintenence and better runtime <br />

commit #15 <br />
fF now scrolls properly <br />
bug fixes <br />
added :e <br />
reformatted some things <br />
ADD \t CTRL U CTRL D and zz <br />

commit #14 <br />
added fF oO <br />
reformatted normal mode controls for readability <br />
ADD :e next commit <br />
ADD \t next commit <br />
ADD CTRL+U and CTRL+D and zz next commit <br />
start work on a proper system for doing things like dw 2j 5k dfn etc etc <br />

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

commit #12 <br />
added w q q! wq commands <br />
added better status bar support <br />
can finally create a new file by typing ./editor [new filename] and it will create a blank file <br />

commit #11 <br />
added baby statusbar and better color support <br />
added saveas command <br />

commit #10 <br />
i think all of the general cusor movement is basically completed <br />
added some basic modal stuff for Normal and Insert <br />
added hjkl q d w b in normal mode <br />
hit <ESC> to switch from Insert to Normal mode <br />
typing ./editor without a filename will now open a default blank buffer with a greeting message <br />
moving cursor now wraps lines <br />

commit #9 <br />
fixed a bug with scrolling <br />
now shows line numbers and ~ for empty lines <br />
going on plane so in case i fuck anything up i have a good backup <br />
need to fix delete \n cursor movement <br />
RIGHTSIDE SCROLLING OCCURS TOO EARLY <br />

commit #8 <br />
12/27/2024 <br />
bug fix creating newline if cursor at rend_HEIGHT wouldnt work <br />

commit #7 <br />
added deleting and creating new lines, still need to implement the cursor moving to the correct place though <br />
scrolling works perfectly <br />
more color support on the window being draw <br />
need to add command line and make it modal now <br />

commit #6 <br />
horizontal scrolling is 100% working now <br />
afaik the line snapping is working perfectly too <br />
added easy logging functionality too <br />

commit #5 <br />
NOW IS PROPERLY SPAM PROOF <br />
there is still some weirdness in the get.curs.pos function where spamming can cause the cursor to fall behind <br />
color support <br />
there is also weirdness with rightside horizontal scrolling <br />--mimic common vim motions (ex hjkl wW bB hjkl dd i oO <ESC> f) <br />

commit #4 <br />   
now can scroll horizontally <br />

commit #3 <br />
improved line snapping feature <br />
lines are dynamically allocated instead of bein set length <br />
still need to fix typing too fast <br />
why are these lines getting synced with each other <br />
oh okay its because now i am basically writing into unallocated memory so undefined behavior <br />
this is a side effect of dynamically set lines <br />
fixed flickering issue on scroll <br />
         
commit #2 <br />
added future support for horizontal text scrolling for long ass lines <br />
added vertical scroll support <br />
going to add proper implementation for \n \t text insert and backspace <br />

commit #1 <br />
FIX THE ERROR OF TWO KEY PRESSES AT ONCE, look up "cannonical terminal input mode" <br />
first commit is with 1 buffer for entire file with in-out range <br />
cant add newlines to the text file <br />

