
/*
 * 
 * Copyright (C) 2013 Ridgerun (http://www.ridgerun.com). 
 * 
 * Simple gstreamer C plugin creating pipeline:
 * 
 * gst-launch audiotestsrc ! alsasink
 * 
 */

#include <gst/gst.h>
#include <glib.h>
#include <stdlib.h>
#include <signal.h>

GstElement *pipeline;
GMainLoop *loop;

/* Signal handler for ctrl+c */
void intHandler(int dummy) {
	/* Out of the main loop, clean up nicely */
	g_print ("Returned, stopping playback\n");
	gst_element_set_state (pipeline, GST_STATE_NULL);
	g_print ("Deleting pipeline\n");
	gst_object_unref (GST_OBJECT (pipeline));
	g_main_loop_unref (loop);

	exit(0);
}

int
main (int   argc,
      char *argv[])
{

  GstElement *source, *encoder, *sink;
  GstBus *bus;
  guint bus_watch_id;

  /* Initialisation */
  gst_init (&argc, &argv);
  signal(SIGINT, intHandler);

  loop = g_main_loop_new (NULL, FALSE);

  /* Create gstreamer elements */
  pipeline = gst_pipeline_new ("pipeline");
  source   = gst_element_factory_make ("audiotestsrc",	"audiotestsrc");
  sink     = gst_element_factory_make ("alsasink",		"alsasink");

  if (!pipeline || !source || !sink) {
    g_printerr ("One element could not be created. Exiting.\n");
    return -1;
  }

  /* we add all elements into the pipeline */
  gst_bin_add_many (GST_BIN (pipeline),
                    source, sink, NULL);
  /* we link the elements together */
  gst_element_link_many (source, sink, NULL);

  /* Set the pipeline to "playing" state*/
  gst_element_set_state (pipeline, GST_STATE_PLAYING);

  /* Iterate */
  g_print ("Running...\n");
  g_main_loop_run (loop);  

  return 0;
}
