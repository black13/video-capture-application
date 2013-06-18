
/*
* 
* Copyright (C) 2013 Ridgerun (http://www.ridgerun.com). 
* 
*/

#include <gst/gst.h>
#include <glib.h>
#include <stdlib.h>
#include <signal.h>

GstElement *pipeline;
GMainLoop *loop;
GstElement *source, *filter, *accel, *encoder, *queue, *muxer, *sink;
GstCaps *video_caps;
GstBus *bus;
guint bus_watch_id;
