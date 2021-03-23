#define _GNU_SOURCE

#include <unistd.h>
#include <sys/types.h>
#include <signal.h>
#include <unistd.h>
#include <math.h>
#include <time.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>

#include <gtk/gtk.h>
#include <gdk-pixbuf/gdk-pixbuf.h>

#include <gd.h>

#include ".h/filters.h"
#include ".h/image_settings.h"

/*
 * Group: Orion
 * Project: Nebula
 * Developpers: Tom TERMAAT, André HOANG, Albert EL-HELOU, Adrien HICAUBERT
 * Description: Orion Nebula is a raster graphics editor.
 */

#pragma region Main

struct Point {
    int x;
    int y;
    double red, green, blue, white;
    struct Point *next;
};

struct Point *p1, *p2, *start;

double red, green, blue;

typedef struct {

    // Pointer to GtkWidget
    GtkWidget *w_window;               
    GtkWidget *w_dlg_file_choose;       
    GtkWidget *w_img_main;             
    GtkWidget *w_scale_brightness;      
    GtkWidget *w_scale_contrast;       
    GtkWidget *w_scale_temperature;     
    GtkWidget *w_entry_brightness;      
    GtkWidget *w_entry_contrast;        
    GtkWidget *w_entry_temperature;     
    GtkWidget *w_info_resolution;       
    GtkWidget *w_info_zoom;             
    GtkWidget *w_btn_apply;             
    GtkWidget *w_scale_zoom;            
    GtkWidget *w_drawing_aera;           
    GtkWidget *w_prev_none;             
    GtkWidget *w_prev_oldschool;        
    GtkWidget *w_prev_glowfilter;       
    GtkWidget *w_prev_redflagfilter;    
    GtkWidget *w_prev_summertimefilter;  
    GtkWidget *w_check_none;
    GtkWidget *w_check_oldschool;
    GtkWidget *w_check_glowfilter;
    GtkWidget *w_check_redflagfilter;
    GtkWidget *w_check_summertimefilter;
    
    // Global variables
    GdkPixbuf *tmp_img;                 
    int tmp_w;                          
    int tmp_h;                            
    gdImagePtr gd_img;                 
    int gd_w;                           
    int gd_h;                           
    FILE *gd_out;                       
    char *resolution_info;              
    cairo_surface_t *cr_surface    

} app_widgets;

