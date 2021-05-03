/* Project Information
 * Group: Orion
 * Project: Nebula
 * Authors: Tom TERMAAT, André HOANG, Albert EL HELOU, Adrien HICAUBERT
 * Description: Orion Nebula is a raster graphics editor.
 */

#pragma region Include

#define _GNU_SOURCE

#include <unistd.h>
#include <sys/types.h>
#include <signal.h>
#include <unistd.h>
#include <err.h>
#include <math.h>
#include <time.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>

#include <gtk/gtk.h>
#include <gdk/gdk.h>
#include <gdk-pixbuf/gdk-pixbuf.h>

#include <gd.h>

#include ".h/filters.h"
#include ".h/image_settings.h"
#include ".h/drawing.h"
#include ".h/func_divers.h"

#pragma endregion Include

#pragma region Structure

struct Point {
    int x;
    int y;
    struct Point *next;
};

struct Color {
    int r;
    int g;
    int b;
    int a;
};

struct gdImage_list {
    gdImagePtr img;
    struct gdImage_list *next;
};

typedef struct {

    // Pointer to GtkWidget
    GtkWidget *w_window;
    GtkWidget *w_dlg_file_choose;
    GtkWidget *w_img_main;
    GtkWidget *w_scale_brightness;
    GtkWidget *w_scale_contrast;
    GtkWidget *w_scale_temperature;
    GtkWidget *w_scale_noise;
    GtkWidget *w_scale_sharpen;
    GtkWidget *w_scale_zoom;
    GtkWidget *w_scale_rotation;
    GtkWidget *w_entry_brightness;
    GtkWidget *w_entry_contrast;
    GtkWidget *w_entry_temperature;
    GtkWidget *w_entry_noise;
    GtkWidget *w_entry_sharpen;
    GtkWidget *w_entryW_crop;
    GtkWidget *w_entryH_crop;
    GtkWidget *w_entry_rotation;
    GtkWidget *w_info_resolution;
    GtkWidget *w_info_zoom;
    GtkWidget *w_btn_apply;
    GtkWidget *w_btn_crop;
    GtkWidget *w_btn_resize;
    GtkWidget *w_drawing_aera;
    GtkWidget *w_prev_none;
    GtkWidget *w_prev_oldschool;
    GtkWidget *w_prev_glowfilter;
    GtkWidget *w_prev_redflagfilter;
    GtkWidget *w_prev_summertimefilter;
    GtkWidget *w_prev_vogue;
    GtkWidget *w_check_none;
    GtkWidget *w_check_oldschool;
    GtkWidget *w_check_glowfilter;
    GtkWidget *w_check_redflagfilter;
    GtkWidget *w_check_summertimefilter;
    GtkWidget *w_cb_brush;
    GtkWidget *w_cb_thickness;
    GtkWidget *w_btn_color;
    GtkWidget *w_btn_fill;
    GtkWidget *w_btn_text;
    GtkWidget *w_btn_erase;
    GtkWidget *w_btn_wipe;
    GtkWidget *w_check_vogue;

    int *color_array;
    int *draw_array;

    // Temp. image info
    GdkPixbuf *tmp_img;
    int tmp_w;
    int tmp_h;

    // Gd image info
    gdImagePtr gd_img;
    int gd_w;
    int gd_h;
    FILE *gd_out;

    // undo / redo
    struct gdImage_list *gd_list;

    // Drawing info
    int brush_type;
    int thickness;
    struct Color *color; 
    int cr_action; 

    // global variables
    char *resolution_info;
    int zoom;
    int focus_text;
    int focus_fill;
    int focus_draw;
    int focus_erase;
    int focus_wipe;

} app_widgets;

#pragma endregion Structure

#pragma region Main

struct Point *p1, *p2, *start;

int main(int argc, char *argv[])
{
    GtkBuilder      *builder;
    app_widgets     *widgets = g_slice_new(app_widgets);

    p1 = p2 = start = NULL;
    start = malloc(sizeof(struct Point));
    memset(start, 0, sizeof(struct Point));
    widgets->color = calloc(sizeof(struct Color), 0);
    widgets->gd_list = malloc(sizeof(struct gdImage_list));
    widgets->gd_list->img = NULL;
    widgets->gd_list->next = NULL;

    p1 = start;
    widgets->focus_draw = 0;
    widgets->focus_fill = 0;
    widgets->focus_erase = 0;
    widgets->focus_wipe = 0;

    widgets->cr_action = 0;
    widgets->thickness = 1;

    gtk_init(&argc, &argv);

    builder = gtk_builder_new_from_file("src/glade/window_main.glade");

    widgets->w_window = GTK_WIDGET(
            gtk_builder_get_object(builder, "window_main"));
    widgets->w_dlg_file_choose = GTK_WIDGET(
            gtk_builder_get_object(builder, "dlg_file_choose"));
    widgets->w_drawing_aera = GTK_WIDGET(
        gtk_builder_get_object(builder, "drw_area"));
    widgets->w_scale_brightness = GTK_WIDGET(
        gtk_builder_get_object(builder, "scale_brightness"));
    widgets->w_scale_contrast = GTK_WIDGET(
        gtk_builder_get_object(builder, "scale_contrast"));
    widgets->w_scale_temperature = GTK_WIDGET(
        gtk_builder_get_object(builder, "scale_temperature"));
    widgets->w_entry_brightness = GTK_WIDGET(
        gtk_builder_get_object(builder, "entry_brightness"));
    widgets->w_entry_contrast = GTK_WIDGET(
        gtk_builder_get_object(builder, "entry_contrast"));
    widgets->w_entry_temperature = GTK_WIDGET(
        gtk_builder_get_object(builder, "entry_temperature"));
    widgets->w_info_resolution = GTK_WIDGET(
        gtk_builder_get_object(builder, "info_resolution"));
    widgets->w_info_zoom = GTK_WIDGET(
        gtk_builder_get_object(builder, "info_zoom"));
    widgets->w_btn_apply = GTK_WIDGET(
        gtk_builder_get_object(builder, "btn_apply"));
    widgets->w_scale_zoom = GTK_WIDGET(
        gtk_builder_get_object(builder, "scale_zoom"));
    widgets->w_prev_none = GTK_WIDGET(
        gtk_builder_get_object(builder, "prev_none"));
    widgets->w_prev_oldschool = GTK_WIDGET(
        gtk_builder_get_object(builder, "prev_oldschool"));
    widgets->w_prev_glowfilter = GTK_WIDGET(
        gtk_builder_get_object(builder, "prev_glowfilter"));
    widgets->w_prev_redflagfilter = GTK_WIDGET(
        gtk_builder_get_object(builder, "prev_redflagfilter"));
    widgets->w_prev_summertimefilter = GTK_WIDGET(
        gtk_builder_get_object(builder, "prev_summertimefilter"));
    widgets->w_prev_vogue = GTK_WIDGET(
        gtk_builder_get_object(builder, "prev_vogue"));
    widgets->w_check_none = GTK_WIDGET(
        gtk_builder_get_object(builder, "check_none"));
    widgets->w_check_oldschool = GTK_WIDGET(
        gtk_builder_get_object(builder, "check_oldschool"));
    widgets->w_check_glowfilter = GTK_WIDGET(
        gtk_builder_get_object(builder, "check_glowfilter"));
    widgets->w_check_redflagfilter = GTK_WIDGET(
        gtk_builder_get_object(builder, "check_redflagfilter"));
    widgets->w_check_vogue = GTK_WIDGET(
        gtk_builder_get_object(builder, "check_vogue"));
    widgets->w_check_summertimefilter = GTK_WIDGET(
        gtk_builder_get_object(builder, "check_summertimefilter"));
    widgets->w_btn_crop = GTK_WIDGET(
        gtk_builder_get_object(builder, "btn_crop"));
    widgets->w_entryW_crop = GTK_WIDGET(
        gtk_builder_get_object(builder, "entryW_crop"));
    widgets->w_entryH_crop = GTK_WIDGET(
        gtk_builder_get_object(builder, "entryH_crop"));
    widgets->w_btn_resize = GTK_WIDGET(
        gtk_builder_get_object(builder, "btn_resize"));
    widgets->w_entry_rotation = GTK_WIDGET(
        gtk_builder_get_object(builder, "entry_rotation"));
    widgets->w_scale_rotation = GTK_WIDGET(
        gtk_builder_get_object(builder, "scale_rotation"));
    widgets->w_cb_brush = GTK_WIDGET(
        gtk_builder_get_object(builder, "cb_brush"));
    widgets->w_cb_thickness = GTK_WIDGET(
        gtk_builder_get_object(builder, "cb_thickness"));
    widgets->w_btn_color = GTK_WIDGET(
        gtk_builder_get_object(builder, "btn_color"));
    widgets->w_btn_text = GTK_WIDGET(
        gtk_builder_get_object(builder, "btn_text"));
    widgets->w_btn_fill = GTK_WIDGET(
        gtk_builder_get_object(builder, "btn_fill"));
    widgets->w_scale_noise = GTK_WIDGET(
        gtk_builder_get_object(builder, "scale_noise"));
    widgets->w_scale_sharpen = GTK_WIDGET(
        gtk_builder_get_object(builder, "scale_sharpen"));
    widgets->w_entry_noise = GTK_WIDGET(
        gtk_builder_get_object(builder, "entry_noise"));
    widgets->w_entry_sharpen = GTK_WIDGET(
        gtk_builder_get_object(builder, "entry_sharpen"));
    widgets->w_btn_erase = GTK_WIDGET(
        gtk_builder_get_object(builder, "btn_erase"));
    widgets->w_btn_wipe = GTK_WIDGET(
        gtk_builder_get_object(builder, "btn_wipe"));

    gtk_builder_connect_signals(builder, widgets);
    g_object_unref(builder);

    gtk_widget_set_events(widgets->w_drawing_aera, GDK_BUTTON_MOTION_MASK |
        GDK_BUTTON_PRESS_MASK | GDK_BUTTON_RELEASE_MASK);

    //Set default states & values
    gtk_widget_set_sensitive(widgets->w_scale_brightness, FALSE);
    gtk_widget_set_sensitive(widgets->w_scale_contrast, FALSE);
    gtk_widget_set_sensitive(widgets->w_scale_temperature, FALSE);
    gtk_widget_set_sensitive(widgets->w_entry_brightness, FALSE);
    gtk_widget_set_sensitive(widgets->w_entry_contrast, FALSE);
    gtk_widget_set_sensitive(widgets->w_entry_temperature, FALSE);
    gtk_widget_set_sensitive(widgets->w_entry_noise, FALSE);
    gtk_widget_set_sensitive(widgets->w_entry_sharpen, FALSE);
    gtk_widget_set_sensitive(widgets->w_btn_apply, FALSE);
    gtk_widget_set_sensitive(widgets->w_scale_zoom, FALSE);
    gtk_widget_set_sensitive(widgets->w_check_none, FALSE);
    gtk_widget_set_sensitive(widgets->w_check_oldschool, FALSE);
    gtk_widget_set_sensitive(widgets->w_check_glowfilter,FALSE);
    gtk_widget_set_sensitive(widgets->w_check_redflagfilter, FALSE);
    gtk_widget_set_sensitive(widgets->w_check_summertimefilter, FALSE);
    gtk_widget_set_sensitive(widgets->w_btn_crop, FALSE);
    gtk_widget_set_sensitive(widgets->w_btn_resize, FALSE);
    gtk_widget_set_sensitive(widgets->w_btn_erase, FALSE);
    gtk_widget_set_sensitive(widgets->w_entryW_crop, FALSE);
    gtk_widget_set_sensitive(widgets->w_entryH_crop, FALSE);
    gtk_widget_set_sensitive(widgets->w_entry_rotation, FALSE);
    gtk_widget_set_sensitive(widgets->w_scale_rotation, FALSE);
    gtk_widget_set_sensitive(widgets->w_cb_brush, FALSE);
    gtk_widget_set_sensitive(widgets->w_cb_thickness, FALSE);
    gtk_widget_set_sensitive(widgets->w_btn_color, FALSE);
    gtk_widget_set_sensitive(widgets->w_btn_fill, FALSE);
    gtk_widget_set_sensitive(widgets->w_btn_text, FALSE);
    gtk_widget_set_sensitive(widgets->w_scale_noise, FALSE);
    gtk_widget_set_sensitive(widgets->w_scale_sharpen, FALSE);
    gtk_widget_set_sensitive(widgets->w_btn_wipe, FALSE);


    gtk_widget_show(widgets->w_window);
    gtk_main();
    gdFree(widgets->gd_img);
    free(widgets->color);
    struct gdImage_list *tmp;
    while(widgets->gd_list != NULL)
    {
        tmp = widgets->gd_list;
        widgets->gd_list = widgets->gd_list->next;
        free(tmp);
    }
    g_slice_free(app_widgets, widgets);
    return 0;
}

