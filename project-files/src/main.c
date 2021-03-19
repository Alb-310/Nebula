#define _GNU_SOURCE 
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <gtk/gtk.h>
#include <gdk-pixbuf/gdk-pixbuf.h>

#include <gd.h>

#include ".h/filters.h"
#include ".h/image_settings.h"

#pragma region Main

typedef struct {
    GtkWidget *w_dlg_file_choose;       // Pointer to file chooser dialog box
    GtkWidget *w_img_main;              // Pointer to image widget
    GtkWidget *w_scale_brightness;      // Pointer to scale button
    GtkWidget *w_scale_contrast;        // Pointer to scale button
    GtkWidget *w_scale_temperature;     // Pointer to scale button
    GtkWidget *w_entry_brightness;      // Pointer to scale button
    GtkWidget *w_entry_contrast;        // Pointer to scale button
    GtkWidget *w_entry_temperature;     // Pointer to scale button
    GtkWidget *w_info_resolution;       // Pointer to label widget
    GtkWidget *w_btn_apply;             // Pointer to button widget
    GtkWidget *w_scale_zoom;            // Pointer to scale button
    GtkWidget *w_drawing_aera;          // Pointer to drawing aera 
    GdkPixbuf *tmp_img;                 // Global variable
    int tmp_w;                          // Global variable
    int tmp_h;                          // Global variable  
    gdImagePtr gd_img;                  // Global variable
    int gd_w;                           // Global variable
    int gd_h;                           // Global variable
    FILE *gd_out;                       // Global variable 
    char *resolution_info;              // Global variable
} app_widgets;

int main(int argc, char *argv[])
{
    GtkBuilder      *builder;
    GtkWidget       *window;
    app_widgets     *widgets = g_slice_new(app_widgets);

    gtk_init(&argc, &argv);

    builder = gtk_builder_new_from_file("src/glade/window_main.glade");

    window = GTK_WIDGET(gtk_builder_get_object(builder, "window_main"));
    widgets->w_dlg_file_choose = GTK_WIDGET(gtk_builder_get_object(builder, "dlg_file_choose"));
    widgets->w_drawing_aera = GTK_WIDGET(gtk_builder_get_object(builder, "drw_aera"));
    widgets->w_scale_brightness = GTK_WIDGET(gtk_builder_get_object(builder, "scale_brightness"));
    widgets->w_scale_contrast = GTK_WIDGET(gtk_builder_get_object(builder, "scale_contrast"));
    widgets->w_scale_temperature = GTK_WIDGET(gtk_builder_get_object(builder, "scale_temperature"));
    widgets->w_entry_brightness = GTK_WIDGET(gtk_builder_get_object(builder, "entry_brightness"));
    widgets->w_entry_contrast = GTK_WIDGET(gtk_builder_get_object(builder, "entry_contrast"));
    widgets->w_entry_temperature = GTK_WIDGET(gtk_builder_get_object(builder, "entry_temperature"));
    widgets->w_info_resolution = GTK_WIDGET(gtk_builder_get_object(builder, "info_resolution"));
    widgets->w_btn_apply = GTK_WIDGET(gtk_builder_get_object(builder, "btn_apply"));
    widgets->w_scale_zoom = GTK_WIDGET(gtk_builder_get_object(builder, "scale_zoom"));

    
    gtk_builder_connect_signals(builder, widgets);
    g_object_unref(builder);

    //Set default states & values
    gtk_widget_set_sensitive(widgets->w_scale_brightness, FALSE);
    gtk_widget_set_sensitive(widgets->w_scale_contrast, FALSE);
    gtk_widget_set_sensitive(widgets->w_scale_temperature, FALSE);
    gtk_widget_set_sensitive(widgets->w_entry_brightness, FALSE);
    gtk_widget_set_sensitive(widgets->w_entry_contrast, FALSE);
    gtk_widget_set_sensitive(widgets->w_entry_temperature, FALSE);
    gtk_widget_set_sensitive(widgets->w_btn_apply, FALSE);
    gtk_widget_set_sensitive(widgets->w_scale_zoom, FALSE);

    gtk_widget_show(window);                
    gtk_main();
    g_slice_free(app_widgets, widgets);
    return 0;
}

