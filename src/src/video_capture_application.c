
/*
* 
* Copyright (C) 2013 Ridgerun (http://www.ridgerun.com). 
* 
* Simple gstreamer C plugin creating pipeline:
* 
* gst-launch -e videotestsrc num-buffers=300 ! 'video/x-raw-yuv,format=(fourcc)NV12,width=640,
* height=480' ! dmaiaccel ! dmaienc_h264 ! queue ! qtmux ! filesink location=<Specified as argv[1]>
* 
* Example: To record a file named: test_video.h264
* 
*  video-capture-application test_video.h264
* 
*/

#include "video_capture.h"

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

static gboolean bus_call	(GstBus     *bus,
							 GstMessage *msg,
							 gpointer    data)
{
	GMainLoop *loop = (GMainLoop *) data;

	switch (GST_MESSAGE_TYPE (msg)) {

		case GST_MESSAGE_EOS:
			g_print ("End of stream\n");
			g_main_loop_quit (loop);
		break;

		case GST_MESSAGE_ERROR: {
			gchar  *debug;
			GError *error;

			gst_message_parse_error (msg, &error, &debug);
			g_free (debug);

			g_printerr ("Error: %s\n", error->message);
			g_error_free (error);

			g_main_loop_quit (loop);
		break;
		}
		
		default:
		break;
	}

	return TRUE;
}

int factory_make(char* location)
{
	/* Create gstreamer elements */
	pipeline = gst_pipeline_new ("pipeline");
	source   = gst_element_factory_make ("videotestsrc",	"videotestsrc");
	filter   = gst_element_factory_make ("capsfilter",	"filter");
	accel    = gst_element_factory_make ("dmaiaccel",		"dmaiaccel");
	encoder  = gst_element_factory_make ("dmaienc_h264",	"dmaienc_h264");
	queue    = gst_element_factory_make ("queue",			"queue");
	muxer    = gst_element_factory_make ("qtmux",			"mux");
	sink     = gst_element_factory_make ("filesink",		"filesink");

	/* Video caps */
	video_caps = gst_caps_new_simple ("video/x-raw",
	"format", G_TYPE_STRING, "NV12",
	"width", G_TYPE_INT, 640,
	"height", G_TYPE_INT, 480,
	NULL);

	if (!pipeline || !source || !filter || !accel || !encoder || !queue || !muxer || !sink || !video_caps) {
		g_printerr ("One element could not be created. Exiting.\n");
		return -1;
	}

	/* Set up elements */
	g_object_set (G_OBJECT (filter), "caps", video_caps, NULL);
	gst_caps_unref (video_caps);
	g_object_set (G_OBJECT (source), "num-buffers", 300, NULL);

	/* we set the input filename to the source element */
	g_object_set (G_OBJECT (sink), "location", location, NULL);

	return 0;

}

int pipeline_make()
{
	/* we add all elements into the pipeline */
	gst_bin_add_many (GST_BIN (pipeline),
		source, accel, encoder, queue, muxer, sink, NULL);
	/* we link the elements together */
	gst_element_link_many (source, accel, encoder, queue, muxer, sink, NULL);

	return 0;
}

int state_handler()
{
	gst_element_set_state (pipeline, GST_STATE_PLAYING);
	return 0;
}

int watcher_make()
{
	/* we add a message handler */
	bus = gst_pipeline_get_bus (GST_PIPELINE (pipeline));
	bus_watch_id = gst_bus_add_watch (bus, bus_call, loop);
	gst_object_unref (bus);	
	return 0;
}

int main (int   argc, char *argv[])
{
	/* Initialisation */
	gst_init (&argc, &argv);
	signal(SIGINT, intHandler);

	loop = g_main_loop_new (NULL, FALSE);
	
	/* Check input arguments */
	if (argc != 2) {
		g_printerr ("Usage: %s <Recorded file name eg: test.h264>\n", argv[0]);
		return -1;
	}

	/* Initialize elements */
	if(factory_make(argv[1]) != 0)
		return -1;

	/* Add function to watch bus */
	if(watcher_make() != 0)
		return -1;

	/* Add elements to pipeline, and link them */
	if(pipeline_make() != 0)
		return -1;

	/* Handle pipeline state */
	/* Set the pipeline to "playing" state*/
	if(state_handler() !=0)
		return -1;

	/* Iterate */
	g_print ("Running...\n");
	g_main_loop_run (loop);  

	return 0;
}