#pragma endregion Main

#pragma region Utils

//// Functions
//
int count_chained_list(struct Point *head){
    int c = 0;

    while (head != NULL)
    {
        head = head->next;
        c++;
    }

    return c;
}

int free_point_struct(struct Point *head){

    struct Point* tmp;
    int c = 0;

    while (head != NULL)
    {
        tmp = head;
        head = head->next;
        free(tmp);
        c++;
    }

    if(c > 0)
    {
        start->next = NULL;
        p1 = start;
    }

    return c;
}
//
////

#pragma endregion Utils

#pragma region Interface

//// Functions
int print_list(struct gdImage_list *list)
{
    struct gdImage_list *tmp = list;
    int i = 0;
    while(tmp->next != NULL)
    {
        i++;
        printf("%d->", i);
        tmp = tmp->next;
    }
    printf("NULL\n");
    return 0;
}

int update_buffer(app_widgets *app_wdgts)
{
    app_wdgts->tmp_img = gdk_pixbuf_scale_simple(app_wdgts->tmp_img,
        app_wdgts->tmp_w, app_wdgts->tmp_h, GDK_INTERP_NEAREST);
    gtk_widget_set_size_request(app_wdgts->w_drawing_aera,
        app_wdgts->tmp_w, app_wdgts->tmp_h);
    app_wdgts->cr_action = 0;
    gtk_widget_queue_draw(app_wdgts->w_drawing_aera);
    asprintf(&app_wdgts->resolution_info, "%d x %d",
        app_wdgts->tmp_w, app_wdgts->tmp_h);
    gtk_label_set_text(GTK_LABEL(app_wdgts->w_info_resolution),
        app_wdgts->resolution_info);
    return 0;
}

int update_info_zoom(app_widgets *app_wdgts)
{
    int scale_value = gtk_range_get_value(GTK_RANGE(app_wdgts->w_scale_zoom));

    float zoom_percentage = ((float)scale_value/20)*100;

    char *zoom_info;
    asprintf(&zoom_info, "%.0f %%", zoom_percentage);
    gtk_label_set_text(GTK_LABEL(app_wdgts->w_info_zoom), zoom_info);
    return 0;
}

int update_all_prev(app_widgets *app_wdgts)
{
    GdkPixbuf *prev_none = gdk_pixbuf_new_from_file("cache/cp_img.png", NULL);
    prev_none = gdk_pixbuf_scale_simple(prev_none, app_wdgts->gd_w/5,
        app_wdgts->gd_h/5, GDK_INTERP_NEAREST);
    gtk_image_set_from_pixbuf(GTK_IMAGE(app_wdgts->w_prev_none), prev_none);

    gdImagePtr gd_prev = gdImageCreateFromFile("cache/cp_img.png");
    OldSchool_Filter(gd_prev, app_wdgts->gd_out, "cache/prev_img.png");
    GdkPixbuf *prev_oldschool =
        gdk_pixbuf_new_from_file("cache/prev_img.png", NULL);
    prev_oldschool = gdk_pixbuf_scale_simple(prev_oldschool, 
        app_wdgts->gd_w/5, app_wdgts->gd_h/5, GDK_INTERP_NEAREST);
    gtk_image_set_from_pixbuf(
        GTK_IMAGE(app_wdgts->w_prev_oldschool), prev_oldschool);

    gd_prev = gdImageCreateFromFile("cache/cp_img.png");
    Glow_Filter(gd_prev, app_wdgts->gd_out, "cache/prev_img.png");
    GdkPixbuf *prev_glowfilter =
        gdk_pixbuf_new_from_file("cache/prev_img.png", NULL);
    prev_glowfilter = gdk_pixbuf_scale_simple(prev_glowfilter,
        app_wdgts->gd_w/5, app_wdgts->gd_h/5, GDK_INTERP_NEAREST);
    gtk_image_set_from_pixbuf(GTK_IMAGE(app_wdgts->w_prev_glowfilter),
        prev_glowfilter);

    gd_prev = gdImageCreateFromFile("cache/cp_img.png");
    RedFlag_Filter(gd_prev, app_wdgts->gd_out, "cache/prev_img.png");
    GdkPixbuf *prev_redflagfilter =
        gdk_pixbuf_new_from_file("cache/prev_img.png", NULL);
    prev_redflagfilter = gdk_pixbuf_scale_simple(prev_redflagfilter,
        app_wdgts->gd_w/5, app_wdgts->gd_h/5, GDK_INTERP_NEAREST);
    gtk_image_set_from_pixbuf(GTK_IMAGE(app_wdgts->w_prev_redflagfilter),
        prev_redflagfilter);

    gd_prev = gdImageCreateFromFile("cache/cp_img.png");
    SummerTime_Filter(gd_prev, app_wdgts->gd_out, "cache/prev_img.png");
    GdkPixbuf *prev_summertimefilter =
        gdk_pixbuf_new_from_file("cache/prev_img.png", NULL);
    prev_summertimefilter = gdk_pixbuf_scale_simple(prev_summertimefilter,
        app_wdgts->gd_w/5, app_wdgts->gd_h/5, GDK_INTERP_NEAREST);
    gtk_image_set_from_pixbuf(GTK_IMAGE(app_wdgts->w_prev_summertimefilter),
        prev_summertimefilter);

    gd_prev = gdImageCreateFromFile("cache/cp_img.png");
    Vogue_Filter(gd_prev, app_wdgts->gd_out, "cache/prev_img.png");
    GdkPixbuf *prev_vogue =
        gdk_pixbuf_new_from_file("cache/prev_img.png", NULL);
    prev_vogue = gdk_pixbuf_scale_simple(prev_vogue,
        app_wdgts->gd_w/5, app_wdgts->gd_h/5, GDK_INTERP_NEAREST);
    gtk_image_set_from_pixbuf(GTK_IMAGE(app_wdgts->w_prev_vogue),
        prev_vogue);

    return 0;
}

int insert_gdImage_list(app_widgets *app_wdgts)
{
    struct gdImage_list *new_gd = malloc(sizeof(struct gdImage_list));
    new_gd->img = gdImageCreateFromFile("cache/cp_img.png");
    new_gd->next = app_wdgts->gd_list;

    app_wdgts->gd_list = new_gd;

    //print_list(app_wdgts->gd_list);

    return 0;
}

gdImagePtr pop_gdImage_list(app_widgets *app_wdgts)
{
    struct gdImage_list *tmp = app_wdgts->gd_list;
    app_wdgts->gd_list = app_wdgts->gd_list->next;
    gdImagePtr img = app_wdgts->gd_list->img;
    free(tmp);
    return img;
}

int init_color_array(app_widgets *app_wdgts)
{
    int width = app_wdgts->gd_w;
    int height = app_wdgts->gd_h;

    app_wdgts->color_array = malloc(width*height * sizeof(int));
    app_wdgts->draw_array = malloc(width*height * sizeof(int));

    for(int x = 0; x < height; x++)
    {
        for(int y = 0; y < width; y++)
        {
            app_wdgts->draw_array[x * width + y] = 
                            gdImageGetPixel(app_wdgts->gd_img, x, y);
            app_wdgts->draw_array[x * width + y] = 0;
        }
    }

    return 0;
}

