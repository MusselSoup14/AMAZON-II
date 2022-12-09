#pragma once
/**< Buffer Size */
#define KEY_BUFFER_SIZE  64

/*
	if buffered mode, retrieve last input data and clear buffered data.
	if not buffered mode, return current data input
*/
bool get_touchxy(int* x, int* y);

/*
	if buffered mode, retrieve first input data in queue
	if not buffered mode, return false
*/
bool get_touchxy_buffered(int* x, int* y);

void touchinit();

/*
	return old-mode
*/
bool enable_touch_bufferd_mode(bool b);

