#define _GNU_SOURCE 
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <gtk/gtk.h>
#include <gdk-pixbuf/gdk-pixbuf.h>

#include <gd.h>

#include ".h/filters.h"
#include ".h/image_settings.h"

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
} app_widgets;

GdkPixbuf *temp_img;
gdImagePtr gd_img;
FILE *gd_out; 
char * resolution_info;

int main(int argc, char *argv[])
{
    GtkBuilder      *builder;
    GtkWidget       *window;
    app_widgets     *widgets = g_slice_new(app_widgets);

    gtk_init(&argc, &argv);

    builder = gtk_builder_new_from_file("src/glade/window_main.glade");

    window = GTK_WIDGET(gtk_builder_get_object(builder, "window_main"));
    widgets->w_dlg_file_choose = GTK_WIDGET(gtk_builder_get_object(builder, "dlg_file_choose"));
    widgets->w_img_main = GTK_WIDGET(gtk_builder_get_object(builder, "img_main"));
    widgets->w_scale_brightness = GTK_WIDGET(gtk_builder_get_object(builder, "scale_brightness"));
    widgets->w_scale_contrast = GTK_WIDGET(gtk_builder_get_object(builder, "scale_contrast"));
    widgets->w_scale_temperature = GTK_WIDGET(gtk_builder_get_object(builder, "scale_temperature"));
    widgets->w_entry_brightness = GTK_WIDGET(gtk_builder_get_object(builder, "entry_brightness"));
    widgets->w_entry_contrast = GTK_WIDGET(gtk_builder_get_object(builder, "entry_contrast"));
    widgets->w_entry_temperature = GTK_WIDGET(gtk_builder_get_object(builder, "entry_temperature"));
    widgets->w_info_resolution = GTK_WIDGET(gtk_builder_get_object(builder, "info_resolution"));
    widgets->w_btn_apply = GTK_WIDGET(gtk_builder_get_object(builder, "btn_apply"));
    
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

    gtk_widget_show(window);                
    gtk_main();
    g_slice_free(app_widgets, widgets);
    return 0;
}

// File --> Open
void on_menubar_open_activate(GtkMenuItem *menuitem, app_widgets *app_wdgts)
{
    gchar *file_name = NULL;        // Name of file to open from dialog box
    
    // Show the "Open Image" dialog box
    gtk_widget_show(app_wdgts->w_dlg_file_choose);
    
    // Check return value from Open Image dialog box to see if user clicked the Open button
    if (gtk_dialog_run(GTK_DIALOG (app_wdgts->w_dlg_file_choose)) == GTK_RESPONSE_OK) {
        // Get the file name from the dialog box
        file_name = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(app_wdgts->w_dlg_file_choose));
        if (file_name != NULL) {

            temp_img = gdk_pixbuf_new_from_file(file_name, NULL);
            gdk_pixbuf_savev(temp_img, "cache/cp_img.png", "png", NULL, NULL, NULL);
            gdk_pixbuf_savev(temp_img, "cache/temp_img.png", "png", NULL, NULL, NULL);

            gd_img = gdImageCreateFromFile("cache/cp_img.png");
            gtk_image_set_from_pixbuf(GTK_IMAGE(app_wdgts->w_img_main), temp_img);

            gtk_widget_set_sensitive(app_wdgts->w_scale_brightness, TRUE);
            gtk_widget_set_sensitive(app_wdgts->w_scale_contrast, TRUE);
            gtk_widget_set_sensitive(app_wdgts->w_scale_temperature, TRUE);
            gtk_widget_set_sensitive(app_wdgts->w_entry_brightness, TRUE);
            gtk_widget_set_sensitive(app_wdgts->w_entry_contrast, TRUE);
            gtk_widget_set_sensitive(app_wdgts->w_entry_temperature, TRUE);

            int img_w = gdk_pixbuf_get_width(temp_img);
            int img_h = gdk_pixbuf_get_height(temp_img);

            asprintf(&resolution_info, "%d x %d", img_w, img_h);
            gtk_label_set_text(GTK_LABEL(app_wdgts->w_info_resolution), resolution_info);
        }
        g_free(file_name);
    }

    // Finished with the "Open Image" dialog box, so hide it
    gtk_widget_hide(app_wdgts->w_dlg_file_choose);
}

on_scale_brightness_value_changed(GtkMenuItem *menuitem, app_widgets *app_wdgts)
{
    gtk_widget_set_sensitive(app_wdgts->w_scale_contrast, FALSE);
    gtk_widget_set_sensitive(app_wdgts->w_scale_temperature, FALSE);
    gtk_widget_set_sensitive(app_wdgts->w_entry_contrast, FALSE);
    gtk_widget_set_sensitive(app_wdgts->w_entry_temperature, FALSE);
    gtk_widget_set_sensitive(app_wdgts->w_btn_apply, TRUE);

    gd_img = gdImageCreateFromFile("cache/cp_img.png");
    Brightness(gd_img, gtk_range_get_value(GTK_RANGE(app_wdgts->w_scale_brightness)), gd_out);
    temp_img = gdk_pixbuf_new_from_file("cache/temp_img.png", NULL);
    gtk_image_set_from_pixbuf(GTK_IMAGE(app_wdgts->w_img_main), temp_img);
    return TRUE;
}