int print_array(app_widgets *app_wdgts,int *array)
{
    int width = app_wdgts->gd_w;
    int height = app_wdgts->gd_h;

    printf("ARRAY = \n");

    for(int x = 0; x < height; x++)
    {
        printf("[");
        for(int y = 0; y < width; y++)
        {
            printf(" %d ", array[x * width + y]);
        }
        printf("]\n");
    }

    printf("\n\n");

    return 0;
}
//

//// SignalCallbackFunctions
int on_menubar_open_activate(GtkMenuItem *menuitem, app_widgets *app_wdgts)
{
    printf("%ld\n", sizeof(menuitem));
    // Show the "Open Image" dialog box
    gtk_widget_show(app_wdgts->w_dlg_file_choose);

    // Check return value from Open Image dialog box to 
    // see if user clicked the Open button
    if (gtk_dialog_run(
            GTK_DIALOG (app_wdgts->w_dlg_file_choose)) == GTK_RESPONSE_OK) {

        // Get the file name from the dialog box
        char *file_name = gtk_file_chooser_get_filename(
            GTK_FILE_CHOOSER(app_wdgts->w_dlg_file_choose));
        if (file_name != NULL) {

            // Create the pixbuf image to display
            app_wdgts->tmp_img = gdk_pixbuf_new_from_file(file_name, NULL);

            // Save copies of the original files
            gdk_pixbuf_savev(
             app_wdgts->tmp_img, "cache/cp_img.png", "png", NULL, NULL, NULL);
            gdk_pixbuf_savev(
             app_wdgts->tmp_img, "cache/temp_img.png", "png", NULL, NULL, NULL);

            app_wdgts->gd_img = gdImageCreateFromFile("cache/cp_img.png");

            // Set sensitive to TRUE for all widgets
            gtk_widget_set_sensitive(app_wdgts->w_scale_brightness, TRUE);
            gtk_widget_set_sensitive(app_wdgts->w_scale_contrast, TRUE);
            gtk_widget_set_sensitive(app_wdgts->w_scale_temperature, TRUE);
            gtk_widget_set_sensitive(app_wdgts->w_scale_noise, TRUE);
            gtk_widget_set_sensitive(app_wdgts->w_scale_sharpen, TRUE);
            gtk_widget_set_sensitive(app_wdgts->w_entry_brightness, TRUE);
            gtk_widget_set_sensitive(app_wdgts->w_entry_contrast, TRUE);
            gtk_widget_set_sensitive(app_wdgts->w_entry_noise, TRUE);
            gtk_widget_set_sensitive(app_wdgts->w_entry_sharpen, TRUE);           
            gtk_widget_set_sensitive(app_wdgts->w_entry_temperature, TRUE);
            gtk_widget_set_sensitive(app_wdgts->w_scale_zoom, TRUE);
            gtk_widget_set_sensitive(app_wdgts->w_check_none, TRUE);
            gtk_widget_set_sensitive(app_wdgts->w_check_oldschool, TRUE);
            gtk_widget_set_sensitive(app_wdgts->w_check_glowfilter,TRUE);
            gtk_widget_set_sensitive(app_wdgts->w_check_redflagfilter, TRUE);
            gtk_widget_set_sensitive(app_wdgts->w_check_summertimefilter, TRUE);
            gtk_widget_set_sensitive(app_wdgts->w_btn_crop, TRUE);
            gtk_widget_set_sensitive(app_wdgts->w_btn_resize, TRUE);
            gtk_widget_set_sensitive(app_wdgts->w_entryH_crop, TRUE);
            gtk_widget_set_sensitive(app_wdgts->w_entryW_crop, TRUE);
            gtk_widget_set_sensitive(app_wdgts->w_entry_rotation, TRUE);
            gtk_widget_set_sensitive(app_wdgts->w_scale_rotation, TRUE);
            gtk_widget_set_sensitive(app_wdgts->w_cb_brush, TRUE);
            gtk_widget_set_sensitive(app_wdgts->w_cb_thickness, TRUE);
            gtk_widget_set_sensitive(app_wdgts->w_btn_color, TRUE);
            gtk_widget_set_sensitive(app_wdgts->w_btn_fill, TRUE);
            gtk_widget_set_sensitive(app_wdgts->w_btn_text, TRUE);
            gtk_widget_set_sensitive(app_wdgts->w_btn_erase, TRUE);
            gtk_widget_set_sensitive(app_wdgts->w_btn_wipe, TRUE);

            // gets info of the image
            int img_w = gdImageSX(app_wdgts->gd_img);
            int img_h = gdImageSY(app_wdgts->gd_img);

            gtk_spin_button_set_value(
                GTK_SPIN_BUTTON(app_wdgts->w_entryW_crop), img_w);
            gtk_spin_button_set_value(
                GTK_SPIN_BUTTON(app_wdgts->w_entryH_crop), img_h);

            app_wdgts->tmp_w = img_w;
            app_wdgts->tmp_h = img_h;
            app_wdgts->gd_w = img_w;
            app_wdgts->gd_h = img_h;

            init_color_array(app_wdgts);

            // Redraw the drawing area to display the image in the UI
            app_wdgts->cr_action = 0;
            gtk_widget_queue_draw(app_wdgts->w_drawing_aera);
            update_info_zoom(app_wdgts);

            // Display the resolution in the UI
            asprintf(&app_wdgts->resolution_info, "%d x %d",
                app_wdgts->tmp_w, app_wdgts->tmp_h);
            gtk_label_set_text(GTK_LABEL(app_wdgts->w_info_resolution),
                app_wdgts->resolution_info);

            // empty the undo list
            struct gdImage_list *tmp;

            while(app_wdgts->gd_list != NULL)
            {
                tmp = app_wdgts->gd_list;
                app_wdgts->gd_list = app_wdgts->gd_list->next;
                free(tmp);
            }      

            app_wdgts->gd_list = malloc(sizeof(struct gdImage_list));
            app_wdgts->gd_list->img = NULL;      

            // Update preview
            gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(
                app_wdgts->w_check_none), TRUE);
            update_all_prev(app_wdgts);

            insert_gdImage_list(app_wdgts);
        }
        g_free(file_name);
    }

    // Finished with the "Open Image" dialog box, so hide it
    gtk_widget_hide(app_wdgts->w_dlg_file_choose);

    return 0;
}

void on_menubar_quit_activate(GtkMenuItem *menuitem, app_widgets *app_wdgts)
{
    printf("%ld\n", sizeof(menuitem));
    printf("%ld\n", sizeof(app_wdgts));
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
    printf("%ld\n", sizeof(menuitem));
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

    app_wdgts->tmp_img = gdk_pixbuf_scale_simple(cp_img,
        app_wdgts->tmp_w, app_wdgts->tmp_h, GDK_INTERP_NEAREST);
    app_wdgts->cr_action = 0;
    gtk_widget_queue_draw(app_wdgts->w_drawing_aera);
    update_info_zoom(app_wdgts);

    return 0;
}

int on_btn_undo_activate(GtkMenuItem *menuitem, app_widgets *app_wdgts)
{
    printf("%ld\n", sizeof(menuitem));

    if(app_wdgts->gd_list->next->img != NULL)
    {
        gdImagePtr prev_gd = pop_gdImage_list(app_wdgts);

        app_wdgts->gd_img = prev_gd;
        app_wdgts->gd_out = fopen("cache/cp_img.png", "wb");
        gdImagePng(app_wdgts->gd_img, app_wdgts->gd_out);
        fclose(app_wdgts->gd_out);
        app_wdgts->tmp_img = gdk_pixbuf_new_from_file("cache/cp_img.png", NULL);
        app_wdgts->gd_img = gdImageCreateFromFile("cache/cp_img.png");
        gdk_pixbuf_savev(
            app_wdgts->tmp_img, "cache/temp_img.png", "png", NULL, NULL, NULL);
        update_buffer(app_wdgts);
        update_all_prev(app_wdgts);
        //print_list(app_wdgts->gd_list);
    }

    return 0;
}

int on_btn_redo_activate(GtkMenuItem *menuitem, app_widgets *app_wdgts)
{
    printf("%ld\n", sizeof(menuitem));
    printf("%ld\n", sizeof(app_wdgts));
    return 0;
}
//

#pragma endregion Interface

#pragma region Drawing

//// Functions
static void draw_brush(GtkWidget *widget, gdouble x, gdouble y,
                        app_widgets *app_wdgts)
{
    printf("%ld\n", sizeof(widget));
    
    if(app_wdgts->focus_wipe || app_wdgts->focus_erase)
    {
        struct Point *new_p = malloc(sizeof(struct Point));
        if(new_p == NULL) { printf("out of memory\n"); abort(); }
        new_p->x = x;
        new_p->y = y;
        new_p->next = NULL;
        p1->next = new_p;
        p1 = p1->next;
    }
    else if(app_wdgts->focus_draw){
        struct Point *new_p = malloc(sizeof(struct Point));
        if(new_p == NULL) { printf("out of memory\n"); abort(); }
        new_p->x = x;
        new_p->y = y;
        new_p->next = NULL;
        p1->next = new_p;
        p1 = p1->next;
        app_wdgts->draw_array[(int)x * app_wdgts->gd_w + (int)y] = 1;
    }
}

