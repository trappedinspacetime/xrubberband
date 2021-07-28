# xrubberband
Rubberbanding is a screen selection rectangle drawn on the screen while dragging the mouse on the screen. When the mouse button releases, it prints geometry of the screen. X Window system lacks transparency in desktop, so it uses pseudo transparency by merging layers.  The screen which includes desktop wallpaper is called "root window". To draw on the root window is a matter, because the other windows , for example a clock, need to repaint itself, in this casee your rubberbanding rectangle flickers. To avoid this issue you need to use a translucent or transparent which covers the whole screen.

https://stackoverflow.com/questions/25191715/unwanted-flickering-rubberband-while-dragging-mouse-on-screen-with-xdrawrectangl
