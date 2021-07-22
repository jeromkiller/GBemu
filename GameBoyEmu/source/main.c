#include <gtk/gtk.h>
#include "sharedData.h"
#include "GameBoyEmu.h"

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
void setupSignals(GtkWidget* window, thread_data** sharedDataBlocks)
{
	player_input* input = NULL;
	framebuffer* fb = NULL;

	//itterate through the shared data blocks, to assign the pointers
	for (int i = 0; i < NUM_DATATYPES; i++)
	{
		switch(sharedDataBlocks[i]->type)
		{
			case SHARED_DATA_TYPE_PLAYER_INPUT:
				input = sharedDataBlocks[i];
				break;
			case SHARED_DATA_TYPE_FRAMEBUFFER:
				fb = sharedDataBlocks[i];
				break;
			case SHARED_DATA_TYPE_NONE:
				printf("ERROR: sharedDataBlocks[%d]->type is DATATYPE_NONE\n", i);
				exit(0);
				break;
		}
	}
	if(input == NULL || fb == NULL)
	{
		printf("Error: shared data block pointers are NULL\n");
		exit(0);
	}

	//todo: add a destroy signal for cleaning up the emulator

	//connect signal for player input
	g_signal_connect(G_OBJECT(window), "key-press-event", G_CALLBACK(handleKeyEvent), input);
	g_signal_connect(G_OBJECT(window), "key-release-event", G_CALLBACK(handleKeyEvent), input);
}

static void activate(GtkApplication* app, gpointer user_data)
{
	//build the gui
	GtkWidget *window = buildGUI(app);
	gtk_widget_show_all(window);

	//setup blocks of shared data
	thread_data* sharedDataBlocks[NUM_DATATYPES];
	sharedDataBlocks[0] = create_shared_input();
	sharedDataBlocks[1] = create_shared_framebuffer();

	setupSignals(window, sharedDataBlocks);

	//start the gameboy emulator thread
	/*GThread* emulatorThread =*/ g_thread_new("EmulatorCore", (GThreadFunc)startGameboy, sharedDataBlocks);
	/*	currently not using emulator thread
	*/
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