int main(int argc, char *argv[])
{
    GtkBuilder      *builder;
    app_widgets     *widgets = g_slice_new(app_widgets);

    p1 = p2 = start = NULL;

    red = green = blue = 0.0;

    gtk_init(&argc, &argv);

    builder = gtk_builder_new_from_file("src/glade/window_main.glade");

    widgets->w_window = GTK_WIDGET(gtk_builder_get_object(builder, "window_main"));
    widgets->w_dlg_file_choose = GTK_WIDGET(gtk_builder_get_object(builder, "dlg_file_choose"));
    widgets->w_drawing_aera = GTK_WIDGET(gtk_builder_get_object(builder, "drw_area"));
    widgets->w_scale_brightness = GTK_WIDGET(gtk_builder_get_object(builder, "scale_brightness"));
    widgets->w_scale_contrast = GTK_WIDGET(gtk_builder_get_object(builder, "scale_contrast"));
    widgets->w_scale_temperature = GTK_WIDGET(gtk_builder_get_object(builder, "scale_temperature"));
    widgets->w_entry_brightness = GTK_WIDGET(gtk_builder_get_object(builder, "entry_brightness"));
    widgets->w_entry_contrast = GTK_WIDGET(gtk_builder_get_object(builder, "entry_contrast"));
    widgets->w_entry_temperature = GTK_WIDGET(gtk_builder_get_object(builder, "entry_temperature"));
    widgets->w_info_resolution = GTK_WIDGET(gtk_builder_get_object(builder, "info_resolution"));
    widgets->w_info_zoom = GTK_WIDGET(gtk_builder_get_object(builder, "info_zoom"));
    widgets->w_btn_apply = GTK_WIDGET(gtk_builder_get_object(builder, "btn_apply"));
    widgets->w_scale_zoom = GTK_WIDGET(gtk_builder_get_object(builder, "scale_zoom"));
    widgets->w_prev_none = GTK_WIDGET(gtk_builder_get_object(builder, "prev_none"));
    widgets->w_prev_oldschool = GTK_WIDGET(gtk_builder_get_object(builder, "prev_oldschool"));
    widgets->w_prev_glowfilter = GTK_WIDGET(gtk_builder_get_object(builder, "prev_glowfilter"));
    widgets->w_prev_redflagfilter = GTK_WIDGET(gtk_builder_get_object(builder, "prev_redflagfilter"));
    widgets->w_prev_summertimefilter = GTK_WIDGET(gtk_builder_get_object(builder, "prev_summertimefilter"));
    widgets->w_check_none = GTK_WIDGET(gtk_builder_get_object(builder, "check_none"));
    widgets->w_check_oldschool = GTK_WIDGET(gtk_builder_get_object(builder, "check_oldschool"));
    widgets->w_check_glowfilter = GTK_WIDGET(gtk_builder_get_object(builder, "check_glowfilter"));
    widgets->w_check_redflagfilter = GTK_WIDGET(gtk_builder_get_object(builder, "check_redflagfilter"));
    widgets->w_check_summertimefilter = GTK_WIDGET(gtk_builder_get_object(builder, "check_summertimefilter"));

    gtk_builder_connect_signals(builder, widgets);
    g_object_unref(builder);

    gtk_widget_set_events(widgets->w_drawing_aera, GDK_BUTTON_MOTION_MASK | GDK_BUTTON_PRESS_MASK);

    //Set default states & values
    gtk_widget_set_sensitive(widgets->w_scale_brightness, FALSE);
    gtk_widget_set_sensitive(widgets->w_scale_contrast, FALSE);
    gtk_widget_set_sensitive(widgets->w_scale_temperature, FALSE);
    gtk_widget_set_sensitive(widgets->w_entry_brightness, FALSE);
    gtk_widget_set_sensitive(widgets->w_entry_contrast, FALSE);
    gtk_widget_set_sensitive(widgets->w_entry_temperature, FALSE);
    gtk_widget_set_sensitive(widgets->w_btn_apply, FALSE);
    gtk_widget_set_sensitive(widgets->w_scale_zoom, FALSE);
    gtk_widget_set_sensitive(widgets->w_check_none, FALSE);
    gtk_widget_set_sensitive(widgets->w_check_oldschool, FALSE);
    gtk_widget_set_sensitive(widgets->w_check_glowfilter,FALSE);
    gtk_widget_set_sensitive(widgets->w_check_redflagfilter, FALSE);
    gtk_widget_set_sensitive(widgets->w_check_summertimefilter, FALSE);

    gtk_widget_show(widgets->w_window);                
    gtk_main();
    g_slice_free(app_widgets, widgets);
    return 0;
}

#pragma endregion Main

#pragma region Interface

////
// Functions
int update_buffer(app_widgets *app_wdgts)
{
    app_wdgts->tmp_img = gdk_pixbuf_scale_simple(app_wdgts->tmp_img, app_wdgts->tmp_w, app_wdgts->tmp_h, GDK_INTERP_NEAREST);
    gtk_widget_set_size_request(app_wdgts->w_drawing_aera, app_wdgts->tmp_w, app_wdgts->tmp_h);
    gtk_widget_queue_draw(app_wdgts->w_drawing_aera);
    return 0;
}