//// SignalsCallbackFunctions
int on_draw_event(GtkWidget *widget, cairo_t *cr, app_widgets *app_wdgts)
{   
    printf("%ld\n", sizeof(widget));
    if(app_wdgts->tmp_img != NULL)
    {
        gdk_cairo_set_source_pixbuf(cr, app_wdgts->tmp_img, 0, 0);
        gtk_widget_set_size_request(
            app_wdgts->w_drawing_aera, app_wdgts->tmp_w, app_wdgts->tmp_h);
        cairo_paint(cr);

        if(app_wdgts->cr_action)
        {
            gtk_widget_set_sensitive(app_wdgts->w_scale_brightness, FALSE);
            gtk_widget_set_sensitive(app_wdgts->w_scale_contrast, FALSE);
            gtk_widget_set_sensitive(app_wdgts->w_scale_temperature, FALSE);
            gtk_widget_set_sensitive(app_wdgts->w_scale_noise, FALSE);
            gtk_widget_set_sensitive(app_wdgts->w_scale_sharpen, FALSE);
            gtk_widget_set_sensitive(app_wdgts->w_entry_brightness, FALSE);
            gtk_widget_set_sensitive(app_wdgts->w_entry_contrast, FALSE);
            gtk_widget_set_sensitive(app_wdgts->w_entry_temperature, FALSE);
            gtk_widget_set_sensitive(app_wdgts->w_entry_noise, FALSE);
            gtk_widget_set_sensitive(app_wdgts->w_entry_sharpen, FALSE);
            gtk_widget_set_sensitive(app_wdgts->w_scale_zoom, FALSE);
            gtk_widget_set_sensitive(app_wdgts->w_check_none, FALSE);
            gtk_widget_set_sensitive(app_wdgts->w_check_oldschool, FALSE);
            gtk_widget_set_sensitive(app_wdgts->w_check_glowfilter,FALSE);
            gtk_widget_set_sensitive(app_wdgts->w_check_redflagfilter, FALSE);
            gtk_widget_set_sensitive(app_wdgts->w_check_summertimefilter,FALSE);
            gtk_widget_set_sensitive(app_wdgts->w_btn_crop, FALSE);
            gtk_widget_set_sensitive(app_wdgts->w_btn_resize, FALSE);
            gtk_widget_set_sensitive(app_wdgts->w_entry_rotation, FALSE);
            gtk_widget_set_sensitive(app_wdgts->w_scale_rotation, FALSE);
            gtk_widget_set_sensitive(app_wdgts->w_cb_brush, FALSE);
            gtk_widget_set_sensitive(app_wdgts->w_cb_thickness, FALSE);
            gtk_widget_set_sensitive(app_wdgts->w_btn_color, FALSE);

            if(!app_wdgts->focus_fill)
                gtk_widget_set_sensitive(app_wdgts->w_btn_fill, FALSE);

            if(!app_wdgts->focus_text)
                gtk_widget_set_sensitive(app_wdgts->w_btn_text, FALSE);

            gtk_widget_set_sensitive(app_wdgts->w_btn_apply, TRUE);

            if(start->next != NULL)
            {
                if(app_wdgts->focus_erase)
                {
                    struct Color *info_src = malloc(sizeof(struct Color));
                    info_src->r = 255;
                    info_src->g = 255;
                    info_src->b = 255;
                    erase(app_wdgts->gd_img, app_wdgts->gd_out, (void*)start->next,
                    (void*)info_src, app_wdgts->thickness,
                    app_wdgts->zoom, "cache/temp_img.png");
                }
                else if(app_wdgts->focus_wipe)
                {
                    wipe(app_wdgts->gd_img, app_wdgts->gd_out, (void*)start->next,
                    app_wdgts->color_array, app_wdgts->draw_array, app_wdgts->gd_w,
                    app_wdgts->zoom, "cache/temp_img.png");
                }
                else{
                    line_to(app_wdgts->gd_img, app_wdgts->gd_out,
                        (void *)start->next, (void *)app_wdgts->color,
                            app_wdgts->thickness,
                            app_wdgts->brush_type, 1, "cache/temp_img.png");
                }
                free_point_struct(start->next);
            }

            app_wdgts->tmp_img = gdk_pixbuf_new_from_file("cache/temp_img.png", NULL);
            update_buffer(app_wdgts);
        }

        app_wdgts->cr_action = 0;
    }

    return 0;
}

void on_clear_clicked(GtkWidget *b1, app_widgets *app_wdgts)
{
    printf("%ld\n", sizeof(b1));
    printf("%ld\n", sizeof(app_wdgts));
}

int on_draw_motion_notify_event(GtkWidget *widget, GdkEventMotion *event,
                                    app_widgets *app_wdgts)
{
    if(event->state & GDK_BUTTON1_MASK){
        draw_brush(widget, event->x, event->y, app_wdgts);
    }

    return 0;
}

int on_draw_button_release_event(GtkWidget *widget, GdkEventButton *event,
                                   app_widgets *app_wdgts){

    printf("%ld\n", sizeof(widget));
    int modif = 0;

    if(app_wdgts->focus_erase){
        modif = 1;
    }

    else if(app_wdgts->focus_wipe){
        modif = 1;
    }

    else if(app_wdgts->focus_draw){
        set_pixel(app_wdgts->gd_img, app_wdgts->gd_out, event->x, event->y,
            (void*)app_wdgts->color,
                app_wdgts->thickness,
                    app_wdgts->brush_type, 1, "cache/temp_img.png");
        modif = 1;
    }

    else if(app_wdgts->focus_fill){
        int truepixel = gdImageGetTrueColorPixel(
            app_wdgts->gd_img, event->x, event->y);
        int r = gdImageRed(app_wdgts->gd_img,truepixel);
        int b = gdImageBlue(app_wdgts->gd_img,truepixel);
        int g = gdImageGreen(app_wdgts->gd_img,truepixel);
        struct Color *info_src = malloc(sizeof(struct Color));
        info_src->r = r;
        info_src->g = g;
        info_src->b = b;
        info_src->a = 0;
        fill(app_wdgts->gd_img, app_wdgts->gd_out, (void*)info_src, (void*)app_wdgts->color,
                "cache/temp_img.png");
        free(info_src);
        modif = 1;
    }

    else if(app_wdgts->focus_text){
        char *font = "/usr/share/fonts/truetype/ubuntu/Ubuntu-LI.ttf";
        int size = 12;
        Add_text(app_wdgts->gd_img, app_wdgts->gd_out, "cache/temp_img.png",
            font, event->x, event->y + size , 200, 200, 0, size, 0, "Test");
        modif = 1;
    }

    if(modif){
        app_wdgts->cr_action = 1;
        gtk_widget_queue_draw(app_wdgts->w_drawing_aera);
    }

    return 0;
}

int on_btn_color_color_set(GtkWidget *color_button, app_widgets *app_wdgts)
{
    GdkRGBA *color_set = malloc(sizeof(GdkRGBA));
    gtk_color_chooser_get_rgba(GTK_COLOR_CHOOSER(color_button), color_set);
    app_wdgts->color->r = color_set->red * 255;
    app_wdgts->color->g = color_set->green * 255;
    app_wdgts->color->b = color_set->blue * 255;
    app_wdgts->color->a = (color_set->alpha * 127) * (-1);
    return 0;
}

int on_cb_brush_changed(GtkWidget *cb_brush, app_widgets *app_wdgts)
{
    int brush_index = gtk_combo_box_get_active(GTK_COMBO_BOX(cb_brush));

    if(brush_index == 0)
        app_wdgts->focus_draw = 0;
    else{
        app_wdgts->focus_draw = 1;
        app_wdgts->brush_type = brush_index;
    }

    return 0;
}

int on_cb_thickness_changed(GtkWidget *cb_thickness, app_widgets *app_wdgts)
{
    int thickness_index = gtk_combo_box_get_active(GTK_COMBO_BOX(cb_thickness));

    switch(thickness_index){
        case 0: app_wdgts->thickness = 1; break;
        case 1: app_wdgts->thickness = 3; break;
        case 2: app_wdgts->thickness = 5; break;
        case 3: app_wdgts->thickness = 7; break;
        default: errx(EXIT_FAILURE, "Couldn't set thickness.");
    }

    return 0;
}

int on_btn_fill_focus_in_event(GtkWidget *btn_fill, GdkEventMotion *event,
                                app_widgets *app_wdgts)
{
    printf("%ld\n", sizeof(btn_fill));
    printf("%ld\n", sizeof(event));
    printf("%ld\n", sizeof(app_wdgts));
    app_wdgts->focus_fill = 1;
    return 0;
}

int on_btn_fill_focus_out_event(GtkWidget *btn_fill, GdkEventMotion *event,
                                    app_widgets *app_wdgts)
{
    printf("%ld\n", sizeof(btn_fill));
    printf("%ld\n", sizeof(event));
    printf("%ld\n", sizeof(app_wdgts));
    app_wdgts->focus_fill = 0;
    return 0;
}

int on_btn_erase_focus_in_event(GtkWidget *btn_fill, GdkEventMotion *event,
                                app_widgets *app_wdgts)
{
    printf("%ld\n", sizeof(btn_fill));
    printf("%ld\n", sizeof(event));
    printf("%ld\n", sizeof(app_wdgts));
    app_wdgts->focus_erase = 1;
    return 0;
}

int on_btn_erase_focus_out_event(GtkWidget *btn_fill, GdkEventMotion *event,
                                    app_widgets *app_wdgts)
{
    printf("%ld\n", sizeof(btn_fill));
    printf("%ld\n", sizeof(event));
    printf("%ld\n", sizeof(app_wdgts));
    app_wdgts->focus_erase = 0;
    return 0;
}

int on_btn_wipe_focus_in_event(GtkWidget *btn_fill, GdkEventMotion *event,
                                app_widgets *app_wdgts)
{
    printf("%ld\n", sizeof(btn_fill));
    printf("%ld\n", sizeof(event));
    printf("%ld\n", sizeof(app_wdgts));
    app_wdgts->focus_wipe = 1;
    return 0;
}

int on_btn_wipe_focus_out_event(GtkWidget *btn_fill, GdkEventMotion *event,
                                    app_widgets *app_wdgts)
{
    printf("%ld\n", sizeof(btn_fill));
    printf("%ld\n", sizeof(event));
    printf("%ld\n", sizeof(app_wdgts));
    app_wdgts->focus_wipe = 0;
    return 0;
}
//

#pragma endregion Drawing

#pragma region ImageProcessing

//// Functions
//
//
////

