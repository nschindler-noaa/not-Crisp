#ifndef _qMapMedAnnouncements_h_
#define _qMapMedAnnouncements_h_

// Message indicates that left or right mouse button has been pressed but
// there was nothing under the cursor
// returns NULL
const int MM_LButtonPressed_NoOp = 100;
const int MM_MButtonPressed_NoOp = 101;
const int MM_RButtonPressed_NoOp = 102;

// Message indicates that a icon or geo feature has been pressed by the 
// left or right mouse button
// returns reference to a QmapObject
const int MM_LButtonPressed_Icon = 110;
const int MM_MButtonPressed_Icon = 111;
const int MM_RButtonPressed_Icon = 112;

const int MM_LButtonPressed_Geography = 120;
const int MM_MButtonPressed_Geography = 121;
const int MM_RButtonPressed_Geography = 122;

// Message indicates that a icon or geo feature has been moved over by the mouse
// returns reference to a QmapObject
const int MM_MouseMoved_Icon = 130;
const int MM_MouseMoved_Geography = 131;

// Message contains the current position of the mouse on the map
// returns reference to a QmapObject
const int MM_MouseMoved_GeoPosition = 132; 

const int MM_StatusBar_Message = 140;

#endif