int update_info_zoom(app_widgets *app_wdgts)
{
    int scale_value = gtk_range_get_value(app_wdgts->w_scale_zoom);

    float zoom_percentage = ((float)scale_value/20)*100;

    char *zoom_info;
    asprintf(&zoom_info, "%.0f %%", zoom_percentage);
    gtk_label_set_text(GTK_LABEL(app_wdgts->w_info_zoom), zoom_info);
    return 0;
}
//
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
            gtk_widget_set_sensitive(app_wdgts->w_check_none, TRUE);
            gtk_widget_set_sensitive(app_wdgts->w_check_oldschool, TRUE);
            gtk_widget_set_sensitive(app_wdgts->w_check_glowfilter,TRUE);
            gtk_widget_set_sensitive(app_wdgts->w_check_redflagfilter, TRUE);
            gtk_widget_set_sensitive(app_wdgts->w_check_summertimefilter, TRUE);

            // gets info of the image
            int img_w = gdImageSX(app_wdgts->gd_img);
            int img_h = gdImageSY(app_wdgts->gd_img);

            app_wdgts->tmp_w = img_w;
            app_wdgts->tmp_h = img_h;
            app_wdgts->gd_w = img_w;
            app_wdgts->gd_h = img_h;

            // Redraw the drawing area to display the image in the UI
            gtk_widget_queue_draw(app_wdgts->w_drawing_aera);
            update_info_zoom(app_wdgts);

            // Display the resolution in the UI
            asprintf(&app_wdgts->resolution_info, "%d x %d", app_wdgts->tmp_w, app_wdgts->tmp_h);
            gtk_label_set_text(GTK_LABEL(app_wdgts->w_info_resolution), app_wdgts->resolution_info);

            // Update preview

            gtk_toggle_button_set_active(app_wdgts->w_check_none, TRUE);
            update_all_prev(app_wdgts);
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
    update_info_zoom(app_wdgts);

    return 0;
}
//
////

#pragma endregion Interface

#pragma region Drawing

//// Functions
//
static void draw_brush(GtkWidget *widget, gdouble x, gdouble y, app_widgets *app_wdgts)
{
    p1 = malloc(sizeof(struct Point));
    if(p1 == NULL) { printf("out of memory\n"); abort(); }
    p1->x = x;
    p1->y = y; 
    p1->red = red;
    p1->green = green;
    p1->blue = blue;
    p1->next = start;
    start = p1;
    gtk_widget_queue_draw(app_wdgts->w_drawing_aera);
}

//// SignalsCallbackFunctions
//
int on_draw_button_press_event(GtkWidget *widget, GdkEventButton *event, app_widgets *app_wdgts)
{
    draw_brush(widget, event->x, event->y, app_wdgts);
    return 0;
}

void on_clear_clicked(GtkWidget *b1, app_widgets *app_wdgts)
{
    p1 = start;
    while(p1) { p2 = p1->next; free(p1); p1 = p2; }
    start = NULL;
    gtk_widget_queue_draw(app_wdgts->w_drawing_aera);
}

int on_draw_motion_notify_event(GtkWidget *widget, GdkEventMotion *event, app_widgets *app_wdgts)
{
    if(event->state & GDK_BUTTON1_MASK)
        draw_brush(widget, event->x, event->y, app_wdgts);

    return 0;
}

void on_red_clicked(GtkWidget *b1) { red = 1.0; green = 0.0; blue = 0.0; }

void on_green_clicked(GtkWidget *b1) { red = 0.0; green = 1.0; blue = 0.0; }

void on_blue_clicked(GtkWidget *b1) { red = 0.0; green = 0.0; blue = 1.0; }

void on_white_clicked(GtkWidget *b1) { red = 1.0; green = 1.0; blue = 1.0; }

int on_draw_event(GtkWidget *widget, cairo_t *cr, app_widgets *app_wdgts)
{   
    if(app_wdgts->tmp_img != NULL)
    {
        gdk_cairo_set_source_pixbuf(cr, app_wdgts->tmp_img, 0, 0);
        cairo_paint(cr);
        gtk_widget_set_size_request(app_wdgts->w_drawing_aera, app_wdgts->tmp_w, app_wdgts->tmp_h);        

        if(start != NULL)
        {
            guint width, height;

            width = gtk_widget_get_allocated_width(widget);
            height = gtk_widget_get_allocated_height(widget);

            cairo_set_line_width(cr, 1.0);

            int old_x = start->x;
            int old_y = start->y;

            p1 = start->next;        

            while(p1 != NULL)
            {
                cairo_set_source_rgb(cr, p1->red, p1->green, p1->blue);

                cairo_move_to(cr, (double) old_x, (double) old_y);
                cairo_line_to(cr, (double) p1->x, (double) p1->y);
                cairo_stroke(cr);

                old_x = p1->x;
                old_y = p1->y;
                p1 = p1->next;
            }
        }

        app_wdgts->cr_surface = cairo_get_target(cr);
    }

    return 0;
}
//
////

