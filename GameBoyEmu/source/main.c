#include <gtk/gtk.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include "sharedData.h"
#include "GameBoyEmu.h"

#include <errno.h>

//global variables
shared_Thread_Blocks* sharedData = NULL;
GtkWidget *window = NULL;
GThread* emulatorThread = NULL;

//---------------------
//-- signal handlers --
//---------------------
//handle player keypresses
void handleKeyEvent(GtkWidget *widget, GdkEventKey *event, player_input* sharedInput)
{
	//lock the mutex
	player_input_data* keydata = get_player_input_data(sharedInput);

	//pressed or released
	unsigned char pressed = event->type == GDK_KEY_PRESS ? 1 : 0;

	//filter the keypresses, and set the data
	switch(event->keyval)
	{
		case GDK_KEY_Up:
			keydata->input_up = pressed;
			break;
		case GDK_KEY_Down:
			keydata->input_down = pressed;
			break;
		case GDK_KEY_Left:
			keydata->input_left = pressed;
			break;
		case GDK_KEY_Right:
			keydata->input_right = pressed;
			break;
		case GDK_KEY_z:
			keydata->input_A = pressed;
			break;
		case GDK_KEY_x:
			keydata->input_B = pressed;
			break;
		case GDK_KEY_a:
			keydata->input_start = pressed;
			break;
		case GDK_KEY_s:
			keydata->input_select = pressed;
			break;
	}

	//release the data
	unlock_shared_data(sharedInput);
}

//handle exit signal
void handleExit(GtkWidget *widget, GdkEvent *event, emu_status_flags* sharedFlags)
{
	//for some reason sharedFlags is always 0x01 here

	//let the emulator know that we're exiting
	//lock the mutex and get the data
	emu_status_flags_data* status_flags = get_status_flags_data(get_shared_status_flags(sharedData));
	status_flags->flags |= STATUS_FLAG_EMULATOR_STOPING;
	unlock_shared_data(get_shared_status_flags(sharedData));

	//wait for the emulator to finish
	g_thread_join(emulatorThread);
	g_thread_unref(emulatorThread);
	emulatorThread = NULL;

	//clean up
	destroy_shared_Thread_Blocks(sharedData);
	sharedData = NULL;
}

//build the GUI
GtkWidget* buildGUI(GtkApplication* app)
{
	GtkWidget* window;
	GtkWidget* vbox;
	GtkWidget* screen;

	//create the window
	window = gtk_application_window_new(app);
	gtk_window_set_title(GTK_WINDOW(window), "GameBoy Emulator");
	gtk_window_set_default_size(GTK_WINDOW(window), 160, 144);

	//create the vbox
	vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 1);
	gtk_container_add(GTK_CONTAINER(window), vbox);

	//create the screen
	screen = gtk_image_new();	//the pixbuff will be added later
	gtk_box_pack_start(GTK_BOX(vbox), screen, TRUE, TRUE, 0);

	return window;
}

//setup signals
void setupSignals(GtkWidget* window, shared_Thread_Blocks* sharedDataBlocks)
{
	//get the shared data headers
	emu_status_flags* emu_status = get_shared_status_flags(sharedDataBlocks);
	player_input* input = get_shared_player_input(sharedDataBlocks);
	framebuffer* fb = get_shared_framebuffer(sharedDataBlocks);

	if(emu_status == NULL || input == NULL || fb == NULL)
	{
		printf("Error: shared data block pointers are NULL\n");
		exit(0);
	}

	//connect the signals, and assign the shared data headers to the signal callbacks
	g_signal_connect(G_OBJECT(window), "destroy", G_CALLBACK(handleExit), emu_status);

	//connect signal for player input
	g_signal_connect(G_OBJECT(window), "key-press-event", G_CALLBACK(handleKeyEvent), input);
	g_signal_connect(G_OBJECT(window), "key-release-event", G_CALLBACK(handleKeyEvent), input);

}

static void activate(GtkApplication* app, gpointer user_data)
{
	//build the gui
	window = buildGUI(app);
	gtk_widget_show_all(window);

	//setup blocks of shared data
	shared_Thread_Blocks* sharedDataBlocks = create_shared_Thread_Blocks();

	//keep a global copy so we can use it in signal handlers
	sharedData = sharedDataBlocks;

	setupSignals(window, sharedDataBlocks);

	//start the gameboy emulator thread
	emulatorThread = g_thread_new("EmulatorCore", (GThreadFunc)startGameboy, sharedDataBlocks);

}

int main(int argc, char *argv[])
{
	GtkApplication *app;
	int emu_status;

	app = gtk_application_new("com.github.jeromkiller.gbEmu", G_APPLICATION_FLAGS_NONE);	
	g_signal_connect(app, "activate", G_CALLBACK(activate), NULL);
	emu_status = g_application_run (G_APPLICATION (app), argc, argv);

	g_object_unref(app);

	return emu_status;
}