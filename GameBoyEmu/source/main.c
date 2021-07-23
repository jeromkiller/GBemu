#include <gtk/gtk.h>
#include <stdio.h>
#include <fcntl.h>
#include "sharedData.h"
#include "GameBoyEmu.h"
#include <unistd.h>
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
	release_data(sharedInput);
}

//handle exit signal
void handleExit(GtkWidget *widget, GdkEvent *event, gpointer data)
{
	//let the emulator know that we're exiting
	write(sharedData->gui_pipe[PIPE_WRITE], "X", 1);

	//wait for the emulator to finish
	g_thread_join(emulatorThread);
	g_thread_unref(emulatorThread);
	emulatorThread = NULL;

	//clean up
	free_shared_data(sharedData->input);
	free_shared_data(sharedData->fb);
	free(sharedData);
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
	//get the 
	player_input* input = get_specified_header(sharedDataBlocks, SHARED_DATA_TYPE_PLAYER_INPUT);
	framebuffer* fb = get_specified_header(sharedDataBlocks, SHARED_DATA_TYPE_FRAMEBUFFER);

	if(input == NULL || fb == NULL)
	{
		printf("Error: shared data block pointers are NULL\n");
		exit(0);
	}

	g_signal_connect(G_OBJECT(window), "destroy", G_CALLBACK(handleExit), NULL);

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
	shared_Thread_Blocks* sharedDataBlocks = (shared_Thread_Blocks*)malloc(sizeof(shared_Thread_Blocks));
	sharedDataBlocks->input = create_shared_input();
	sharedDataBlocks->fb = create_shared_framebuffer();
	//create two pipes for comunication between the emulator and the GUI
	int retval = pipe(sharedDataBlocks->gui_pipe);
	retval = pipe(sharedDataBlocks->emu_pipe);

	//keep a global copy so we can use it in signal handlers
	sharedData = sharedDataBlocks;

	setupSignals(window, sharedDataBlocks);

	//start the gameboy emulator thread
	emulatorThread = g_thread_new("EmulatorCore", (GThreadFunc)startGameboy, sharedDataBlocks);

}

int main(int argc, char *argv[])
{
	GtkApplication *app;
	int status;

	app = gtk_application_new("com.github.jeromkiller.gbEmu", G_APPLICATION_FLAGS_NONE);	
	g_signal_connect(app, "activate", G_CALLBACK(activate), NULL);
	status = g_application_run (G_APPLICATION (app), argc, argv);

	g_object_unref(app);

	return status;
}