#pragma endregion Drawing

#pragma region ImageProcessing

//// Functions
//
int update_all_prev(app_widgets *app_wdgts)
{
    GdkPixbuf *prev_none = gdk_pixbuf_new_from_file("cache/cp_img.png", NULL);
    prev_none = gdk_pixbuf_scale_simple(prev_none, app_wdgts->gd_w/5, app_wdgts->gd_h/5, GDK_INTERP_NEAREST);
    gtk_image_set_from_pixbuf(app_wdgts->w_prev_none, prev_none);

    gdImagePtr gd_prev = gdImageCreateFromFile("cache/temp_img.png");
    FILE *gd_out;
    OldSchool_Filter(gd_prev, app_wdgts->gd_out, "cache/prev_img.png");
    GdkPixbuf *prev_oldschool = gdk_pixbuf_new_from_file("cache/prev_img.png", NULL);
    prev_oldschool = gdk_pixbuf_scale_simple(prev_oldschool, app_wdgts->gd_w/5, app_wdgts->gd_h/5, GDK_INTERP_NEAREST);
    gtk_image_set_from_pixbuf(app_wdgts->w_prev_oldschool, prev_oldschool);

    gd_prev = gdImageCreateFromFile("cache/temp_img.png");
    Glow_Filter(gd_prev, app_wdgts->gd_out, "cache/prev_img.png");
    GdkPixbuf *prev_glowfilter = gdk_pixbuf_new_from_file("cache/prev_img.png", NULL);
    prev_glowfilter = gdk_pixbuf_scale_simple(prev_glowfilter, app_wdgts->gd_w/5, app_wdgts->gd_h/5, GDK_INTERP_NEAREST);
    gtk_image_set_from_pixbuf(app_wdgts->w_prev_glowfilter, prev_glowfilter);

    gd_prev = gdImageCreateFromFile("cache/temp_img.png");
    RedFlag_Filter(gd_prev, app_wdgts->gd_out, "cache/prev_img.png");
    GdkPixbuf *prev_redflagfilter = gdk_pixbuf_new_from_file("cache/prev_img.png", NULL);
    prev_redflagfilter = gdk_pixbuf_scale_simple(prev_redflagfilter, app_wdgts->gd_w/5, app_wdgts->gd_h/5, GDK_INTERP_NEAREST);
    gtk_image_set_from_pixbuf(app_wdgts->w_prev_redflagfilter, prev_redflagfilter);

    gd_prev = gdImageCreateFromFile("cache/temp_img.png");
    SummerTime_Filter(gd_prev, app_wdgts->gd_out, "cache/prev_img.png");
    GdkPixbuf *prev_summertimefilter = gdk_pixbuf_new_from_file("cache/prev_img.png", NULL);
    prev_summertimefilter = gdk_pixbuf_scale_simple(prev_summertimefilter, app_wdgts->gd_w/5, app_wdgts->gd_h/5, GDK_INTERP_NEAREST);
    gtk_image_set_from_pixbuf(app_wdgts->w_prev_summertimefilter, prev_summertimefilter);

    return 0;
}