#pragma endregion Main

#pragma region Interface

////
// Functions
int on_draw_event(GtkWidget *widget, cairo_t *cr, app_widgets *app_wdgts)
{   
    if(app_wdgts->tmp_img != NULL)
    {
        printf("widget redraw\n");
        gdk_cairo_set_source_pixbuf(cr, app_wdgts->tmp_img, 0, 0);
        cairo_paint(cr);
        gtk_widget_set_size_request(app_wdgts->w_drawing_aera, app_wdgts->tmp_w, app_wdgts->tmp_h);
    }

    return 0;
}
////

////
// SignalCallbackFunctions
int on_menubar_open_activate(GtkMenuItem *menuitem, app_widgets *app_wdgts)
{
    gchar *file_name = NULL;        
    
    // Show the "Open Image" dialog box
    gtk_widget_show(app_wdgts->w_dlg_file_choose);
    
    // Check return value from Open Image dialog box to see if user clicked the Open button
    if (gtk_dialog_run(GTK_DIALOG (app_wdgts->w_dlg_file_choose)) == GTK_RESPONSE_OK) {

        // Get the file name from the dialog box
        char *file_name = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(app_wdgts->w_dlg_file_choose));
        if (file_name != NULL) {

            // Create the pixbuf image to display
            app_wdgts->tmp_img = gdk_pixbuf_new_from_file(file_name, NULL);

            // Save copies of the original files
            gdk_pixbuf_savev(app_wdgts->tmp_img, "cache/cp_img.png", "png", NULL, NULL, NULL);
            gdk_pixbuf_savev(app_wdgts->tmp_img, "cache/temp_img.png", "png", NULL, NULL, NULL);
            app_wdgts->gd_img = gdImageCreateFromFile("cache/cp_img.png");

            // Set sensitive to TRUE for all widgets
            gtk_widget_set_sensitive(app_wdgts->w_scale_brightness, TRUE);
            gtk_widget_set_sensitive(app_wdgts->w_scale_contrast, TRUE);
            gtk_widget_set_sensitive(app_wdgts->w_scale_temperature, TRUE);
            gtk_widget_set_sensitive(app_wdgts->w_entry_brightness, TRUE);
            gtk_widget_set_sensitive(app_wdgts->w_entry_contrast, TRUE);
            gtk_widget_set_sensitive(app_wdgts->w_entry_temperature, TRUE);
            gtk_widget_set_sensitive(app_wdgts->w_scale_zoom, TRUE);

            // gets info of the image
            int img_w = gdImageSX(app_wdgts->gd_img);
            int img_h = gdImageSY(app_wdgts->gd_img);

            app_wdgts->tmp_w = img_w;
            app_wdgts->tmp_h = img_h;
            app_wdgts->gd_w = img_w;
            app_wdgts->gd_h = img_h;

            // Redraw the drawing area to display the image in the UI
            gtk_widget_queue_draw(app_wdgts->w_drawing_aera);

            // Display the resolution in the UI
            asprintf(&app_wdgts->resolution_info, "%d x %d", app_wdgts->tmp_w, app_wdgts->tmp_h);
            gtk_label_set_text(GTK_LABEL(app_wdgts->w_info_resolution), app_wdgts->resolution_info);
        }
        g_free(file_name);
    }

    // Finished with the "Open Image" dialog box, so hide it
    gtk_widget_hide(app_wdgts->w_dlg_file_choose);

    return 0;
}

void on_menubar_quit_activate(GtkMenuItem *menuitem, app_widgets *app_wdgts)
{
    system("rm -r -f cache/*");
    gtk_main_quit();
}

void on_window_main_destroy()
{
    system("rm -r -f cache/*");
    gtk_main_quit();
}
////

#pragma endregion Interface

#pragma region ImageProcessing