//// SignalsCallbackFunctions
//
int on_scale_brightness_value_changed(GtkMenuItem *menuitem,
                                        app_widgets *app_wdgts)
{
    printf("%ld\n", sizeof(menuitem));
    gtk_widget_set_sensitive(app_wdgts->w_scale_contrast, FALSE);
    gtk_widget_set_sensitive(app_wdgts->w_scale_temperature, FALSE);
    gtk_widget_set_sensitive(app_wdgts->w_scale_noise, FALSE);
    gtk_widget_set_sensitive(app_wdgts->w_scale_sharpen, FALSE);
    gtk_widget_set_sensitive(app_wdgts->w_entry_noise, FALSE);
    gtk_widget_set_sensitive(app_wdgts->w_entry_sharpen, FALSE);
    gtk_widget_set_sensitive(app_wdgts->w_entry_contrast, FALSE);
    gtk_widget_set_sensitive(app_wdgts->w_entry_temperature, FALSE);
    gtk_widget_set_sensitive(app_wdgts->w_scale_zoom, FALSE);
    gtk_widget_set_sensitive(app_wdgts->w_check_none, FALSE);
    gtk_widget_set_sensitive(app_wdgts->w_check_oldschool, FALSE);
    gtk_widget_set_sensitive(app_wdgts->w_check_glowfilter,FALSE);
    gtk_widget_set_sensitive(app_wdgts->w_check_redflagfilter, FALSE);
    gtk_widget_set_sensitive(app_wdgts->w_check_summertimefilter, FALSE);
    gtk_widget_set_sensitive(app_wdgts->w_btn_crop, FALSE);
    gtk_widget_set_sensitive(app_wdgts->w_btn_resize, FALSE);
    gtk_widget_set_sensitive(app_wdgts->w_entry_rotation, FALSE);
    gtk_widget_set_sensitive(app_wdgts->w_scale_rotation, FALSE);
    gtk_widget_set_sensitive(app_wdgts->w_cb_brush, FALSE);
    gtk_widget_set_sensitive(app_wdgts->w_cb_thickness, FALSE);
    gtk_widget_set_sensitive(app_wdgts->w_btn_color, FALSE);
    gtk_widget_set_sensitive(app_wdgts->w_btn_fill, FALSE);
    gtk_widget_set_sensitive(app_wdgts->w_btn_text, FALSE);
    gtk_widget_set_sensitive(app_wdgts->w_btn_apply, TRUE);

    app_wdgts->gd_img = gdImageCreateFromFile("cache/cp_img.png");
    Brightness(app_wdgts->gd_img, gtk_range_get_value(
        GTK_RANGE(app_wdgts->w_scale_brightness)), app_wdgts->gd_out);
    app_wdgts->tmp_img = gdk_pixbuf_new_from_file("cache/temp_img.png", NULL);
    update_buffer(app_wdgts);

    return 0;
}

int on_scale_contrast_value_changed(GtkMenuItem *menuitem,
                                        app_widgets *app_wdgts)
{
    printf("%ld\n", sizeof(menuitem));
    gtk_widget_set_sensitive(app_wdgts->w_scale_noise, FALSE);
    gtk_widget_set_sensitive(app_wdgts->w_scale_sharpen, FALSE);
    gtk_widget_set_sensitive(app_wdgts->w_entry_noise, FALSE);
    gtk_widget_set_sensitive(app_wdgts->w_entry_sharpen, FALSE);
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
    gtk_widget_set_sensitive(app_wdgts->w_btn_crop, FALSE);
    gtk_widget_set_sensitive(app_wdgts->w_btn_resize, FALSE);
    gtk_widget_set_sensitive(app_wdgts->w_entry_rotation, FALSE);
    gtk_widget_set_sensitive(app_wdgts->w_scale_rotation, FALSE);
    gtk_widget_set_sensitive(app_wdgts->w_cb_brush, FALSE);
    gtk_widget_set_sensitive(app_wdgts->w_cb_thickness, FALSE);
    gtk_widget_set_sensitive(app_wdgts->w_btn_color, FALSE);
    gtk_widget_set_sensitive(app_wdgts->w_btn_fill, FALSE);
    gtk_widget_set_sensitive(app_wdgts->w_btn_text, FALSE);
    gtk_widget_set_sensitive(app_wdgts->w_btn_apply, TRUE);

    app_wdgts->gd_img = gdImageCreateFromFile("cache/cp_img.png");
    Contrast(app_wdgts->gd_img,
                gtk_range_get_value(
                   GTK_RANGE(app_wdgts->w_scale_contrast)), app_wdgts->gd_out);
    app_wdgts->tmp_img = gdk_pixbuf_new_from_file("cache/temp_img.png", NULL);
    update_buffer(app_wdgts);

    return 0;
}

int on_scale_temperature_value_changed(GtkMenuItem *menuitem,
                                        app_widgets *app_wdgts)
{
    printf("%ld\n", sizeof(menuitem));
    gtk_widget_set_sensitive(app_wdgts->w_scale_noise, FALSE);
    gtk_widget_set_sensitive(app_wdgts->w_scale_sharpen, FALSE);
    gtk_widget_set_sensitive(app_wdgts->w_entry_noise, FALSE);
    gtk_widget_set_sensitive(app_wdgts->w_entry_sharpen, FALSE);
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
    gtk_widget_set_sensitive(app_wdgts->w_btn_crop, FALSE);
    gtk_widget_set_sensitive(app_wdgts->w_btn_resize, FALSE);
    gtk_widget_set_sensitive(app_wdgts->w_entry_rotation, FALSE);
    gtk_widget_set_sensitive(app_wdgts->w_scale_rotation, FALSE);
    gtk_widget_set_sensitive(app_wdgts->w_cb_brush, FALSE);
    gtk_widget_set_sensitive(app_wdgts->w_cb_thickness, FALSE);
    gtk_widget_set_sensitive(app_wdgts->w_btn_color, FALSE);
    gtk_widget_set_sensitive(app_wdgts->w_btn_fill, FALSE);
    gtk_widget_set_sensitive(app_wdgts->w_btn_text, FALSE);
    gtk_widget_set_sensitive(app_wdgts->w_btn_apply, TRUE);

    app_wdgts->gd_img = gdImageCreateFromFile("cache/cp_img.png");
    Temperature(app_wdgts->gd_img, gtk_range_get_value(
        GTK_RANGE(app_wdgts->w_scale_temperature)), app_wdgts->gd_out);
    app_wdgts->tmp_img = gdk_pixbuf_new_from_file("cache/temp_img.png", NULL);
    update_buffer(app_wdgts);

    return 0;
}

int on_scale_noise_value_changed(GtkMenuItem *menuitem,
                                        app_widgets *app_wdgts)
{
    printf("%ld\n", sizeof(menuitem));
    gtk_widget_set_sensitive(app_wdgts->w_scale_brightness, FALSE);
    gtk_widget_set_sensitive(app_wdgts->w_scale_contrast, FALSE);
    gtk_widget_set_sensitive(app_wdgts->w_scale_sharpen, FALSE);
    gtk_widget_set_sensitive(app_wdgts->w_scale_temperature, FALSE);
    gtk_widget_set_sensitive(app_wdgts->w_entry_brightness, FALSE);
    gtk_widget_set_sensitive(app_wdgts->w_entry_contrast, FALSE);
    gtk_widget_set_sensitive(app_wdgts->w_entry_sharpen, FALSE);
    gtk_widget_set_sensitive(app_wdgts->w_entry_temperature, FALSE);
    gtk_widget_set_sensitive(app_wdgts->w_check_none, FALSE);
    gtk_widget_set_sensitive(app_wdgts->w_check_oldschool, FALSE);
    gtk_widget_set_sensitive(app_wdgts->w_check_glowfilter,FALSE);
    gtk_widget_set_sensitive(app_wdgts->w_check_redflagfilter, FALSE);
    gtk_widget_set_sensitive(app_wdgts->w_check_summertimefilter, FALSE);
    gtk_widget_set_sensitive(app_wdgts->w_scale_zoom, FALSE);
    gtk_widget_set_sensitive(app_wdgts->w_btn_crop, FALSE);
    gtk_widget_set_sensitive(app_wdgts->w_btn_resize, FALSE);
    gtk_widget_set_sensitive(app_wdgts->w_entry_rotation, FALSE);
    gtk_widget_set_sensitive(app_wdgts->w_scale_rotation, FALSE);
    gtk_widget_set_sensitive(app_wdgts->w_cb_brush, FALSE);
    gtk_widget_set_sensitive(app_wdgts->w_cb_thickness, FALSE);
    gtk_widget_set_sensitive(app_wdgts->w_btn_color, FALSE);
    gtk_widget_set_sensitive(app_wdgts->w_btn_fill, FALSE);
    gtk_widget_set_sensitive(app_wdgts->w_btn_text, FALSE);
    gtk_widget_set_sensitive(app_wdgts->w_btn_apply, TRUE);

    app_wdgts->gd_img = gdImageCreateFromFile("cache/cp_img.png");
    Noise(app_wdgts->gd_img, gtk_range_get_value(
        GTK_RANGE(app_wdgts->w_scale_noise)), app_wdgts->gd_out);
    app_wdgts->tmp_img = gdk_pixbuf_new_from_file("cache/temp_img.png", NULL);
    update_buffer(app_wdgts);

    return 0;
}