//// SignalsCallbackFunctions
//
int on_scale_brightness_value_changed(GtkMenuItem *menuitem, app_widgets *app_wdgts)
{
    gtk_widget_set_sensitive(app_wdgts->w_scale_contrast, FALSE);
    gtk_widget_set_sensitive(app_wdgts->w_scale_temperature, FALSE);
    gtk_widget_set_sensitive(app_wdgts->w_entry_contrast, FALSE);
    gtk_widget_set_sensitive(app_wdgts->w_entry_temperature, FALSE);
    gtk_widget_set_sensitive(app_wdgts->w_scale_zoom, FALSE);
    gtk_widget_set_sensitive(app_wdgts->w_check_none, FALSE);
    gtk_widget_set_sensitive(app_wdgts->w_check_oldschool, FALSE);
    gtk_widget_set_sensitive(app_wdgts->w_check_glowfilter,FALSE);
    gtk_widget_set_sensitive(app_wdgts->w_check_redflagfilter, FALSE);
    gtk_widget_set_sensitive(app_wdgts->w_check_summertimefilter, FALSE);
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
    gtk_widget_set_sensitive(app_wdgts->w_check_none, FALSE);
    gtk_widget_set_sensitive(app_wdgts->w_check_oldschool, FALSE);
    gtk_widget_set_sensitive(app_wdgts->w_check_glowfilter,FALSE);
    gtk_widget_set_sensitive(app_wdgts->w_check_redflagfilter, FALSE);
    gtk_widget_set_sensitive(app_wdgts->w_check_summertimefilter, FALSE);
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
    gtk_widget_set_sensitive(app_wdgts->w_check_none, FALSE);
    gtk_widget_set_sensitive(app_wdgts->w_check_oldschool, FALSE);
    gtk_widget_set_sensitive(app_wdgts->w_check_glowfilter,FALSE);
    gtk_widget_set_sensitive(app_wdgts->w_check_redflagfilter, FALSE);
    gtk_widget_set_sensitive(app_wdgts->w_check_summertimefilter, FALSE);
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
    gtk_widget_set_sensitive(app_wdgts->w_check_none, FALSE);
    gtk_widget_set_sensitive(app_wdgts->w_check_oldschool, FALSE);
    gtk_widget_set_sensitive(app_wdgts->w_check_glowfilter,FALSE);
    gtk_widget_set_sensitive(app_wdgts->w_check_redflagfilter, FALSE);
    gtk_widget_set_sensitive(app_wdgts->w_check_summertimefilter, FALSE);
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
    gtk_widget_set_sensitive(app_wdgts->w_check_none, FALSE);
    gtk_widget_set_sensitive(app_wdgts->w_check_oldschool, FALSE);
    gtk_widget_set_sensitive(app_wdgts->w_check_glowfilter,FALSE);
    gtk_widget_set_sensitive(app_wdgts->w_check_redflagfilter, FALSE);
    gtk_widget_set_sensitive(app_wdgts->w_check_summertimefilter, FALSE);
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
    gtk_widget_set_sensitive(app_wdgts->w_check_none, FALSE);
    gtk_widget_set_sensitive(app_wdgts->w_check_oldschool, FALSE);
    gtk_widget_set_sensitive(app_wdgts->w_check_glowfilter,FALSE);
    gtk_widget_set_sensitive(app_wdgts->w_check_redflagfilter, FALSE);
    gtk_widget_set_sensitive(app_wdgts->w_check_summertimefilter, FALSE);
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
    gtk_widget_set_sensitive(app_wdgts->w_check_none, TRUE);
    gtk_widget_set_sensitive(app_wdgts->w_check_oldschool, TRUE);
    gtk_widget_set_sensitive(app_wdgts->w_check_glowfilter,TRUE);
    gtk_widget_set_sensitive(app_wdgts->w_check_redflagfilter, TRUE);
    gtk_widget_set_sensitive(app_wdgts->w_check_summertimefilter, TRUE);
    gtk_widget_set_sensitive(app_wdgts->w_scale_zoom, TRUE);
    gtk_widget_set_sensitive(app_wdgts->w_btn_apply, FALSE);

    gtk_toggle_button_set_active(app_wdgts->w_check_none, FALSE);
    gtk_toggle_button_set_active(app_wdgts->w_check_oldschool, FALSE);
    gtk_toggle_button_set_active(app_wdgts->w_check_glowfilter,FALSE);
    gtk_toggle_button_set_active(app_wdgts->w_check_redflagfilter, FALSE);
    gtk_toggle_button_set_active(app_wdgts->w_check_summertimefilter, FALSE);

    if(gtk_toggle_button_get_active(app_wdgts->w_check_oldschool))
    {
        OldSchool_Filter(app_wdgts->gd_img, app_wdgts->gd_out, "cache/temp_img.png");
        app_wdgts->tmp_img = gdk_pixbuf_new_from_file("cache/temp_img.png", NULL);
    }

    if(gtk_toggle_button_get_active(app_wdgts->w_check_glowfilter))
    {
        Glow_Filter(app_wdgts->gd_img, app_wdgts->gd_out, "cache/temp_img.png");
        app_wdgts->tmp_img = gdk_pixbuf_new_from_file("cache/temp_img.png", NULL);
    }

    if(gtk_toggle_button_get_active(app_wdgts->w_check_redflagfilter))
    {
        RedFlag_Filter(app_wdgts->gd_img, app_wdgts->gd_out, "cache/temp_img.png");
        app_wdgts->tmp_img = gdk_pixbuf_new_from_file("cache/temp_img.png", NULL);
    }

    if(gtk_toggle_button_get_active(app_wdgts->w_check_summertimefilter))
    {
        SummerTime_Filter(app_wdgts->gd_img, app_wdgts->gd_out, "cache/temp_img.png");
        app_wdgts->tmp_img = gdk_pixbuf_new_from_file("cache/temp_img.png", NULL);
    }



    gdk_pixbuf_savev(app_wdgts->tmp_img, "cache/cp_img.png", "png", NULL, NULL, NULL);
    update_buffer(app_wdgts);
    update_all_prev(app_wdgts);

    return 0;
}