//// SignalsCallbackFunctions
//
int on_scale_brightness_value_changed(GtkMenuItem *menuitem, app_widgets *app_wdgts)
{
    gtk_widget_set_sensitive(app_wdgts->w_scale_contrast, FALSE);
    gtk_widget_set_sensitive(app_wdgts->w_scale_temperature, FALSE);
    gtk_widget_set_sensitive(app_wdgts->w_entry_contrast, FALSE);
    gtk_widget_set_sensitive(app_wdgts->w_entry_temperature, FALSE);
    gtk_widget_set_sensitive(app_wdgts->w_scale_zoom, FALSE);
    gtk_widget_set_sensitive(app_wdgts->w_btn_apply, TRUE);

    app_wdgts->gd_img = gdImageCreateFromFile("cache/cp_img.png");
    Brightness(app_wdgts->gd_img, gtk_range_get_value(GTK_RANGE(app_wdgts->w_scale_brightness)), app_wdgts->gd_out);
    app_wdgts->tmp_img = gdk_pixbuf_new_from_file("cache/temp_img.png", NULL);
    update_buffer(app_wdgts);
    
    return 0;
}

int on_scale_contrast_value_changed(GtkMenuItem *menuitem, app_widgets *app_wdgts)
{
    gtk_widget_set_sensitive(app_wdgts->w_scale_brightness, FALSE);
    gtk_widget_set_sensitive(app_wdgts->w_scale_temperature, FALSE);
    gtk_widget_set_sensitive(app_wdgts->w_entry_brightness, FALSE);
    gtk_widget_set_sensitive(app_wdgts->w_entry_temperature, FALSE);
    gtk_widget_set_sensitive(app_wdgts->w_scale_zoom, FALSE);
    gtk_widget_set_sensitive(app_wdgts->w_btn_apply, TRUE);

    app_wdgts->gd_img = gdImageCreateFromFile("cache/cp_img.png");
    Contrast(app_wdgts->gd_img, gtk_range_get_value(GTK_RANGE(app_wdgts->w_scale_contrast)), app_wdgts->gd_out);
    app_wdgts->tmp_img = gdk_pixbuf_new_from_file("cache/temp_img.png", NULL);
    update_buffer(app_wdgts);
    
    return 0;
}

int on_scale_temperature_value_changed(GtkMenuItem *menuitem, app_widgets *app_wdgts)
{
    gtk_widget_set_sensitive(app_wdgts->w_scale_brightness, FALSE);
    gtk_widget_set_sensitive(app_wdgts->w_scale_contrast, FALSE);
    gtk_widget_set_sensitive(app_wdgts->w_entry_brightness, FALSE);
    gtk_widget_set_sensitive(app_wdgts->w_entry_contrast, FALSE);
    gtk_widget_set_sensitive(app_wdgts->w_scale_zoom, FALSE);
    gtk_widget_set_sensitive(app_wdgts->w_btn_apply, TRUE);

    app_wdgts->gd_img = gdImageCreateFromFile("cache/cp_img.png");
    Temperature(app_wdgts->gd_img, gtk_range_get_value(GTK_RANGE(app_wdgts->w_scale_temperature)), app_wdgts->gd_out);
    app_wdgts->tmp_img = gdk_pixbuf_new_from_file("cache/temp_img.png", NULL);
    update_buffer(app_wdgts);
    
    return 0;
}

int on_entry_brightness_activate(GtkMenuItem *menuitem, app_widgets *app_wdgts)
{
    gtk_widget_set_sensitive(app_wdgts->w_scale_contrast, FALSE);
    gtk_widget_set_sensitive(app_wdgts->w_scale_temperature, FALSE);
    gtk_widget_set_sensitive(app_wdgts->w_entry_contrast, FALSE);
    gtk_widget_set_sensitive(app_wdgts->w_entry_temperature, FALSE);
    gtk_widget_set_sensitive(app_wdgts->w_scale_zoom, FALSE);
    gtk_widget_set_sensitive(app_wdgts->w_btn_apply, TRUE);

    on_scale_brightness_value_changed(menuitem, app_wdgts);
    
    return 0;
}