int on_scale_sharpen_value_changed(GtkMenuItem *menuitem,
                                        app_widgets *app_wdgts)
{
    printf("%ld\n", sizeof(menuitem));
    gtk_widget_set_sensitive(app_wdgts->w_scale_brightness, FALSE);
    gtk_widget_set_sensitive(app_wdgts->w_scale_contrast, FALSE);
    gtk_widget_set_sensitive(app_wdgts->w_scale_temperature, FALSE);
    gtk_widget_set_sensitive(app_wdgts->w_scale_noise, FALSE);
    gtk_widget_set_sensitive(app_wdgts->w_entry_brightness, FALSE);
    gtk_widget_set_sensitive(app_wdgts->w_entry_contrast, FALSE);
    gtk_widget_set_sensitive(app_wdgts->w_entry_noise, FALSE);
    gtk_widget_set_sensitive(app_wdgts->w_entry_temperature, FALSE);
    gtk_widget_set_sensitive(app_wdgts->w_check_none, FALSE);
    gtk_widget_set_sensitive(app_wdgts->w_check_oldschool, FALSE);
    gtk_widget_set_sensitive(app_wdgts->w_check_glowfilter,FALSE);
    gtk_widget_set_sensitive(app_wdgts->w_check_redflagfilter, FALSE);
    gtk_widget_set_sensitive(app_wdgts->w_check_summertimefilter, FALSE);
    gtk_widget_set_sensitive(app_wdgts->w_scale_zoom, FALSE);
    gtk_widget_set_sensitive(app_wdgts->w_btn_crop, FALSE);
    gtk_widget_set_sensitive(app_wdgts->w_btn_resize, FALSE);
    gtk_widget_set_sensitive(app_wdgts->w_entry_rotation, FALSE);
    gtk_widget_set_sensitive(app_wdgts->w_scale_rotation, FALSE);
    gtk_widget_set_sensitive(app_wdgts->w_cb_brush, FALSE);
    gtk_widget_set_sensitive(app_wdgts->w_cb_thickness, FALSE);
    gtk_widget_set_sensitive(app_wdgts->w_btn_color, FALSE);
    gtk_widget_set_sensitive(app_wdgts->w_btn_fill, FALSE);
    gtk_widget_set_sensitive(app_wdgts->w_btn_text, FALSE);
    gtk_widget_set_sensitive(app_wdgts->w_btn_apply, TRUE);

    app_wdgts->gd_img = gdImageCreateFromFile("cache/cp_img.png");
    Sharpen(app_wdgts->gd_img, gtk_range_get_value(
        GTK_RANGE(app_wdgts->w_scale_sharpen)), app_wdgts->gd_out);
    app_wdgts->tmp_img = gdk_pixbuf_new_from_file("cache/temp_img.png", NULL);
    update_buffer(app_wdgts);

    return 0;
}

int on_entry_brightness_activate(GtkMenuItem *menuitem, app_widgets *app_wdgts)
{
    printf("%ld\n", sizeof(menuitem));
    on_scale_brightness_value_changed(menuitem, app_wdgts);

    return 0;
}

int on_entry_contrast_activate(GtkMenuItem *menuitem, app_widgets *app_wdgts)
{
    printf("%ld\n", sizeof(menuitem));

    on_scale_contrast_value_changed(menuitem, app_wdgts);

    return 0;
}

int on_entry_temperature_activate(GtkMenuItem *menuitem, app_widgets *app_wdgts)
{
    printf("%ld\n", sizeof(menuitem));

    on_scale_temperature_value_changed(menuitem, app_wdgts);

    return 0;
}

int on_entry_noise_activate(GtkMenuItem *menuitem, app_widgets *app_wdgts)
{
    printf("%ld\n", sizeof(menuitem));
    
    on_scale_noise_value_changed(menuitem, app_wdgts);

    return 0;
}

int on_entry_sharpen_activate(GtkMenuItem *menuitem, app_widgets *app_wdgts)
{
    printf("%ld\n", sizeof(menuitem));

    on_scale_sharpen_value_changed(menuitem, app_wdgts);

    return 0;
}

int on_btn_apply_clicked(GtkMenuItem *menuitem, app_widgets *app_wdgts)
{
    printf("%ld\n", sizeof(menuitem));
    gtk_widget_set_sensitive(app_wdgts->w_scale_brightness, TRUE);
    gtk_widget_set_sensitive(app_wdgts->w_scale_contrast, TRUE);
    gtk_widget_set_sensitive(app_wdgts->w_scale_temperature, TRUE);
    gtk_widget_set_sensitive(app_wdgts->w_scale_noise, TRUE);
    gtk_widget_set_sensitive(app_wdgts->w_scale_sharpen, TRUE);
    gtk_widget_set_sensitive(app_wdgts->w_entry_brightness, TRUE);
    gtk_widget_set_sensitive(app_wdgts->w_entry_contrast, TRUE);
    gtk_widget_set_sensitive(app_wdgts->w_entry_temperature, TRUE);
    gtk_widget_set_sensitive(app_wdgts->w_entry_noise, TRUE);
    gtk_widget_set_sensitive(app_wdgts->w_entry_sharpen, TRUE);
    gtk_widget_set_sensitive(app_wdgts->w_check_none, TRUE);
    gtk_widget_set_sensitive(app_wdgts->w_check_oldschool, TRUE);
    gtk_widget_set_sensitive(app_wdgts->w_check_glowfilter,TRUE);
    gtk_widget_set_sensitive(app_wdgts->w_check_redflagfilter, TRUE);
    gtk_widget_set_sensitive(app_wdgts->w_check_summertimefilter, TRUE);
    gtk_widget_set_sensitive(app_wdgts->w_scale_zoom, TRUE);
    gtk_widget_set_sensitive(app_wdgts->w_btn_crop, TRUE);
    gtk_widget_set_sensitive(app_wdgts->w_btn_resize, TRUE);
    gtk_widget_set_sensitive(app_wdgts->w_entry_rotation, TRUE);
    gtk_widget_set_sensitive(app_wdgts->w_scale_rotation, TRUE);
    gtk_widget_set_sensitive(app_wdgts->w_cb_brush, TRUE);
    gtk_widget_set_sensitive(app_wdgts->w_cb_thickness, TRUE);
    gtk_widget_set_sensitive(app_wdgts->w_btn_color, TRUE);
    gtk_widget_set_sensitive(app_wdgts->w_btn_fill, TRUE);
    gtk_widget_set_sensitive(app_wdgts->w_btn_text, TRUE);
    gtk_widget_set_sensitive(app_wdgts->w_btn_apply, FALSE);

    gtk_toggle_button_set_active(
        GTK_TOGGLE_BUTTON(app_wdgts->w_check_none), FALSE);
    gtk_toggle_button_set_active(
        GTK_TOGGLE_BUTTON(app_wdgts->w_check_oldschool), FALSE);
    gtk_toggle_button_set_active(
        GTK_TOGGLE_BUTTON(app_wdgts->w_check_glowfilter),FALSE);
    gtk_toggle_button_set_active(
        GTK_TOGGLE_BUTTON(app_wdgts->w_check_redflagfilter), FALSE);
    gtk_toggle_button_set_active(
        GTK_TOGGLE_BUTTON(app_wdgts->w_check_summertimefilter), FALSE);

    if(gtk_toggle_button_get_active(
            GTK_TOGGLE_BUTTON(app_wdgts->w_check_oldschool)))
    {
        OldSchool_Filter(app_wdgts->gd_img,
            app_wdgts->gd_out, "cache/temp_img.png");
            app_wdgts->tmp_img =
            gdk_pixbuf_new_from_file("cache/temp_img.png", NULL);
    }

    if(gtk_toggle_button_get_active(
            GTK_TOGGLE_BUTTON(app_wdgts->w_check_glowfilter)))
    {
        Glow_Filter(app_wdgts->gd_img, app_wdgts->gd_out, "cache/temp_img.png");
        app_wdgts->tmp_img =
            gdk_pixbuf_new_from_file("cache/temp_img.png", NULL);
    }

    if(gtk_toggle_button_get_active(
            GTK_TOGGLE_BUTTON(app_wdgts->w_check_redflagfilter)))
    {
        RedFlag_Filter(
            app_wdgts->gd_img, app_wdgts->gd_out, "cache/temp_img.png");
        app_wdgts->tmp_img = 
            gdk_pixbuf_new_from_file("cache/temp_img.png", NULL);
    }

    if(gtk_toggle_button_get_active(
            GTK_TOGGLE_BUTTON(app_wdgts->w_check_summertimefilter)))
    {
        SummerTime_Filter(
            app_wdgts->gd_img, app_wdgts->gd_out, "cache/temp_img.png");
        app_wdgts->tmp_img =
            gdk_pixbuf_new_from_file("cache/temp_img.png", NULL);
    }
    gdk_pixbuf_savev(app_wdgts->tmp_img,
        "cache/cp_img.png", "png", NULL, NULL, NULL);
    app_wdgts->gd_img = gdImageCreateFromFile("cache/cp_img.png");
    app_wdgts->gd_w = app_wdgts->tmp_w;
    app_wdgts->gd_h = app_wdgts->tmp_h;
    insert_gdImage_list(app_wdgts);
    update_buffer(app_wdgts);
    update_all_prev(app_wdgts);

    return 0;
}

int on_check_none_toggled(GtkToggleButton *togglebutton, app_widgets *app_wdgts)
{
    if(gtk_toggle_button_get_active(togglebutton))
    {
        gtk_toggle_button_set_active(
            GTK_TOGGLE_BUTTON(app_wdgts->w_check_oldschool), FALSE);
        gtk_toggle_button_set_active(
            GTK_TOGGLE_BUTTON(app_wdgts->w_check_glowfilter),FALSE);
        gtk_toggle_button_set_active(
            GTK_TOGGLE_BUTTON(app_wdgts->w_check_redflagfilter), FALSE);
        gtk_toggle_button_set_active(
            GTK_TOGGLE_BUTTON(app_wdgts->w_check_summertimefilter), FALSE);
        gtk_toggle_button_set_active(
            GTK_TOGGLE_BUTTON(app_wdgts->w_check_vogue), FALSE);

        gtk_widget_set_sensitive(app_wdgts->w_btn_apply, TRUE);

        app_wdgts->tmp_img =
            gdk_pixbuf_new_from_file("cache/cp_img.png", NULL);
        gdk_pixbuf_savev(
            app_wdgts->tmp_img, "cache/temp_img.png", "png", NULL, NULL, NULL);
        update_buffer(app_wdgts);
    }

    return 0;
}