on_scale_contrast_value_changed(GtkMenuItem *menuitem, app_widgets *app_wdgts)
{
    gtk_widget_set_sensitive(app_wdgts->w_scale_brightness, FALSE);
    gtk_widget_set_sensitive(app_wdgts->w_scale_temperature, FALSE);
    gtk_widget_set_sensitive(app_wdgts->w_entry_brightness, FALSE);
    gtk_widget_set_sensitive(app_wdgts->w_entry_temperature, FALSE);
    gtk_widget_set_sensitive(app_wdgts->w_btn_apply, TRUE);

    gd_img = gdImageCreateFromFile("cache/cp_img.png");
    Contrast(gd_img, gtk_range_get_value(GTK_RANGE(app_wdgts->w_scale_contrast)), gd_out);
    temp_img = gdk_pixbuf_new_from_file("cache/temp_img.png", NULL);
    gtk_image_set_from_pixbuf(GTK_IMAGE(app_wdgts->w_img_main), temp_img);
    return TRUE;
}

on_scale_temperature_value_changed(GtkMenuItem *menuitem, app_widgets *app_wdgts)
{
    gtk_widget_set_sensitive(app_wdgts->w_scale_brightness, FALSE);
    gtk_widget_set_sensitive(app_wdgts->w_scale_contrast, FALSE);
    gtk_widget_set_sensitive(app_wdgts->w_entry_brightness, FALSE);
    gtk_widget_set_sensitive(app_wdgts->w_entry_contrast, FALSE);
    gtk_widget_set_sensitive(app_wdgts->w_btn_apply, TRUE);

    gd_img = gdImageCreateFromFile("cache/cp_img.png");
    Temperature(gd_img, gtk_range_get_value(GTK_RANGE(app_wdgts->w_scale_temperature)), gd_out);
    temp_img = gdk_pixbuf_new_from_file("cache/temp_img.png", NULL);
    gtk_image_set_from_pixbuf(GTK_IMAGE(app_wdgts->w_img_main), temp_img);
    return TRUE;
}

on_entry_brightness_activate(GtkMenuItem *menuitem, app_widgets *app_wdgts)
{
    gtk_widget_set_sensitive(app_wdgts->w_scale_contrast, FALSE);
    gtk_widget_set_sensitive(app_wdgts->w_scale_temperature, FALSE);
    gtk_widget_set_sensitive(app_wdgts->w_entry_contrast, FALSE);
    gtk_widget_set_sensitive(app_wdgts->w_entry_temperature, FALSE);
    gtk_widget_set_sensitive(app_wdgts->w_btn_apply, TRUE);

    on_scale_brightness_value_changed(menuitem, app_wdgts);
    return TRUE;
}

on_entry_contrast_activate(GtkMenuItem *menuitem, app_widgets *app_wdgts)
{
    gtk_widget_set_sensitive(app_wdgts->w_scale_brightness, FALSE);
    gtk_widget_set_sensitive(app_wdgts->w_scale_temperature, FALSE);
    gtk_widget_set_sensitive(app_wdgts->w_entry_brightness, FALSE);
    gtk_widget_set_sensitive(app_wdgts->w_entry_temperature, FALSE);
    gtk_widget_set_sensitive(app_wdgts->w_btn_apply, TRUE);

    on_scale_contrast_value_changed(menuitem, app_wdgts);
    return TRUE;
}

on_entry_temperature_activate(GtkMenuItem *menuitem, app_widgets *app_wdgts)
{
    gtk_widget_set_sensitive(app_wdgts->w_scale_brightness, FALSE);
    gtk_widget_set_sensitive(app_wdgts->w_scale_contrast, FALSE);
    gtk_widget_set_sensitive(app_wdgts->w_entry_brightness, FALSE);
    gtk_widget_set_sensitive(app_wdgts->w_entry_contrast, FALSE);
    gtk_widget_set_sensitive(app_wdgts->w_btn_apply, TRUE);

    on_scale_temperature_value_changed(menuitem, app_wdgts);
    return TRUE;
}

on_btn_apply_clicked(GtkMenuItem *menuitem, app_widgets *app_wdgts)
{
    gtk_widget_set_sensitive(app_wdgts->w_scale_brightness, TRUE);
    gtk_widget_set_sensitive(app_wdgts->w_scale_contrast, TRUE);
    gtk_widget_set_sensitive(app_wdgts->w_scale_temperature, TRUE);
    gtk_widget_set_sensitive(app_wdgts->w_entry_brightness, TRUE);
    gtk_widget_set_sensitive(app_wdgts->w_entry_contrast, TRUE);
    gtk_widget_set_sensitive(app_wdgts->w_entry_temperature, TRUE);
    gtk_widget_set_sensitive(app_wdgts->w_btn_apply, FALSE);

    gdk_pixbuf_savev(temp_img, "cache/cp_img.png", "png", NULL, NULL, NULL);

    return TRUE;
}

// File --> Quit
void on_menubar_quit_activate(GtkMenuItem *menuitem, app_widgets *app_wdgts)
{
    gtk_main_quit();
}

// called when window is closed
void on_window_main_destroy()
{
    gtk_main_quit();
}