int on_entry_contrast_activate(GtkMenuItem *menuitem, app_widgets *app_wdgts)
{
    gtk_widget_set_sensitive(app_wdgts->w_scale_brightness, FALSE);
    gtk_widget_set_sensitive(app_wdgts->w_scale_temperature, FALSE);
    gtk_widget_set_sensitive(app_wdgts->w_entry_brightness, FALSE);
    gtk_widget_set_sensitive(app_wdgts->w_entry_temperature, FALSE);
    gtk_widget_set_sensitive(app_wdgts->w_scale_zoom, FALSE);
    gtk_widget_set_sensitive(app_wdgts->w_btn_apply, TRUE);

    on_scale_contrast_value_changed(menuitem, app_wdgts);
    
    return 0;
}

int on_entry_temperature_activate(GtkMenuItem *menuitem, app_widgets *app_wdgts)
{
    gtk_widget_set_sensitive(app_wdgts->w_scale_brightness, FALSE);
    gtk_widget_set_sensitive(app_wdgts->w_scale_contrast, FALSE);
    gtk_widget_set_sensitive(app_wdgts->w_entry_brightness, FALSE);
    gtk_widget_set_sensitive(app_wdgts->w_entry_contrast, FALSE);
    gtk_widget_set_sensitive(app_wdgts->w_scale_zoom, FALSE);
    gtk_widget_set_sensitive(app_wdgts->w_btn_apply, TRUE);

    on_scale_temperature_value_changed(menuitem, app_wdgts);
    
    return 0;
}

int on_btn_apply_clicked(GtkMenuItem *menuitem, app_widgets *app_wdgts)
{
    gtk_widget_set_sensitive(app_wdgts->w_scale_brightness, TRUE);
    gtk_widget_set_sensitive(app_wdgts->w_scale_contrast, TRUE);
    gtk_widget_set_sensitive(app_wdgts->w_scale_temperature, TRUE);
    gtk_widget_set_sensitive(app_wdgts->w_entry_brightness, TRUE);
    gtk_widget_set_sensitive(app_wdgts->w_entry_contrast, TRUE);
    gtk_widget_set_sensitive(app_wdgts->w_entry_temperature, TRUE);
    gtk_widget_set_sensitive(app_wdgts->w_scale_zoom, TRUE);
    gtk_widget_set_sensitive(app_wdgts->w_btn_apply, FALSE);


    gdk_pixbuf_savev(app_wdgts->tmp_img, "cache/cp_img.png", "png", NULL, NULL, NULL);

    return 0;
}
////
//

#pragma endregion ImageProcessing

#pragma region Utils

//// Functions
//
int update_buffer(app_widgets *app_wdgts)
{
    app_wdgts->tmp_img = gdk_pixbuf_scale_simple(app_wdgts->tmp_img, app_wdgts->tmp_w, app_wdgts->tmp_h, GDK_INTERP_NEAREST);
    gtk_widget_set_size_request(app_wdgts->w_drawing_aera, app_wdgts->tmp_w, app_wdgts->tmp_h);
    gtk_widget_queue_draw(app_wdgts->w_drawing_aera);
    return 0;
}
//
////

////
// SignalCallbackFunctions
int on_scale_zoom_value_changed(GtkMenuItem *menuitem, app_widgets *app_wdgts)
{
    GdkPixbuf *cp_img = gdk_pixbuf_new_from_file("cache/cp_img.png", NULL);
    int value = gtk_range_get_value(GTK_RANGE(app_wdgts->w_scale_zoom));

    if(value == 0)
        value = 1;    

    else if(value > 0)
    {
        app_wdgts->tmp_w = app_wdgts->gd_w * value;
        app_wdgts->tmp_h = app_wdgts->gd_h * value;
    }        
    else
    {
        app_wdgts->tmp_w = app_wdgts->gd_w / ((-1) * value);
        app_wdgts->tmp_h = app_wdgts->gd_h / ((-1) * value);
    }

    app_wdgts->tmp_img = gdk_pixbuf_scale_simple(cp_img, app_wdgts->tmp_w, app_wdgts->tmp_h, GDK_INTERP_NEAREST);
    gtk_widget_queue_draw(app_wdgts->w_drawing_aera);

    return 0;
}
//
////

#pragma endregion Utils