int on_check_oldschool_toggled(GtkToggleButton *togglebutton,
    app_widgets *app_wdgts)
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
        gtk_widget_set_sensitive(app_wdgts->w_btn_crop, FALSE);
        gtk_widget_set_sensitive(app_wdgts->w_btn_resize, FALSE);
        gtk_widget_set_sensitive(app_wdgts->w_entry_rotation, FALSE);
        gtk_widget_set_sensitive(app_wdgts->w_scale_rotation, FALSE);
        gtk_widget_set_sensitive(app_wdgts->w_cb_brush, FALSE);
        gtk_widget_set_sensitive(app_wdgts->w_cb_thickness, FALSE);
        gtk_widget_set_sensitive(app_wdgts->w_btn_color, FALSE);
        gtk_widget_set_sensitive(app_wdgts->w_btn_fill, FALSE);
        gtk_widget_set_sensitive(app_wdgts->w_btn_text, FALSE);
        gtk_widget_set_sensitive(app_wdgts->w_scale_noise, FALSE);
        gtk_widget_set_sensitive(app_wdgts->w_scale_sharpen, FALSE);
        gtk_widget_set_sensitive(app_wdgts->w_entry_noise, FALSE);
        gtk_widget_set_sensitive(app_wdgts->w_entry_sharpen, FALSE);
        gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(
            app_wdgts->w_check_none), FALSE);
        gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(
            app_wdgts->w_check_glowfilter),FALSE);
        gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(
            app_wdgts->w_check_redflagfilter), FALSE);
        gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(
            app_wdgts->w_check_summertimefilter), FALSE);
        gtk_toggle_button_set_active(
            GTK_TOGGLE_BUTTON(app_wdgts->w_check_vogue), FALSE);
        gtk_widget_set_sensitive(app_wdgts->w_btn_apply, TRUE);

        app_wdgts->gd_img = gdImageCreateFromFile("cache/cp_img.png");
        OldSchool_Filter(app_wdgts->gd_img,
            app_wdgts->gd_out, "cache/temp_img.png");
        app_wdgts->tmp_img =
            gdk_pixbuf_new_from_file("cache/temp_img.png", NULL);
        update_buffer(app_wdgts);
        update_all_prev(app_wdgts);
    }

    else
    {

    }

    return 0;
}

int on_check_summertimefilter_toggled(GtkToggleButton *togglebutton,
    app_widgets *app_wdgts)
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
        gtk_widget_set_sensitive(app_wdgts->w_btn_crop, FALSE);
        gtk_widget_set_sensitive(app_wdgts->w_btn_resize, FALSE);
        gtk_widget_set_sensitive(app_wdgts->w_entry_rotation, FALSE);
        gtk_widget_set_sensitive(app_wdgts->w_scale_rotation, FALSE);
        gtk_widget_set_sensitive(app_wdgts->w_cb_brush, FALSE);
        gtk_widget_set_sensitive(app_wdgts->w_cb_thickness, FALSE);
        gtk_widget_set_sensitive(app_wdgts->w_btn_color, FALSE);
        gtk_widget_set_sensitive(app_wdgts->w_btn_fill, FALSE);
        gtk_widget_set_sensitive(app_wdgts->w_btn_text, FALSE);
        gtk_widget_set_sensitive(app_wdgts->w_scale_noise, FALSE);
        gtk_widget_set_sensitive(app_wdgts->w_scale_sharpen, FALSE);
        gtk_widget_set_sensitive(app_wdgts->w_entry_noise, FALSE);
        gtk_widget_set_sensitive(app_wdgts->w_entry_sharpen, FALSE);
        gtk_toggle_button_set_active(
            GTK_TOGGLE_BUTTON(app_wdgts->w_check_none), FALSE);
        gtk_toggle_button_set_active(
            GTK_TOGGLE_BUTTON(app_wdgts->w_check_oldschool), FALSE);
        gtk_toggle_button_set_active(
            GTK_TOGGLE_BUTTON(app_wdgts->w_check_glowfilter),FALSE);
        gtk_toggle_button_set_active(
            GTK_TOGGLE_BUTTON(app_wdgts->w_check_redflagfilter), FALSE);
        gtk_toggle_button_set_active(
            GTK_TOGGLE_BUTTON(app_wdgts->w_check_vogue), FALSE);
        gtk_widget_set_sensitive(app_wdgts->w_btn_apply, TRUE);

        app_wdgts->gd_img = gdImageCreateFromFile("cache/cp_img.png");
        SummerTime_Filter(app_wdgts->gd_img,
            app_wdgts->gd_out, "cache/temp_img.png");
        app_wdgts->tmp_img =
            gdk_pixbuf_new_from_file("cache/temp_img.png", NULL);
        update_buffer(app_wdgts);
        update_all_prev(app_wdgts);
    }

    else
    {

    }

    return 0;
}

int on_check_glowfilter_toggled(GtkToggleButton *togglebutton,
    app_widgets *app_wdgts)
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
        gtk_widget_set_sensitive(app_wdgts->w_btn_crop, FALSE);
        gtk_widget_set_sensitive(app_wdgts->w_btn_resize, FALSE);
        gtk_widget_set_sensitive(app_wdgts->w_entry_rotation, FALSE);
        gtk_widget_set_sensitive(app_wdgts->w_scale_rotation, FALSE);
        gtk_widget_set_sensitive(app_wdgts->w_cb_brush, FALSE);
        gtk_widget_set_sensitive(app_wdgts->w_cb_thickness, FALSE);
        gtk_widget_set_sensitive(app_wdgts->w_btn_color, FALSE);
        gtk_widget_set_sensitive(app_wdgts->w_btn_fill, FALSE);
        gtk_widget_set_sensitive(app_wdgts->w_btn_text, FALSE);
        gtk_widget_set_sensitive(app_wdgts->w_scale_noise, FALSE);
        gtk_widget_set_sensitive(app_wdgts->w_scale_sharpen, FALSE);
        gtk_widget_set_sensitive(app_wdgts->w_entry_noise, FALSE);
        gtk_widget_set_sensitive(app_wdgts->w_entry_sharpen, FALSE);
        gtk_toggle_button_set_active(
            GTK_TOGGLE_BUTTON(app_wdgts->w_check_none), FALSE);
        gtk_toggle_button_set_active(
            GTK_TOGGLE_BUTTON(app_wdgts->w_check_oldschool), FALSE);
        gtk_toggle_button_set_active(
            GTK_TOGGLE_BUTTON(app_wdgts->w_check_redflagfilter), FALSE);
        gtk_toggle_button_set_active(
            GTK_TOGGLE_BUTTON(app_wdgts->w_check_summertimefilter), FALSE);
        gtk_toggle_button_set_active(
            GTK_TOGGLE_BUTTON(app_wdgts->w_check_vogue), FALSE);
        gtk_widget_set_sensitive(app_wdgts->w_btn_apply, TRUE);

        app_wdgts->gd_img = gdImageCreateFromFile("cache/cp_img.png");
        Glow_Filter(app_wdgts->gd_img, app_wdgts->gd_out,"cache/temp_img.png");
        app_wdgts->tmp_img =
            gdk_pixbuf_new_from_file("cache/temp_img.png", NULL);
        update_buffer(app_wdgts);
        update_all_prev(app_wdgts);
    }

    else
    {

    }

    return 0;
}

int on_check_redflagfilter_toggled(GtkToggleButton *togglebutton,
    app_widgets *app_wdgts)
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
        gtk_widget_set_sensitive(app_wdgts->w_btn_crop, FALSE);
        gtk_widget_set_sensitive(app_wdgts->w_btn_resize, FALSE);
        gtk_widget_set_sensitive(app_wdgts->w_entry_rotation, FALSE);
        gtk_widget_set_sensitive(app_wdgts->w_scale_rotation, FALSE);
        gtk_widget_set_sensitive(app_wdgts->w_cb_brush, FALSE);
        gtk_widget_set_sensitive(app_wdgts->w_cb_thickness, FALSE);
        gtk_widget_set_sensitive(app_wdgts->w_btn_color, FALSE);
        gtk_widget_set_sensitive(app_wdgts->w_btn_fill, FALSE);
        gtk_widget_set_sensitive(app_wdgts->w_btn_text, FALSE);
        gtk_widget_set_sensitive(app_wdgts->w_scale_noise, FALSE);
        gtk_widget_set_sensitive(app_wdgts->w_scale_sharpen, FALSE);
        gtk_widget_set_sensitive(app_wdgts->w_entry_noise, FALSE);
        gtk_widget_set_sensitive(app_wdgts->w_entry_sharpen, FALSE);
        gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(
            app_wdgts->w_check_none), FALSE);
        gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(
            app_wdgts->w_check_oldschool), FALSE);
        gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(
            app_wdgts->w_check_glowfilter),FALSE);
        gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(
            app_wdgts->w_check_summertimefilter), FALSE);
        gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(
            app_wdgts->w_check_vogue), FALSE);
        gtk_widget_set_sensitive(app_wdgts->w_btn_apply, TRUE);

        app_wdgts->gd_img = gdImageCreateFromFile("cache/cp_img.png");
        RedFlag_Filter(app_wdgts->gd_img,
                        app_wdgts->gd_out, "cache/temp_img.png");
        app_wdgts->tmp_img = 
            gdk_pixbuf_new_from_file("cache/temp_img.png", NULL);
        update_buffer(app_wdgts);
        update_all_prev(app_wdgts);
    }

    else
    {
        
    }

    return 0;
}

int on_check_vogue_toggled(GtkToggleButton *togglebutton,
    app_widgets *app_wdgts)
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
        gtk_widget_set_sensitive(app_wdgts->w_btn_crop, FALSE);
        gtk_widget_set_sensitive(app_wdgts->w_btn_resize, FALSE);
        gtk_widget_set_sensitive(app_wdgts->w_entry_rotation, FALSE);
        gtk_widget_set_sensitive(app_wdgts->w_scale_rotation, FALSE);
        gtk_widget_set_sensitive(app_wdgts->w_cb_brush, FALSE);
        gtk_widget_set_sensitive(app_wdgts->w_cb_thickness, FALSE);
        gtk_widget_set_sensitive(app_wdgts->w_btn_color, FALSE);
        gtk_widget_set_sensitive(app_wdgts->w_btn_fill, FALSE);
        gtk_widget_set_sensitive(app_wdgts->w_btn_text, FALSE);
        gtk_widget_set_sensitive(app_wdgts->w_scale_noise, FALSE);
        gtk_widget_set_sensitive(app_wdgts->w_scale_sharpen, FALSE);
        gtk_widget_set_sensitive(app_wdgts->w_entry_noise, FALSE);
        gtk_widget_set_sensitive(app_wdgts->w_entry_sharpen, FALSE);
        gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(
            app_wdgts->w_check_none), FALSE);
        gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(
            app_wdgts->w_check_oldschool), FALSE);
        gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(
            app_wdgts->w_check_glowfilter),FALSE);
        gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(
            app_wdgts->w_check_summertimefilter), FALSE);
        gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(
            app_wdgts->w_check_redflagfilter), FALSE);
        gtk_widget_set_sensitive(app_wdgts->w_btn_apply, TRUE);

        app_wdgts->gd_img = gdImageCreateFromFile("cache/cp_img.png");
        Vogue_Filter(app_wdgts->gd_img,
                        app_wdgts->gd_out, "cache/temp_img.png");
        app_wdgts->tmp_img = 
            gdk_pixbuf_new_from_file("cache/temp_img.png", NULL);
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

