// gcc xrubberband.c -o xrubberband -lX11
// use case ; maim -w root -g $(./xrubberband)  `date +%H-%M-%S`.png

#include <stdio.h>
#include <X11/Xutil.h>
#include <X11/cursorfont.h>
#include <X11/extensions/XTest.h>
#include <X11/extensions/XInput.h>
#include <X11/Xos.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <stdlib.h>
#include <stdarg.h>
#include <X11/Xatom.h>

#define MAKE_RECT(etype)	\
    x = event.etype.x_root;	\
    y = event.etype.y_root;	\
    rw = x - rootx;		\
    if (rw < 0) rw = -rw;	\
    rh = y - rooty;		\
    if (rh < 0) rh = -rh;	\
    rx = x < rootx ? x : rootx;	\
    ry = y < rooty ? y : rooty

/* MAKE_CURSOR assigns a correct cursor */
#define MAKE_CURSOR(etype)						\
    pointer = (event.etype.x_root > rootx ?				\
	       (event.etype.y_root > rooty ? pointer2 : pointer4) :	\
	       (event.etype.y_root > rooty ? pointer3 : pointer1));
             
XRectangle Select_Rect(Display *dpy, int screen_num, Window root, Window parent, unsigned int display_width, unsigned int display_height)

{

    int status;
    XRectangle xrect;
    XEvent event;
    unsigned int x = 0, y = 0, rootx = 0, rooty = 0;
    Cursor pointer1, pointer2, pointer3, pointer4, pointer;
    int boxDrawn = False, selectionDone = False;
    int rx = 0, ry = 0, rw = 0, rh = 0;


    GC gc;

    /* get some cursors for rectangle formation */
    pointer1 = XCreateFontCursor(dpy, XC_ul_angle);
    pointer2 = XCreateFontCursor(dpy, XC_lr_angle);
    pointer3 = XCreateFontCursor(dpy, XC_ll_angle);
    pointer4 = XCreateFontCursor(dpy, XC_ur_angle);

    /* grab the pointer */
    status = XGrabPointer(dpy, root, False, ButtonPressMask,
			  GrabModeAsync, GrabModeAsync, root,
			  pointer1, CurrentTime);
 //    if (status != GrabSuccess) Fatal_Error("Can't grab the mouse.");
//if (status != GrabSuccess) fprintf( stderr, "Can't grab the mouse.\n");

    /* create a graphics context to draw with */
    gc = XCreateGC(dpy, parent, 0, NULL);
 //   if (!gc) Fatal_Error("Could not get drawing resources.");
if (!gc) fprintf( stderr, "Could not get drawing resources.\n");
    XSetSubwindowMode(dpy, gc, IncludeInferiors);
    XSetForeground(dpy, gc, 255);
    XSetFunction(dpy, gc, GXinvert);

    /* get a button-press and pull out the root location */
    XMaskEvent(dpy, ButtonPressMask, &event);
    rootx = rx = event.xbutton.x_root;
    rooty = ry = event.xbutton.y_root;

    /* get pointer motion events */
    XChangeActivePointerGrab(dpy, ButtonMotionMask | ButtonReleaseMask, pointer2, CurrentTime);

    /* loop to let the user drag a rectangle */
    while (!selectionDone) {
	XNextEvent(dpy, &event);
	switch(event.type) {
	
	case ButtonRelease:
	    if (boxDrawn) {
		XDrawRectangle(dpy, parent, gc, rx, ry, rw, rh);
		boxDrawn = False;
	    }
	    XFlush(dpy);
	    /* record the final location */
	    MAKE_RECT(xbutton);
	    selectionDone = True;
	    break;

	case MotionNotify:
	    if (boxDrawn) {
		XDrawRectangle(dpy, parent, gc, rx, ry, rw, rh);
		boxDrawn = False;
	    }
    
//	    while (XCheckTypedEvent(dpy, MotionNotify, &event)) { }
	    MAKE_RECT(xmotion);

	    XDrawRectangle(dpy, parent, gc, rx, ry, rw, rh);
	    boxDrawn = True;
	    MAKE_CURSOR(xmotion);
	    XChangeActivePointerGrab(dpy,
				     ButtonMotionMask | ButtonReleaseMask,
				     pointer, CurrentTime);
	    break;
	}
    }

    xrect.x      = rx;
    xrect.y      = ry;
    xrect.width  = rw;
    xrect.height = rh;

    /* release resources */
    XFreeGC(dpy, gc);
    XFreeCursor(dpy, pointer1);
    XFreeCursor(dpy, pointer2);
    XFreeCursor(dpy, pointer3);
    XFreeCursor(dpy, pointer4);

    XUngrabPointer(dpy, CurrentTime);
    int x1 = rx + rw, y1 = ry + rh;
    if ( display_width < x1 ) x1 = rx - rw;
    if ( display_height < y1 ) y1 = ry - rh;
printf("\n%dx%d+%d+%d", rw, rh, rx, ry);
    return xrect;
}             
int main(int argc, char* argv[])
{
  Display* dpy;		/* pointer to X Display structure.           */
  int screen_num;		/* number of screen to place the window on.  */
  Window win;			/* pointer to the newly created window.      */
  unsigned int display_width,
               display_height;	/* height and width of the X display.        */
  unsigned int width, height;	/* height and width for the new window.      */
  unsigned int win_x, win_y;	/* location of the window's top-left corner. */
  unsigned int win_border_width; /* width of window's border.                */
  char *display_name = getenv("DISPLAY");  /* address of the X display.      */
  Atom skip, state;
  dpy = XOpenDisplay(display_name);
  if (dpy == NULL) {
    fprintf(stderr, "%s: cannot connect to X server '%s'\n",
            argv[0], display_name);
    exit(1);
  }

  /* get the geometry of the default screen for our display. */
  screen_num = DefaultScreen(dpy);
  display_width = DisplayWidth(dpy, screen_num);
  display_height = DisplayHeight(dpy, screen_num);
// display_width =1280;
// display_height =800;
 char *window_name = (char*)"Drawing window";
 int whiteColor = WhitePixel(dpy, screen_num);

  Window root = RootWindow(dpy,screen_num);
    XVisualInfo vinfo;
    XMatchVisualInfo(dpy, DefaultScreen(dpy), 32, TrueColor, &vinfo);

    XSetWindowAttributes attr;
    attr.colormap = XCreateColormap(dpy, root, vinfo.visual, AllocNone);
    attr.border_pixel = 0;
    attr.background_pixel = 0;
    attr.override_redirect = 1;
    attr.border_pixel = 0;
    Window parent = XCreateWindow(dpy, root, 0, 0, display_width, display_height, 0, vinfo.depth, InputOutput, vinfo.visual, CWColormap | CWBorderPixel | CWBackPixel, &attr);

  state = XInternAtom(dpy, "_NET_WM_STATE", True);
  skip = XInternAtom(dpy, "_NET_WM_STATE_SKIP_TASKBAR", True);

    Atom wm_delete_window = XInternAtom(dpy, "WM_DELETE_WINDOW", 0);
    XSetWMProtocols(dpy, parent, &wm_delete_window, 1);
    XChangeProperty(dpy, parent, state, XA_ATOM, 32,
		  PropModeReplace, (unsigned char*)&skip, 1);

 XStoreName(dpy, parent, window_name);
 XMapWindow(dpy, parent);
 XSelectInput(dpy, parent, StructureNotifyMask| ButtonPressMask| PointerMotionMask| LeaveWindowMask| EnterWindowMask| ButtonReleaseMask );
 Drawable d = parent;
 XGCValues values;
 values.line_width = 4;
 values.line_style = LineSolid;
 GC gc = XCreateGC(dpy, d, GCLineWidth, &values);

 Select_Rect(dpy, screen_num, root, parent, display_width, display_height);
 

  return 0;
}