int on_check_none_toggled(GtkToggleButton *togglebutton, app_widgets *app_wdgts)
{
    if(gtk_toggle_button_get_active(togglebutton))
    {
        gtk_toggle_button_set_active(app_wdgts->w_check_oldschool, FALSE);
        gtk_toggle_button_set_active(app_wdgts->w_check_glowfilter,FALSE);
        gtk_toggle_button_set_active(app_wdgts->w_check_redflagfilter, FALSE);
        gtk_toggle_button_set_active(app_wdgts->w_check_summertimefilter, FALSE);

        gtk_widget_set_sensitive(app_wdgts->w_btn_apply, TRUE);

        app_wdgts->tmp_img = gdk_pixbuf_new_from_file("cache/cp_img.png", NULL);
        gdk_pixbuf_savev(app_wdgts->tmp_img, "cache/temp_img.png", "png", NULL, NULL, NULL);
        update_buffer(app_wdgts);    
    }
}

int on_check_oldschool_toggled(GtkToggleButton *togglebutton, app_widgets *app_wdgts)
{
    if(gtk_toggle_button_get_active(togglebutton))
    {
        gtk_widget_set_sensitive(app_wdgts->w_scale_contrast, FALSE);
        gtk_widget_set_sensitive(app_wdgts->w_scale_temperature, FALSE);
        gtk_widget_set_sensitive(app_wdgts->w_scale_brightness, FALSE);
        gtk_widget_set_sensitive(app_wdgts->w_entry_contrast, FALSE);
        gtk_widget_set_sensitive(app_wdgts->w_entry_temperature, FALSE);
        gtk_widget_set_sensitive(app_wdgts->w_entry_brightness, FALSE);
        gtk_widget_set_sensitive(app_wdgts->w_scale_zoom, FALSE);
        gtk_toggle_button_set_active(app_wdgts->w_check_none, FALSE);
        gtk_widget_set_sensitive(app_wdgts->w_btn_apply, TRUE);

        app_wdgts->gd_img = gdImageCreateFromFile("cache/temp_img.png");
        OldSchool_Filter(app_wdgts->gd_img, app_wdgts->gd_out, "cache/temp_img.png");
        app_wdgts->tmp_img = gdk_pixbuf_new_from_file("cache/temp_img.png", NULL);
        update_buffer(app_wdgts);
        update_all_prev(app_wdgts);
    }

    else
    {

    }

    return 0;
}