#pragma region OthersFunctions

//// CallbackFunctions
//
int on_btn_crop_clicked(GtkWidget *btn_crop, app_widgets *app_wdgts)
{
    printf("%ld\n", sizeof(btn_crop));
    gtk_widget_set_sensitive(app_wdgts->w_scale_brightness, FALSE);
    gtk_widget_set_sensitive(app_wdgts->w_scale_contrast, FALSE);
    gtk_widget_set_sensitive(app_wdgts->w_scale_temperature, FALSE);
    gtk_widget_set_sensitive(app_wdgts->w_entry_brightness, FALSE);
    gtk_widget_set_sensitive(app_wdgts->w_entry_contrast, FALSE);
    gtk_widget_set_sensitive(app_wdgts->w_entry_temperature, FALSE);
    gtk_widget_set_sensitive(app_wdgts->w_scale_zoom, FALSE);
    gtk_widget_set_sensitive(app_wdgts->w_check_none, FALSE);
    gtk_widget_set_sensitive(app_wdgts->w_check_oldschool, FALSE);
    gtk_widget_set_sensitive(app_wdgts->w_check_glowfilter,FALSE);
    gtk_widget_set_sensitive(app_wdgts->w_check_redflagfilter, FALSE);
    gtk_widget_set_sensitive(app_wdgts->w_check_summertimefilter, FALSE);
    gtk_widget_set_sensitive(app_wdgts->w_entry_rotation, FALSE);
    gtk_widget_set_sensitive(app_wdgts->w_btn_resize, FALSE);
    gtk_widget_set_sensitive(app_wdgts->w_scale_rotation, FALSE);
    gtk_widget_set_sensitive(app_wdgts->w_btn_apply, TRUE);    
    gtk_widget_set_sensitive(app_wdgts->w_cb_brush, FALSE);
    gtk_widget_set_sensitive(app_wdgts->w_cb_thickness, FALSE);
    gtk_widget_set_sensitive(app_wdgts->w_btn_color, FALSE);
    gtk_widget_set_sensitive(app_wdgts->w_btn_fill, FALSE);
    gtk_widget_set_sensitive(app_wdgts->w_btn_text, FALSE);

    int input_w, input_h;
    input_w =
        gtk_spin_button_get_value_as_int (
            GTK_SPIN_BUTTON(app_wdgts->w_entryW_crop));
    input_h =
        gtk_spin_button_get_value_as_int (
            GTK_SPIN_BUTTON(app_wdgts->w_entryH_crop));

    input_w = input_w > app_wdgts->tmp_w ? app_wdgts->tmp_w : input_w;
    input_h = input_h > app_wdgts->tmp_h ? app_wdgts->tmp_h : input_h;

    gtk_spin_button_set_value(
        GTK_SPIN_BUTTON(app_wdgts->w_entryW_crop), input_w);
    gtk_spin_button_set_value(
        GTK_SPIN_BUTTON(app_wdgts->w_entryH_crop), input_h);

    Crop(app_wdgts->gd_img,
        app_wdgts->gd_out, "cache/temp_img.png", 0, 0, input_w, input_h);

    app_wdgts->tmp_img = gdk_pixbuf_new_from_file("cache/temp_img.png", NULL);
    app_wdgts->tmp_w = input_w;
    app_wdgts->tmp_h = input_h;
    app_wdgts->gd_w = input_w;
    app_wdgts->gd_h = input_h;
    update_buffer(app_wdgts);
    return 0;
}

int on_btn_resize_clicked(GtkWidget *btn_resize, app_widgets *app_wdgts)
{
    printf("%ld\n", sizeof(btn_resize));
    gtk_widget_set_sensitive(app_wdgts->w_scale_brightness, FALSE);
    gtk_widget_set_sensitive(app_wdgts->w_scale_contrast, FALSE);
    gtk_widget_set_sensitive(app_wdgts->w_scale_temperature, FALSE);
    gtk_widget_set_sensitive(app_wdgts->w_entry_brightness, FALSE);
    gtk_widget_set_sensitive(app_wdgts->w_entry_contrast, FALSE);
    gtk_widget_set_sensitive(app_wdgts->w_entry_temperature, FALSE);
    gtk_widget_set_sensitive(app_wdgts->w_scale_zoom, FALSE);
    gtk_widget_set_sensitive(app_wdgts->w_check_none, FALSE);
    gtk_widget_set_sensitive(app_wdgts->w_check_oldschool, FALSE);
    gtk_widget_set_sensitive(app_wdgts->w_check_glowfilter,FALSE);
    gtk_widget_set_sensitive(app_wdgts->w_check_redflagfilter, FALSE);
    gtk_widget_set_sensitive(app_wdgts->w_check_summertimefilter, FALSE);
    gtk_widget_set_sensitive(app_wdgts->w_btn_crop, FALSE);
    gtk_widget_set_sensitive(app_wdgts->w_entry_rotation, FALSE);
    gtk_widget_set_sensitive(app_wdgts->w_scale_rotation, FALSE);
    gtk_widget_set_sensitive(app_wdgts->w_cb_brush, FALSE);
    gtk_widget_set_sensitive(app_wdgts->w_cb_thickness, FALSE);
    gtk_widget_set_sensitive(app_wdgts->w_btn_color, FALSE);
    gtk_widget_set_sensitive(app_wdgts->w_btn_fill, FALSE);
    gtk_widget_set_sensitive(app_wdgts->w_btn_text, FALSE);
    gtk_widget_set_sensitive(app_wdgts->w_btn_apply, TRUE);

    int input_w, input_h;
    input_w = gtk_spin_button_get_value_as_int (
        GTK_SPIN_BUTTON(app_wdgts->w_entryW_crop));
    input_h = gtk_spin_button_get_value_as_int (
        GTK_SPIN_BUTTON(app_wdgts->w_entryH_crop));

    Resize(app_wdgts->gd_img,
                app_wdgts->gd_out, "cache/temp_img.png", input_w, input_h);

    app_wdgts->tmp_img = gdk_pixbuf_new_from_file("cache/temp_img.png", NULL);
    app_wdgts->tmp_w = input_w;
    app_wdgts->tmp_h = input_h;
    update_buffer(app_wdgts);
    return 0;
}

int on_scale_rotation_value_changed(GtkWidget *entry_rotation,
                                            app_widgets *app_wdgts)
{
    printf("%ld\n", sizeof(entry_rotation));
    gtk_widget_set_sensitive(app_wdgts->w_scale_brightness, FALSE);
    gtk_widget_set_sensitive(app_wdgts->w_scale_contrast, FALSE);
    gtk_widget_set_sensitive(app_wdgts->w_scale_temperature, FALSE);
    gtk_widget_set_sensitive(app_wdgts->w_entry_brightness, FALSE);
    gtk_widget_set_sensitive(app_wdgts->w_entry_contrast, FALSE);
    gtk_widget_set_sensitive(app_wdgts->w_entry_temperature, FALSE);
    gtk_widget_set_sensitive(app_wdgts->w_scale_zoom, FALSE);
    gtk_widget_set_sensitive(app_wdgts->w_check_none, FALSE);
    gtk_widget_set_sensitive(app_wdgts->w_check_oldschool, FALSE);
    gtk_widget_set_sensitive(app_wdgts->w_check_glowfilter,FALSE);
    gtk_widget_set_sensitive(app_wdgts->w_check_redflagfilter, FALSE);
    gtk_widget_set_sensitive(app_wdgts->w_check_summertimefilter, FALSE);
    gtk_widget_set_sensitive(app_wdgts->w_btn_crop, FALSE);
    gtk_widget_set_sensitive(app_wdgts->w_btn_resize, FALSE);
    gtk_widget_set_sensitive(app_wdgts->w_cb_brush, FALSE);
    gtk_widget_set_sensitive(app_wdgts->w_cb_thickness, FALSE);
    gtk_widget_set_sensitive(app_wdgts->w_btn_color, FALSE);
    gtk_widget_set_sensitive(app_wdgts->w_btn_fill, FALSE);
    gtk_widget_set_sensitive(app_wdgts->w_btn_text, FALSE);
    gtk_widget_set_sensitive(app_wdgts->w_btn_apply, TRUE);

    Rotate(app_wdgts->gd_img, app_wdgts->gd_out,
                "cache/temp_img.png",
                        gtk_range_get_value(
                            GTK_RANGE(app_wdgts->w_scale_rotation)));
    app_wdgts->tmp_img = gdk_pixbuf_new_from_file("cache/temp_img.png", NULL);
    app_wdgts->tmp_w = gdk_pixbuf_get_width(app_wdgts->tmp_img);
    app_wdgts->tmp_h = gdk_pixbuf_get_height(app_wdgts->tmp_img);
    update_buffer(app_wdgts);
    return 0;
}

int on_btn_text_focus_in_event(GtkWidget *text_button, GdkEventButton *event,
                                    app_widgets *app_wdgts)
{
    printf("%ld\n", sizeof(text_button));
    printf("%ld\n", sizeof(event));
    app_wdgts->focus_text = 1;
    return 0;
}

int on_btn_text_focus_out_event(GtkWidget *text_button, GdkEventButton *event,
                                    app_widgets *app_wdgts)
{
    app_wdgts->focus_text = 0;
    printf("%ld\n", sizeof(text_button));
    printf("%ld\n", sizeof(event));
    return 0;
}
//
////

#pragma endregion OthersFunctions