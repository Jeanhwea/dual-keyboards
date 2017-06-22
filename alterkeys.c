// alterkeys.c
// http://osxbook.com
// modified by Chance Miller to support multikeyboard use
//
// Complile using the following command line:
//      gcc -Wall -o alterkeys alterkeys.c -framework ApplicationServices
//
// You need superuser privileges to create the event tap, unless accessibility
// is enabled. To do so, select the "Enable access for assistive devices"
// checkbox in the Universal Access system preference pane.

#include <ApplicationServices/ApplicationServices.h>

// Global Variables to keey track of modifier keys pressed
bool ctrl = false;
bool shift = false;
bool command = false;
bool option = false;

// This callback will be invoked every time there is a keystroke.
CGEventRef cbCGEvent(CGEventTapProxy proxy, CGEventType type, CGEventRef event, void *refcon)
{
    // Paranoid sanity check.
    if ((type != kCGEventKeyDown) && (type != kCGEventKeyUp) && (type != kCGEventFlagsChanged))
        return event;

    // The incoming keycode.
    CGKeyCode keycode = (CGKeyCode)CGEventGetIntegerValueField(event, kCGKeyboardEventKeycode);
    if (keycode == (CGKeyCode)59 || keycode == (CGKeyCode)62) {
        ctrl = !ctrl;
    }
    if (ctrl) CGEventSetFlags(event, NX_CONTROLMASK | CGEventGetFlags(event));
    if (keycode == (CGKeyCode)60 || keycode == (CGKeyCode)56) {
        shift = !shift;
    }
    if (shift) CGEventSetFlags(event, NX_SHIFTMASK | CGEventGetFlags(event));
    if (keycode == (CGKeyCode)55||keycode == (CGKeyCode)54) {
        command = !command;
    }
    if (command) CGEventSetFlags(event, NX_COMMANDMASK | CGEventGetFlags(event));
    if (keycode == (CGKeyCode)58||keycode == (CGKeyCode)61) {
        option = !option;
    }
    if (option) CGEventSetFlags(event, NX_ALTERNATEMASK | CGEventGetFlags(event));
    CGEventSetIntegerValueField(event, kCGKeyboardEventKeycode, (int64_t)keycode);

    // We must return the event for it to be useful.
    return event;
}

int main(int argc, char *argv[])
{
    CFMachPortRef eventTap;
    CGEventMask eventMask;
    CFRunLoopSourceRef runLoopSource;

    // Create an event tap. We are interested in key presses.
    eventMask = ((1 << kCGEventKeyDown) | (1 << kCGEventKeyUp) | (1 << kCGEventFlagsChanged));
    eventTap = CGEventTapCreate(kCGSessionEventTap, kCGHeadInsertEventTap, 0, eventMask, cbCGEvent, NULL);

    if (!eventTap) {
        fprintf(stderr, "failed to create event tap\n");
        exit(1);
    }

    // Create a run loop source.
    runLoopSource = CFMachPortCreateRunLoopSource(kCFAllocatorDefault, eventTap, 0);

    // Add to the current run loop.
    CFRunLoopAddSource(CFRunLoopGetCurrent(), runLoopSource, kCFRunLoopCommonModes);

    // Enable the event tap.
    CGEventTapEnable(eventTap, true);

    // Set it all running.
    CFRunLoopRun();

    // In a real program, one would have arranged for cleaning up.
    return 0;
}