int on_check_summertimefilter_toggled(GtkToggleButton *togglebutton, app_widgets *app_wdgts)
{
    if(gtk_toggle_button_get_active(togglebutton))
    {
        gtk_widget_set_sensitive(app_wdgts->w_scale_contrast, FALSE);
        gtk_widget_set_sensitive(app_wdgts->w_scale_temperature, FALSE);
        gtk_widget_set_sensitive(app_wdgts->w_scale_brightness, FALSE);
        gtk_widget_set_sensitive(app_wdgts->w_entry_contrast, FALSE);
        gtk_widget_set_sensitive(app_wdgts->w_entry_temperature, FALSE);
        gtk_widget_set_sensitive(app_wdgts->w_entry_brightness, FALSE);
        gtk_widget_set_sensitive(app_wdgts->w_scale_zoom, FALSE);
        gtk_toggle_button_set_active(app_wdgts->w_check_none, FALSE);
        gtk_widget_set_sensitive(app_wdgts->w_btn_apply, TRUE);

        app_wdgts->gd_img = gdImageCreateFromFile("cache/temp_img.png");
        SummerTime_Filter(app_wdgts->gd_img, app_wdgts->gd_out, "cache/temp_img.png");
        app_wdgts->tmp_img = gdk_pixbuf_new_from_file("cache/temp_img.png", NULL);
        update_buffer(app_wdgts);
        update_all_prev(app_wdgts);
    }

    else
    {

    }

    return 0;
}

int on_check_glowfilter_toggled(GtkToggleButton *togglebutton, app_widgets *app_wdgts)
{
    if(gtk_toggle_button_get_active(togglebutton))
    {
        gtk_widget_set_sensitive(app_wdgts->w_scale_contrast, FALSE);
        gtk_widget_set_sensitive(app_wdgts->w_scale_temperature, FALSE);
        gtk_widget_set_sensitive(app_wdgts->w_scale_brightness, FALSE);
        gtk_widget_set_sensitive(app_wdgts->w_entry_contrast, FALSE);
        gtk_widget_set_sensitive(app_wdgts->w_entry_temperature, FALSE);
        gtk_widget_set_sensitive(app_wdgts->w_entry_brightness, FALSE);
        gtk_widget_set_sensitive(app_wdgts->w_scale_zoom, FALSE);
        gtk_toggle_button_set_active(app_wdgts->w_check_none, FALSE);
        gtk_widget_set_sensitive(app_wdgts->w_btn_apply, TRUE);

        app_wdgts->gd_img = gdImageCreateFromFile("cache/temp_img.png");
        Glow_Filter(app_wdgts->gd_img, app_wdgts->gd_out,"cache/temp_img.png");
        app_wdgts->tmp_img = gdk_pixbuf_new_from_file("cache/temp_img.png", NULL);
        update_buffer(app_wdgts);
        update_all_prev(app_wdgts);
    }

    else
    {

    }
        
    return 0;
}

int on_check_redflagfilter_toggled(GtkToggleButton *togglebutton, app_widgets *app_wdgts)
{
    if(gtk_toggle_button_get_active(togglebutton))
    {
        gtk_widget_set_sensitive(app_wdgts->w_scale_contrast, FALSE);
        gtk_widget_set_sensitive(app_wdgts->w_scale_temperature, FALSE);
        gtk_widget_set_sensitive(app_wdgts->w_scale_brightness, FALSE);
        gtk_widget_set_sensitive(app_wdgts->w_entry_contrast, FALSE);
        gtk_widget_set_sensitive(app_wdgts->w_entry_temperature, FALSE);
        gtk_widget_set_sensitive(app_wdgts->w_entry_brightness, FALSE);
        gtk_widget_set_sensitive(app_wdgts->w_scale_zoom, FALSE);
        gtk_toggle_button_set_active(app_wdgts->w_check_none, FALSE);
        gtk_widget_set_sensitive(app_wdgts->w_btn_apply, TRUE);
        
        app_wdgts->gd_img = gdImageCreateFromFile("cache/temp_img.png");
        RedFlag_Filter(app_wdgts->gd_img, app_wdgts->gd_out, "cache/temp_img.png");
        app_wdgts->tmp_img = gdk_pixbuf_new_from_file("cache/temp_img.png", NULL);
        update_buffer(app_wdgts);
        update_all_prev(app_wdgts);
    }

    else
    {

    }
        
    return 0;
}
////
//

#pragma endregion ImageProcessing

#pragma region Utils

//// Functions
//
//
////

#pragma endregion Utils

