/* Project Information
 * Group: Orion
 * Project: Nebula
 * Authors: Tom TERMAAT, André HOANG, Albert EL HELOU, Adrien HICAUBERT
 * Description: Orion Nebula is a raster graphics editor.
 */

//#pragma  region Include

#define _GNU_SOURCE
#define CTRL_KEYPRESS(k) ((k)  & 0x1f) 

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

#include <gtk/gtk.h>
#include <gdk/gdk.h>
#include <gdk-pixbuf/gdk-pixbuf.h>

#include <gd.h>

#include ".h/filters.h"
#include ".h/image_settings.h"
#include ".h/drawing.h"
#include ".h/func_divers.h"

//#pragma  endregion Include

//#pragma  region Structure

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

struct Cairo_color {
    double r;
    double g;
    double b;
    double a;
};

struct gdImage_list {
    gdImagePtr img;
    struct gdImage_list *next;
};

typedef struct {

    // Pointer to GtkWidget
    GtkWidget *w_window;
    GtkWidget *w_dlg_file_choose;
    GtkWidget *w_dlg_file_save;
    GtkWidget *w_pop_up_text;
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
    GtkWidget *w_prev_winterfrost;
    GtkWidget *w_prev_timeoclockfilter;
    GtkWidget *w_prev_weekdayfilter;
    GtkWidget *w_check_none;
    GtkWidget *w_check_oldschool;
    GtkWidget *w_check_glowfilter;
    GtkWidget *w_check_redflagfilter;
    GtkWidget *w_check_summertimefilter;
    GtkWidget *w_check_vogue;
    GtkWidget *w_check_winterfrost;
    GtkWidget *w_check_timeoclockfilter;
    GtkWidget *w_check_weekdayfilter;
    GtkWidget *w_cb_brush;
    GtkWidget *w_cb_thickness;
    GtkWidget *w_btn_color;
    GtkWidget *w_btn_fill;
    GtkWidget *w_btn_text;
    GtkWidget *w_btn_erase;
    GtkWidget *w_btn_motif;
    GtkWidget *w_btn_font;
    GtkWidget *w_prev_circle;
    GtkWidget *w_prev_heart;
    GtkWidget *w_prev_pentagon;
    GtkWidget *w_prev_square;
    GtkWidget *w_prev_star;
    GtkWidget *w_prev_triangle;
    GtkWidget *w_btn_heart;
    GtkWidget *w_btn_circle;
    GtkWidget *w_btn_pentagon;
    GtkWidget *w_btn_square;
    GtkWidget *w_btn_star;
    GtkWidget *w_btn_triangle;
    GtkWidget *w_entry_shapes;
    GtkWidget *w_btn_collage;
    GtkWidget *w_window_collage;
    GtkWidget *w_prev_img1;
    GtkWidget *w_prev_img2;
    GtkWidget *w_prev_img3;
    GtkWidget *w_prev_img4;
    GtkWidget *w_prev_preset1;
    GtkWidget *w_prev_preset2;
    GtkWidget *w_prev_preset3;
    GtkWidget *w_check_preset1;
    GtkWidget *w_check_preset2;
    GtkWidget *w_check_preset3;
    GtkWidget *w_prev_final;
    GtkWidget *w_btn_collage_img1;
    GtkWidget *w_btn_collage_img2;
    GtkWidget *w_btn_collage_img3;
    GtkWidget *w_btn_collage_img4;
    GtkWidget *w_btn_collage_delete1;
    GtkWidget *w_btn_collage_delete2;
    GtkWidget *w_btn_collage_delete3;
    GtkWidget *w_btn_collage_delete4;
    GtkWidget *w_text_size;
    GtkWidget *w_entry_text;
    GtkWidget *w_color_text;
    GtkWidget *w_menubar_save;
    GtkWidget *w_menubar_save_as;

    int *color_array;
    int *draw_array;
    char* save_path;

    // Temp. image info
    GdkPixbuf *tmp_img;
    int tmp_w;
    int tmp_h;
    int zoomed_w;
    int zoomed_h;

    // Gd image info
    gdImagePtr gd_img;
    int gd_w;
    int gd_h;
    FILE *gd_out;

    // undo / redo
    struct gdImage_list *gd_list;

    // Image to insert
    gdImagePtr insert_img;
    int insert_img_w;
    int insert_img_h;

    // Drawing info
    int brush_type;
    int thickness;
    struct Color *color; 
    struct Cairo_color *color_get;
    int cr_action; 

    // Collage info
    int img_count;
    char *img1_path;
    char *img2_path;
    char *img3_path;
    char *img4_path;
    GdkPixbuf *prev_final;

    // global variables
    char *resolution_info;
    int zoom;
    int focus_text;
    int focus_fill;
    int focus_draw;
    int focus_erase;
    int focus_motif;
    int insert_img_select;
    char* pattern_type;
    float pattern_size;
    char** font_path;
    int font_index;
    int tmp_x;
    int tmp_y;

} app_widgets;

//#pragma  endregion Structure

//#pragma  region Main

struct Point *p1, *p2, *start, *start2;

int init_shape_and_pattern(app_widgets *app_wdgts)
{
    char* shapes_and_pattern_PathList[6] = {
        "src/resources/func_divers/square.png",
        "src/resources/func_divers/circle.png",
        "src/resources/func_divers/triangle.png",
        "src/resources/func_divers/pentagon.png",
        "src/resources/func_divers/star.png",
        "src/resources/func_divers/heart.png"
    };

    GtkWidget* shapes_and_pattern_WidgetList[6] = {
        app_wdgts->w_prev_square,
        app_wdgts->w_prev_circle,
        app_wdgts->w_prev_triangle,
        app_wdgts->w_prev_pentagon,
        app_wdgts->w_prev_star,
        app_wdgts->w_prev_heart
    };

    size_t len = 6;
    int coef = 16;

    for(size_t i = 0; i < len; i++){

        GdkPixbuf *prev =
            gdk_pixbuf_new_from_file(
                    shapes_and_pattern_PathList[i], 
                    NULL
                    );

        prev = gdk_pixbuf_scale_simple(prev,
                gdk_pixbuf_get_width(prev)/coef,
                gdk_pixbuf_get_height(prev)/coef,
                GDK_INTERP_NEAREST
                );

        gtk_image_set_from_pixbuf(
                GTK_IMAGE(shapes_and_pattern_WidgetList[i]), 
                prev
                );

    }

    return 0;
}

int main(int argc, char *argv[])
{
    GtkBuilder      *builder;
    app_widgets     *widgets = g_slice_new(app_widgets);

    p1 = p2 = start = start2 = NULL;
    start = malloc(sizeof(struct Point));
    memset(start, 0, sizeof(struct Point));
    start2 = malloc(sizeof(struct Point));
    memset(start2, 0, sizeof(struct Point));
    widgets->color = calloc(sizeof(struct Color), 0);
    widgets->color_get = calloc(sizeof(struct Color), 0);
    widgets->gd_list = malloc(sizeof(struct gdImage_list));
    widgets->gd_list->img = NULL;
    widgets->gd_list->next = NULL;

    p1 = start;
    p2 = start2;
    widgets->focus_draw = 0;
    widgets->focus_fill = 0;
    widgets->focus_erase = 0;

    widgets->cr_action = 0;
    widgets->thickness = 1;
    widgets->insert_img_select = 0;

    widgets->zoomed_w = 0;
    widgets->zoomed_h = 0;

    widgets->tmp_x = 0;
    widgets->tmp_y = 0;

    widgets->pattern_size = 0.3;
    widgets->pattern_type = "square";

    widgets->img_count = 0;

    widgets->save_path = "";

    widgets->font_index = 0;
    char* font_path[] = {
            "src/resources/Text_fonts/Absolute.ttf",
            "src/resources/Text_fonts/AmstajulBufett.ttf",
            "src/resources/Text_fonts/Arial.ttf",
            "src/resources/Text_fonts/Bethanie.ttf",
            "src/resources/Text_fonts/Brook.ttf",
            "src/resources/Text_fonts/Calibri.ttf",
            "src/resources/Text_fonts/Cambria.ttf",
            "src/resources/Text_fonts/Caudex.ttf",
            "src/resources/Text_fonts/Digit.ttf",
            "src/resources/Text_fonts/KarpowBold.ttf",
            "src/resources/Text_fonts/NotoSans.ttf",
            "src/resources/Text_fonts/Oswald.ttf",
            "src/resources/Text_fonts/PROPAGANDA.ttf",
            "src/resources/Text_fonts/Ristella.ttf",
            "src/resources/Text_fonts/ThunderLord.ttf",
            "src/resources/Text_fonts/Times.ttf",
            "src/resources/Text_fonts/Vogue.ttf",
    }; 

    widgets->font_path = malloc(18*sizeof(char*));

    for(int i = 0; i < 17; i++)
    {
            widgets->font_path[i] = font_path[i];
    }

    gtk_init(&argc, &argv);

    builder = gtk_builder_new_from_file("src/glade/window_main.glade");

    widgets->w_window = GTK_WIDGET(
            gtk_builder_get_object(builder, "window_main"));
    widgets->w_dlg_file_choose = GTK_WIDGET(
            gtk_builder_get_object(builder, "dlg_file_choose"));
    widgets->w_dlg_file_save = GTK_WIDGET(
            gtk_builder_get_object(builder, "dlg_file_save"));
    widgets->w_pop_up_text = GTK_WIDGET(
            gtk_builder_get_object(builder, "pop_up_text"));
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
    widgets->w_prev_winterfrost = GTK_WIDGET(
            gtk_builder_get_object(builder, "prev_winterfrost"));
    widgets->w_prev_timeoclockfilter = GTK_WIDGET(
            gtk_builder_get_object(builder, "prev_timeoclockfilter"));
    widgets->w_prev_weekdayfilter = GTK_WIDGET(
            gtk_builder_get_object(builder, "prev_weekdayfilter"));
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
    widgets->w_check_winterfrost = GTK_WIDGET(
            gtk_builder_get_object(builder, "check_winterfrost"));
    widgets->w_check_weekdayfilter = GTK_WIDGET(
            gtk_builder_get_object(builder, "check_weekdayfilter"));
    widgets->w_check_timeoclockfilter = GTK_WIDGET(
            gtk_builder_get_object(builder, "check_timeoclockfilter"));
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
    widgets->w_btn_motif = GTK_WIDGET(
            gtk_builder_get_object(builder, "btn_motif"));
    widgets->w_btn_font = GTK_WIDGET(
            gtk_builder_get_object(builder, "btn_font"));
    widgets->w_prev_square = GTK_WIDGET(
            gtk_builder_get_object(builder, "prev_square"));
    widgets->w_prev_circle = GTK_WIDGET(
            gtk_builder_get_object(builder, "prev_circle"));
    widgets->w_prev_triangle = GTK_WIDGET(
            gtk_builder_get_object(builder, "prev_triangle"));
    widgets->w_prev_pentagon = GTK_WIDGET(
            gtk_builder_get_object(builder, "prev_pentagon"));
    widgets->w_prev_star = GTK_WIDGET(
            gtk_builder_get_object(builder, "prev_star"));
    widgets->w_prev_heart = GTK_WIDGET(
            gtk_builder_get_object(builder, "prev_heart"));
    widgets->w_btn_square = GTK_WIDGET(
            gtk_builder_get_object(builder, "btn_square"));
    widgets->w_btn_circle = GTK_WIDGET(
            gtk_builder_get_object(builder, "btn_circle"));
    widgets->w_btn_triangle = GTK_WIDGET(
            gtk_builder_get_object(builder, "btn_triangle"));
    widgets->w_btn_pentagon = GTK_WIDGET(
            gtk_builder_get_object(builder, "btn_pentagon"));
    widgets->w_btn_star = GTK_WIDGET(
            gtk_builder_get_object(builder, "btn_star"));
    widgets->w_btn_heart = GTK_WIDGET(
            gtk_builder_get_object(builder, "btn_heart"));
    widgets->w_entry_shapes = GTK_WIDGET(
            gtk_builder_get_object(builder, "entry_shapes"));  
    widgets->w_btn_collage = GTK_WIDGET(
            gtk_builder_get_object(builder, "btn_collage"));
    widgets->w_window_collage = GTK_WIDGET(
            gtk_builder_get_object(builder, "window_collage"));  
    widgets->w_prev_img1 = GTK_WIDGET(
            gtk_builder_get_object(builder, "prev_img1"));
    widgets->w_prev_img2 = GTK_WIDGET(
            gtk_builder_get_object(builder, "prev_img2"));  
    widgets->w_prev_img3 = GTK_WIDGET(
            gtk_builder_get_object(builder, "prev_img3"));
    widgets->w_prev_img4 = GTK_WIDGET(
            gtk_builder_get_object(builder, "prev_img4"));
    widgets->w_prev_preset1 = GTK_WIDGET(
            gtk_builder_get_object(builder, "prev_preset1"));
    widgets->w_prev_preset2 = GTK_WIDGET(
            gtk_builder_get_object(builder, "prev_preset2"));
    widgets->w_prev_preset3 = GTK_WIDGET(
            gtk_builder_get_object(builder, "prev_preset3"));
    widgets->w_check_preset1 = GTK_WIDGET(
            gtk_builder_get_object(builder, "check_preset1"));
    widgets->w_check_preset2 = GTK_WIDGET(
            gtk_builder_get_object(builder, "check_preset2"));
    widgets->w_check_preset3 = GTK_WIDGET(
            gtk_builder_get_object(builder, "check_preset3"));
    widgets->w_prev_final = GTK_WIDGET(
            gtk_builder_get_object(builder, "prev_final"));
    widgets->w_btn_collage_img1 = GTK_WIDGET(
            gtk_builder_get_object(builder, "btn_collage_img1"));
    widgets->w_btn_collage_img2 = GTK_WIDGET(
            gtk_builder_get_object(builder, "btn_collage_img2"));
    widgets->w_btn_collage_img3 = GTK_WIDGET(
            gtk_builder_get_object(builder, "btn_collage_img3"));
    widgets->w_btn_collage_img4 = GTK_WIDGET(
            gtk_builder_get_object(builder, "btn_collage_img4"));
    widgets->w_btn_collage_delete1 = GTK_WIDGET(
            gtk_builder_get_object(builder, "btn_collage_delete1"));
    widgets->w_btn_collage_delete2 = GTK_WIDGET(
            gtk_builder_get_object(builder, "btn_collage_delete2"));
    widgets->w_btn_collage_delete3 = GTK_WIDGET(
            gtk_builder_get_object(builder, "btn_collage_delete3"));
    widgets->w_btn_collage_delete4 = GTK_WIDGET(
            gtk_builder_get_object(builder, "btn_collage_delete4"));
    widgets->w_text_size = GTK_WIDGET(
            gtk_builder_get_object(builder, "text_size"));
    widgets->w_entry_text = GTK_WIDGET(
            gtk_builder_get_object(builder, "entry_text"));
    widgets->w_color_text = GTK_WIDGET(
            gtk_builder_get_object(builder, "text_color"));
    widgets->w_menubar_save = GTK_WIDGET(
            gtk_builder_get_object(builder, "menubar_save"));
    widgets->w_menubar_save_as = GTK_WIDGET(
            gtk_builder_get_object(builder, "menubar_save_as"));

    gtk_builder_connect_signals(builder, widgets);
    g_object_unref(builder);

    gtk_widget_set_events(widgets->w_drawing_aera, GDK_BUTTON_MOTION_MASK |
            GDK_BUTTON_PRESS_MASK | GDK_BUTTON_RELEASE_MASK);

    init_shape_and_pattern(widgets);

    gtk_file_chooser_set_current_name(GTK_FILE_CHOOSER(widgets->w_dlg_file_save), "Untitled.png");

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
    gtk_widget_set_sensitive(widgets->w_check_timeoclockfilter, FALSE);
    gtk_widget_set_sensitive(widgets->w_check_vogue, FALSE);
    gtk_widget_set_sensitive(widgets->w_check_winterfrost, FALSE);
    gtk_widget_set_sensitive(widgets->w_check_weekdayfilter, FALSE);
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
    gtk_widget_set_sensitive(widgets->w_btn_square, FALSE);
    gtk_widget_set_sensitive(widgets->w_btn_circle, FALSE);
    gtk_widget_set_sensitive(widgets->w_btn_pentagon, FALSE);
    gtk_widget_set_sensitive(widgets->w_btn_star, FALSE);
    gtk_widget_set_sensitive(widgets->w_btn_heart, FALSE);
    gtk_widget_set_sensitive(widgets->w_btn_triangle, FALSE);
    gtk_widget_set_sensitive(widgets->w_entry_shapes, FALSE);
    gtk_widget_set_sensitive(widgets->w_menubar_save, FALSE);
    gtk_widget_set_sensitive(widgets->w_menubar_save_as, FALSE);

    system("mkdir cache/");

    gtk_widget_show(widgets->w_window);
    gtk_main();

    gdFree(widgets->gd_img);
    free(widgets->color);
    free(widgets->font_path);
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

//#pragma  endregion Main

//#pragma  region Utils

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

        if (start2 != NULL && start2->next != NULL)
        {
            start2->next = NULL;
            p2 = start2;
        }
    }

    return c;
}
//
////

//#pragma  endregion Utils

//#pragma  region Interface

//// Functions
int init_pop_up_text(app_widgets *app_wdgts)
{
        gtk_entry_set_text(GTK_ENTRY(app_wdgts->w_entry_text), "");

        GdkRGBA *color_set = malloc(sizeof(GdkRGBA));
        color_set->red = 0.0;
        color_set->green = 0.0;
        color_set->blue = 0.0;
        color_set->alpha = 1.0;         
        gtk_color_chooser_set_rgba(GTK_COLOR_CHOOSER(app_wdgts->w_color_text), color_set); 
        free(color_set);

        return 0;
}

int on_menubar_save_as_activate(GtkWidget *menuitem, app_widgets *app_wdgts)
{
    printf("%ld\n", sizeof(menuitem));
    // Show the "Open Image" dialog box
    gtk_widget_show(app_wdgts->w_dlg_file_save);

    // Check return value from Open Image dialog box to 
    // see if user clicked the Open button
    if (gtk_dialog_run(
                GTK_DIALOG (app_wdgts->w_dlg_file_save)) == GTK_RESPONSE_OK) {

        // Get the file name from the dialog box
        char *file_name = gtk_file_chooser_get_filename(
                GTK_FILE_CHOOSER(app_wdgts->w_dlg_file_save));
        if (file_name != NULL) {

            app_wdgts->save_path = file_name;
            app_wdgts->gd_out = fopen (file_name, "wb");
            gdImagePng(app_wdgts->gd_img, app_wdgts->gd_out);            
            gtk_widget_set_sensitive(app_wdgts->w_menubar_save, TRUE);
            fclose(app_wdgts->gd_out);
        }
    }

    // Finished with the "Open Image" dialog box, so hide it
    gtk_widget_hide(app_wdgts->w_dlg_file_save);

    return 0;
}

int on_menubar_save_activate(GtkWidget *menuitem, app_widgets *app_wdgts)
{
        printf("%ld\n", sizeof(menuitem));
        app_wdgts->gd_out = fopen (app_wdgts->save_path, "wb");
        gdImagePng(app_wdgts->gd_img, app_wdgts->gd_out);
        fclose(app_wdgts->gd_out);
        return 0;
}

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
            app_wdgts->zoomed_w, app_wdgts->zoomed_h, GDK_INTERP_NEAREST);
    gtk_widget_set_size_request(app_wdgts->w_drawing_aera,
            app_wdgts->zoomed_w, app_wdgts->zoomed_h);
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
    int coef, width, height;  
    if(app_wdgts->gd_w > app_wdgts->gd_h)
    {
        coef = app_wdgts->gd_w / 150;
        width = app_wdgts->gd_w / coef;
        height = app_wdgts->gd_h / coef;
    }
    else
    {
        coef = app_wdgts->gd_h / 100;
        width = app_wdgts->gd_w / coef;
        height = app_wdgts->gd_h / coef;
    }

    GdkPixbuf *prev_none = gdk_pixbuf_new_from_file("cache/cp_img.png", NULL);
    prev_none = gdk_pixbuf_scale_simple(prev_none, width,
            height, GDK_INTERP_NEAREST);
    gtk_image_set_from_pixbuf(GTK_IMAGE(app_wdgts->w_prev_none), prev_none);

    gdImagePtr gd_prev = gdImageCreateFromFile("cache/cp_img.png");
    OldSchool_Filter(gd_prev, app_wdgts->gd_out, "cache/prev_img.png");
    GdkPixbuf *prev_oldschool =
        gdk_pixbuf_new_from_file("cache/prev_img.png", NULL);
    prev_oldschool = gdk_pixbuf_scale_simple(prev_oldschool, 
            width, height, GDK_INTERP_NEAREST);
    gtk_image_set_from_pixbuf(
            GTK_IMAGE(app_wdgts->w_prev_oldschool), prev_oldschool);

    gd_prev = gdImageCreateFromFile("cache/cp_img.png");
    Glow_Filter(gd_prev, app_wdgts->gd_out, "cache/prev_img.png");
    GdkPixbuf *prev_glowfilter =
        gdk_pixbuf_new_from_file("cache/prev_img.png", NULL);
    prev_glowfilter = gdk_pixbuf_scale_simple(prev_glowfilter,
            width, height, GDK_INTERP_NEAREST);
    gtk_image_set_from_pixbuf(GTK_IMAGE(app_wdgts->w_prev_glowfilter),
            prev_glowfilter);

    gd_prev = gdImageCreateFromFile("cache/cp_img.png");
    RedFlag_Filter(gd_prev, app_wdgts->gd_out, "cache/prev_img.png");
    GdkPixbuf *prev_redflagfilter =
        gdk_pixbuf_new_from_file("cache/prev_img.png", NULL);
    prev_redflagfilter = gdk_pixbuf_scale_simple(prev_redflagfilter,
            width, height, GDK_INTERP_NEAREST);
    gtk_image_set_from_pixbuf(GTK_IMAGE(app_wdgts->w_prev_redflagfilter),
            prev_redflagfilter);

    gd_prev = gdImageCreateFromFile("cache/cp_img.png");
    SummerTime_Filter(gd_prev, app_wdgts->gd_out, "cache/prev_img.png");
    GdkPixbuf *prev_summertimefilter =
        gdk_pixbuf_new_from_file("cache/prev_img.png", NULL);
    prev_summertimefilter = gdk_pixbuf_scale_simple(prev_summertimefilter,
            width, height, GDK_INTERP_NEAREST);
    gtk_image_set_from_pixbuf(GTK_IMAGE(app_wdgts->w_prev_summertimefilter),
            prev_summertimefilter);

    gd_prev = gdImageCreateFromFile("cache/cp_img.png");
    Vogue_Filter(gd_prev, app_wdgts->gd_out, "cache/prev_img.png");
    GdkPixbuf *prev_vogue =
        gdk_pixbuf_new_from_file("cache/prev_img.png", NULL);
    prev_vogue = gdk_pixbuf_scale_simple(prev_vogue,
            width, height, GDK_INTERP_NEAREST);
    gtk_image_set_from_pixbuf(GTK_IMAGE(app_wdgts->w_prev_vogue),
            prev_vogue);

    gd_prev = gdImageCreateFromFile("cache/cp_img.png");
    WinterFrost_Filter(gd_prev, app_wdgts->gd_out, "cache/prev_img.png");
    GdkPixbuf *prev_winterfrost =
        gdk_pixbuf_new_from_file("cache/prev_img.png", NULL);
    prev_winterfrost = gdk_pixbuf_scale_simple(prev_winterfrost,
            width, height, GDK_INTERP_NEAREST);
    gtk_image_set_from_pixbuf(GTK_IMAGE(app_wdgts->w_prev_winterfrost),
            prev_winterfrost);

    gd_prev = gdImageCreateFromFile("cache/cp_img.png");
    TimeOClock_Filter(gd_prev, app_wdgts->gd_out, "cache/prev_img.png");
    GdkPixbuf *prev_timeoclockfilter =
        gdk_pixbuf_new_from_file("cache/prev_img.png", NULL);
    prev_timeoclockfilter = gdk_pixbuf_scale_simple(prev_timeoclockfilter,
            width, height, GDK_INTERP_NEAREST);
    gtk_image_set_from_pixbuf(GTK_IMAGE(app_wdgts->w_prev_timeoclockfilter),
            prev_timeoclockfilter);
    
    gd_prev = gdImageCreateFromFile("cache/cp_img.png");
    WeekDay_Filter(gd_prev, app_wdgts->gd_out, "cache/prev_img.png");
    GdkPixbuf *prev_weekdayfilter =
        gdk_pixbuf_new_from_file("cache/prev_img.png", NULL);
    prev_weekdayfilter = gdk_pixbuf_scale_simple(prev_weekdayfilter,
            width, height, GDK_INTERP_NEAREST);
    gtk_image_set_from_pixbuf(GTK_IMAGE(app_wdgts->w_prev_weekdayfilter),
            prev_weekdayfilter);

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

int print_array(app_widgets *app_wdgts, int *array)
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

int update_xy(app_widgets *app_widgets, int *x, int *y)
{

    int zoom = app_widgets->zoom; 

    if (zoom == 0)
        zoom = 1;

    if(zoom < 0){
        zoom = -zoom;
        *x *= zoom;
        *y *= zoom;
    }
    else{
        *x /= zoom;
        *y /= zoom;
    }

    return 0;
}

int init_main_window(app_widgets *app_wdgts)
{
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
    gtk_widget_set_sensitive(app_wdgts->w_check_none, TRUE);
    gtk_widget_set_sensitive(app_wdgts->w_check_oldschool, TRUE);
    gtk_widget_set_sensitive(app_wdgts->w_check_glowfilter,TRUE);
    gtk_widget_set_sensitive(app_wdgts->w_check_redflagfilter, TRUE);
    gtk_widget_set_sensitive(app_wdgts->w_check_summertimefilter, TRUE);
    gtk_widget_set_sensitive(app_wdgts->w_check_vogue, TRUE);
    gtk_widget_set_sensitive(app_wdgts->w_check_winterfrost, TRUE);
    gtk_widget_set_sensitive(app_wdgts->w_check_timeoclockfilter, TRUE);
    gtk_widget_set_sensitive(app_wdgts->w_check_weekdayfilter, TRUE);
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
    gtk_widget_set_sensitive(app_wdgts->w_btn_circle, TRUE);
    gtk_widget_set_sensitive(app_wdgts->w_btn_pentagon, TRUE);
    gtk_widget_set_sensitive(app_wdgts->w_btn_square, TRUE);
    gtk_widget_set_sensitive(app_wdgts->w_btn_star, TRUE);
    gtk_widget_set_sensitive(app_wdgts->w_btn_heart, TRUE);
    gtk_widget_set_sensitive(app_wdgts->w_btn_triangle, TRUE);
    gtk_widget_set_sensitive(app_wdgts->w_entry_shapes, TRUE); 
    gtk_widget_set_sensitive(app_wdgts->w_scale_zoom, TRUE);
    gtk_widget_set_sensitive(app_wdgts->w_menubar_save_as, TRUE);

    GdkRGBA *color_set = malloc(sizeof(GdkRGBA));
    color_set->red = 0.0;
    color_set->green = 0.0;
    color_set->blue = 0.0;
    color_set->alpha = 1.0;            

    gtk_color_chooser_set_rgba(GTK_COLOR_CHOOSER(app_wdgts->w_btn_color), color_set); 
    free(color_set);

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
    app_wdgts->zoomed_w = img_w;
    app_wdgts->zoomed_h = img_h;

    //init_color_array(app_wdgts);

    // Redraw the drawing area to display the image in the UI
    gtk_widget_set_size_request(app_wdgts->w_drawing_aera,
            app_wdgts->tmp_w, app_wdgts->tmp_h);
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

    while(app_wdgts->gd_list != NULL && app_wdgts->gd_list->next != NULL)
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
    return 0;
}
//

//// SignalCallbackFunctions
int on_menubar_open_activate(GtkWidget *menuitem, app_widgets *app_wdgts)
{
    app_wdgts->focus_draw = 0;
    app_wdgts->focus_fill = 0;
    app_wdgts->focus_erase = 0;

    app_wdgts->cr_action = 0;
    app_wdgts->thickness = 1;
    app_wdgts->insert_img_select = 0;

    app_wdgts->zoomed_w = 0;
    app_wdgts->zoomed_h = 0;

    app_wdgts->pattern_size = 0.3;
    app_wdgts->pattern_type = "square";

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

            app_wdgts->save_path = file_name;

            // Create the pixbuf image to display
            app_wdgts->tmp_img = gdk_pixbuf_new_from_file(file_name, NULL);

            // Save copies of the original files
            gdk_pixbuf_savev(
                    app_wdgts->tmp_img, "cache/cp_img.png", "png", NULL, NULL, NULL);
            gdk_pixbuf_savev(
                    app_wdgts->tmp_img, "cache/temp_img.png", "png", NULL, NULL, NULL);

            app_wdgts->gd_img = gdImageCreateFromFile("cache/cp_img.png");

            init_main_window(app_wdgts);
        }
    }

    // Finished with the "Open Image" dialog box, so hide it
    gtk_widget_hide(app_wdgts->w_dlg_file_choose);

    return 0;
}

int on_menubar_new_activate(GtkWidget *menuitem, app_widgets *app_wdgts)
{
    app_wdgts->focus_draw = 0;
    app_wdgts->focus_fill = 0;
    app_wdgts->focus_erase = 0;

    app_wdgts->cr_action = 0;
    app_wdgts->thickness = 1;
    app_wdgts->insert_img_select = 0;

    app_wdgts->zoomed_w = 0;
    app_wdgts->zoomed_h = 0;

    app_wdgts->pattern_size = 0.3;
    app_wdgts->pattern_type = "square";

    GdkRGBA *color_set = malloc(sizeof(GdkRGBA));
    color_set->red = 0.0;
    color_set->green = 0.0;
    color_set->blue = 0.0;
    color_set->alpha = 1.0;
    gtk_color_chooser_set_rgba(GTK_COLOR_CHOOSER(app_wdgts->w_btn_color), color_set);
    free(color_set);

    printf("%ld\n", sizeof(menuitem));
    GdkPixbuf *new_img = gdk_pixbuf_new(GDK_COLORSPACE_RGB, TRUE, 8, 800, 800);
    gdk_pixbuf_fill(new_img, 0xffffffff);
    app_wdgts->tmp_img = new_img;

    // Save copies of the original files
    gdk_pixbuf_savev(
            app_wdgts->tmp_img, "cache/cp_img.png", "png", NULL, NULL, NULL);
    gdk_pixbuf_savev(
            app_wdgts->tmp_img, "cache/temp_img.png", "png", NULL, NULL, NULL);

    app_wdgts->gd_img = gdImageCreateFromFile("cache/cp_img.png");

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
    app_wdgts->zoomed_w = img_w;
    app_wdgts->zoomed_h = img_h;

    //init_color_array(app_wdgts);

    // Redraw the drawing area to display the image in the UI
    gtk_widget_set_size_request(app_wdgts->w_drawing_aera,
            app_wdgts->tmp_w, app_wdgts->tmp_h);
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

    while(app_wdgts->gd_list != NULL && app_wdgts->gd_list->next != NULL)
    {
        tmp = app_wdgts->gd_list;
        app_wdgts->gd_list = app_wdgts->gd_list->next;
        free(tmp);
    }      

    app_wdgts->gd_list = malloc(sizeof(struct gdImage_list));
    app_wdgts->gd_list->img = NULL;

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
    gtk_widget_set_sensitive(app_wdgts->w_check_weekdayfilter, TRUE);
    gtk_widget_set_sensitive(app_wdgts->w_check_vogue, TRUE);
    gtk_widget_set_sensitive(app_wdgts->w_check_winterfrost, TRUE);
    gtk_widget_set_sensitive(app_wdgts->w_check_timeoclockfilter, TRUE);
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
    gtk_widget_set_sensitive(app_wdgts->w_btn_circle, TRUE);
    gtk_widget_set_sensitive(app_wdgts->w_btn_square, TRUE);
    gtk_widget_set_sensitive(app_wdgts->w_btn_pentagon, TRUE);
    gtk_widget_set_sensitive(app_wdgts->w_btn_star, TRUE);
    gtk_widget_set_sensitive(app_wdgts->w_btn_heart, TRUE);
    gtk_widget_set_sensitive(app_wdgts->w_btn_triangle, TRUE);
    gtk_widget_set_sensitive(app_wdgts->w_entry_shapes, TRUE);      

    // Update preview
    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(
                app_wdgts->w_check_none), TRUE);
    update_all_prev(app_wdgts);

    insert_gdImage_list(app_wdgts);
    return 0;
}

void on_menubar_quit_activate(GtkWidget *menuitem, app_widgets *app_wdgts)
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

int on_scale_zoom_value_changed(GtkWidget *menuitem, app_widgets *app_wdgts)
{    
    printf("%ld\n", sizeof(menuitem));
    int value = gtk_range_get_value(GTK_RANGE(app_wdgts->w_scale_zoom));

    app_wdgts->zoom = value;

    app_wdgts->zoomed_w = app_wdgts->gd_w;
    app_wdgts->zoomed_h = app_wdgts->gd_h;

    if(value == 0)
        value = 1;    

    else if(value > 0)
    {
        app_wdgts->zoomed_w *= value;
        app_wdgts->zoomed_h *= value;
    }        
    else
    {
        value = -value;
        app_wdgts->zoomed_w /= value;
        app_wdgts->zoomed_h /= value;
    }

    app_wdgts->tmp_img = gdk_pixbuf_new_from_file("cache/cp_img.png", NULL);
    app_wdgts->tmp_img = gdk_pixbuf_scale_simple(app_wdgts->tmp_img,
            app_wdgts->zoomed_w, app_wdgts->zoomed_h, GDK_INTERP_NEAREST);
    gtk_widget_set_size_request(app_wdgts->w_drawing_aera,
            app_wdgts->zoomed_w, app_wdgts->zoomed_h);
    app_wdgts->cr_action = 0;
    gtk_widget_queue_draw(app_wdgts->w_drawing_aera);

    update_info_zoom(app_wdgts);

    return 0;
}

int on_btn_undo_activate(GtkWidget *menuitem, app_widgets *app_wdgts)
{
    printf("%ld\n", sizeof(menuitem));

    if(app_wdgts->gd_list->next != NULL && app_wdgts->gd_list->next->img != NULL)
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

int on_btn_help_activate(GtkWidget *menuitem)
{
        printf("%ld\n", sizeof(menuitem));
        system("sensible-browser https://github.com/Alb-310/Nebula-S4");
        return 0;
}
//

//#pragma  endregion Interface

//#pragma  region Drawing

//// Functions
static void draw_brush(GtkWidget *widget, gdouble x, gdouble y,
        app_widgets *app_wdgts)
{
    printf("%ld\n", sizeof(widget));

    int real_x = x;
    int real_y = y;

    update_xy(app_wdgts, &real_x, &real_y);

    if(app_wdgts->focus_erase)
    {
        struct Point *new_p = malloc(sizeof(struct Point));
        if(new_p == NULL) { printf("out of memory\n"); abort(); }
        new_p->x = real_x;
        new_p->y = real_y;
        new_p->next = NULL;
        p1->next = new_p;
        p1 = p1->next;
    }
    else if(app_wdgts->focus_draw){
        struct Point *new_p = malloc(sizeof(struct Point));
        struct Point *new_p2 = malloc(sizeof(struct Point));
        if(new_p == NULL || new_p2 == NULL) { printf("out of memory\n"); abort(); }
        new_p->x = real_x;
        new_p->y = real_y;
        new_p->next = NULL;
        new_p2->x = x;
        new_p2->y = y;
        new_p2->next = NULL;
        p1->next = new_p;
        p1 = p1->next;
        p2->next = new_p2;
        p2 = p2->next;
        //app_wdgts->draw_array[real_y * app_wdgts->gd_w + real_x] = 1;
        gtk_widget_queue_draw(app_wdgts->w_drawing_aera);
    }
}

int on_btn_color_color_set(GtkWidget *color_button, app_widgets *app_wdgts)
{
    GdkRGBA *color_set = malloc(sizeof(GdkRGBA));
    gtk_color_chooser_get_rgba(GTK_COLOR_CHOOSER(color_button), color_set);
    app_wdgts->color_get->r = color_set->red;
    app_wdgts->color_get->g = color_set->green;
    app_wdgts->color_get->b = color_set->blue;
    app_wdgts->color_get->a = color_set->alpha;

    app_wdgts->color->r = color_set->red * 255;
    app_wdgts->color->g = color_set->green * 255;
    app_wdgts->color->b = color_set->blue * 255;
    app_wdgts->color->a = (color_set->alpha * 127) * (-1);

    return 0;
}


int cairo_update_line_type(cairo_t *cr, app_widgets *app_wdgts)
{
    double thickness;   

    if(app_wdgts->zoom == 0)
        thickness = app_wdgts->thickness;     
    else if(app_wdgts->zoom > 0)
        thickness = (double) app_wdgts->thickness*app_wdgts->zoom;
    else
        thickness = (double) app_wdgts->thickness/-(app_wdgts->zoom);

    int brush_type = app_wdgts->brush_type;
    double type_modif = 0.0;
    int tks_modif = 0;

    switch(brush_type){
        case 2: type_modif = 0.7; break;
        case 3: type_modif = 0.5; tks_modif = 5; break;
        case 4: type_modif = 0.875; break;
        default: break;
    }
    app_wdgts->color_get->a -= type_modif;

    cairo_set_line_width(cr, thickness + tks_modif);

    cairo_set_source_rgba(cr, app_wdgts->color_get->r, 
            app_wdgts->color_get->g, 
            app_wdgts->color_get->b,
            app_wdgts->color_get->a);

    return thickness;
}

//// SignalsCallbackFunctions
int on_draw_event(GtkWidget *widget, cairo_t *cr, app_widgets *app_wdgts)
{   
    printf("%ld\n", sizeof(widget));
    if(app_wdgts->tmp_img != NULL)
    {  
        gdk_cairo_set_source_pixbuf(cr, app_wdgts->tmp_img, 0, 0);
        cairo_paint(cr);

        on_btn_color_color_set(app_wdgts->w_btn_color, app_wdgts);

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
            gtk_widget_set_sensitive(app_wdgts->w_check_none, FALSE);
            gtk_widget_set_sensitive(app_wdgts->w_check_oldschool, FALSE);
            gtk_widget_set_sensitive(app_wdgts->w_check_glowfilter,FALSE);
            gtk_widget_set_sensitive(app_wdgts->w_check_redflagfilter, FALSE);
            gtk_widget_set_sensitive(app_wdgts->w_check_summertimefilter,FALSE);
            gtk_widget_set_sensitive(app_wdgts->w_check_weekdayfilter,FALSE);
            gtk_widget_set_sensitive(app_wdgts->w_check_vogue,FALSE);
            gtk_widget_set_sensitive(app_wdgts->w_check_winterfrost,FALSE);
            gtk_widget_set_sensitive(app_wdgts->w_check_timeoclockfilter,FALSE);
            gtk_widget_set_sensitive(app_wdgts->w_btn_crop, FALSE);
            gtk_widget_set_sensitive(app_wdgts->w_btn_resize, FALSE);
            gtk_widget_set_sensitive(app_wdgts->w_entry_rotation, FALSE);
            gtk_widget_set_sensitive(app_wdgts->w_scale_rotation, FALSE); 
            gtk_widget_set_sensitive(app_wdgts->w_scale_zoom, FALSE);

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
                            "cache/temp_img.png");
                }
                else{
                    line_to(app_wdgts->gd_img, app_wdgts->gd_out,
                            (void *)start->next, (void *)app_wdgts->color,
                            app_wdgts->thickness,
                            app_wdgts->brush_type, "cache/temp_img.png");

                    if(start2 != NULL && start2->next != NULL)
                        free_point_struct(start2->next);
                }
                if(start != NULL && start->next != NULL)
                    free_point_struct(start->next);
            }

            app_wdgts->tmp_img = gdk_pixbuf_new_from_file("cache/temp_img.png", NULL);
            update_buffer(app_wdgts);
        }
        else if(app_wdgts->focus_draw && start2->next != NULL){

            struct Point *point = start2->next;
            struct Point *tmp;

            int thickness;

            thickness = cairo_update_line_type(cr, app_wdgts);

            while (point != NULL)
            {
                if(point->next == NULL)
                    break;  
                tmp = point;
                point = point->next;

                if(app_wdgts->brush_type != 4){
                    cairo_move_to(cr, (double)tmp->x, (double)tmp->y);
                    cairo_line_to(cr, (double)point->x, (double)point->y);
                }
                else{
                    cairo_arc(cr, (double)tmp->x, (double)tmp->y, 
                            thickness * 3, 0, 2 * 2*M_PI);
                    cairo_stroke_preserve(cr);
                    cairo_fill(cr);
                }

                cairo_stroke(cr);  
            }

        }
        app_wdgts->cr_action = 0;
    }

    return 0;
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

    if (app_wdgts->insert_img_select)
    {
        int real_x =  event->x;
        int real_y = event->y;
        update_xy(app_wdgts, &real_x, &real_y);
        picture_insertion(app_wdgts->insert_img, app_wdgts->gd_out,
                app_wdgts->gd_img, "cache/temp_img.png",
                real_x, real_y, 0.25);
        modif = 1;
    }
    else if(app_wdgts->focus_erase){
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
        int real_x =  event->x;
        int real_y = event->y;
        update_xy(app_wdgts, &real_x, &real_y);
        fill(app_wdgts->gd_img, app_wdgts->gd_out, (void*)info_src, (void*)app_wdgts->color,
                real_x, real_y, "cache/temp_img.png");
        free(info_src);
        modif = 1;
    }

    else if(app_wdgts->focus_text){
        int real_x =  event->x;
        int real_y = event->y;
        update_xy(app_wdgts, &real_x, &real_y);
        app_wdgts->tmp_x = real_x;
        app_wdgts->tmp_y = real_y;
        init_pop_up_text(app_wdgts);
        gtk_widget_show(app_wdgts->w_pop_up_text);
    }

    else if(app_wdgts->focus_motif){

        int real_x =  event->x;
        int real_y = event->y;
        update_xy(app_wdgts, &real_x, &real_y);
        printf("pattern type: %s, pattern size: %f\n", app_wdgts->pattern_type, app_wdgts->pattern_size);
        add_motif(app_wdgts->gd_img, app_wdgts->gd_out, "cache/temp_img.png", app_wdgts->pattern_type, 
                real_x, real_y, app_wdgts->pattern_size);
        modif = 1;
    }

    else if(app_wdgts->focus_draw){
        int real_x =  event->x;
        int real_y = event->y;
        update_xy(app_wdgts, &real_x, &real_y);
        set_pixel(app_wdgts->gd_img, app_wdgts->gd_out, real_x, real_y,
                (void*)app_wdgts->color,
                app_wdgts->thickness,
                app_wdgts->brush_type, "cache/temp_img.png");

        modif = 1;
    }

    if(modif){
        app_wdgts->cr_action = 1;
        gtk_widget_queue_draw(app_wdgts->w_drawing_aera);
    }

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
    return 0;
}

int on_btn_wipe_focus_out_event(GtkWidget *btn_fill, GdkEventMotion *event,
        app_widgets *app_wdgts)
{
    printf("%ld\n", sizeof(btn_fill));
    printf("%ld\n", sizeof(event));
    printf("%ld\n", sizeof(app_wdgts));
    return 0;
}
//

//#pragma  endregion Drawing

//#pragma  region ImageProcessing

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
    gtk_widget_set_sensitive(app_wdgts->w_check_vogue, FALSE);
    gtk_widget_set_sensitive(app_wdgts->w_check_winterfrost, FALSE);
    gtk_widget_set_sensitive(app_wdgts->w_check_timeoclockfilter, FALSE);
    gtk_widget_set_sensitive(app_wdgts->w_check_weekdayfilter, FALSE);
    gtk_widget_set_sensitive(app_wdgts->w_btn_crop, FALSE);
    gtk_widget_set_sensitive(app_wdgts->w_btn_resize, FALSE);
    gtk_widget_set_sensitive(app_wdgts->w_entry_rotation, FALSE);
    gtk_widget_set_sensitive(app_wdgts->w_scale_rotation, FALSE);
    gtk_widget_set_sensitive(app_wdgts->w_cb_brush, FALSE);
    gtk_widget_set_sensitive(app_wdgts->w_cb_thickness, FALSE);
    gtk_widget_set_sensitive(app_wdgts->w_btn_color, FALSE);
    gtk_widget_set_sensitive(app_wdgts->w_btn_fill, FALSE);
    gtk_widget_set_sensitive(app_wdgts->w_btn_text, FALSE);
    gtk_widget_set_sensitive(app_wdgts->w_btn_apply, FALSE);
    gtk_widget_set_sensitive(app_wdgts->w_btn_circle, FALSE);
    gtk_widget_set_sensitive(app_wdgts->w_btn_pentagon, FALSE);
    gtk_widget_set_sensitive(app_wdgts->w_btn_square, FALSE);
    gtk_widget_set_sensitive(app_wdgts->w_btn_star, FALSE);
    gtk_widget_set_sensitive(app_wdgts->w_btn_heart, FALSE);
    gtk_widget_set_sensitive(app_wdgts->w_btn_triangle, FALSE);
    gtk_widget_set_sensitive(app_wdgts->w_entry_shapes, FALSE);
    gtk_widget_set_sensitive(app_wdgts->w_btn_erase, FALSE);
    gtk_widget_set_sensitive(app_wdgts->w_scale_zoom, FALSE);
    gtk_widget_set_sensitive(app_wdgts->w_btn_apply, TRUE);

    gtk_toggle_button_set_active(
            GTK_TOGGLE_BUTTON(app_wdgts->w_check_oldschool), FALSE);
    gtk_toggle_button_set_active(
            GTK_TOGGLE_BUTTON(app_wdgts->w_check_glowfilter),FALSE);
    gtk_toggle_button_set_active(
            GTK_TOGGLE_BUTTON(app_wdgts->w_check_redflagfilter), FALSE);
    gtk_toggle_button_set_active(
            GTK_TOGGLE_BUTTON(app_wdgts->w_check_summertimefilter), FALSE);
    gtk_toggle_button_set_active(
            GTK_TOGGLE_BUTTON(app_wdgts->w_check_weekdayfilter), FALSE);
    gtk_toggle_button_set_active(
            GTK_TOGGLE_BUTTON(app_wdgts->w_check_vogue), FALSE);
    gtk_toggle_button_set_active(
            GTK_TOGGLE_BUTTON(app_wdgts->w_check_winterfrost), FALSE);
    gtk_toggle_button_set_active(
            GTK_TOGGLE_BUTTON(app_wdgts->w_check_timeoclockfilter), FALSE);

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
    gtk_widget_set_sensitive(app_wdgts->w_check_weekdayfilter, FALSE);
    gtk_widget_set_sensitive(app_wdgts->w_check_vogue, FALSE);
    gtk_widget_set_sensitive(app_wdgts->w_check_winterfrost, FALSE);
    gtk_widget_set_sensitive(app_wdgts->w_check_timeoclockfilter, FALSE);
    gtk_widget_set_sensitive(app_wdgts->w_btn_crop, FALSE);
    gtk_widget_set_sensitive(app_wdgts->w_btn_resize, FALSE);
    gtk_widget_set_sensitive(app_wdgts->w_entry_rotation, FALSE);
    gtk_widget_set_sensitive(app_wdgts->w_scale_rotation, FALSE);
    gtk_widget_set_sensitive(app_wdgts->w_cb_brush, FALSE);
    gtk_widget_set_sensitive(app_wdgts->w_cb_thickness, FALSE);
    gtk_widget_set_sensitive(app_wdgts->w_btn_color, FALSE);
    gtk_widget_set_sensitive(app_wdgts->w_btn_fill, FALSE);
    gtk_widget_set_sensitive(app_wdgts->w_btn_text, FALSE);
    gtk_widget_set_sensitive(app_wdgts->w_btn_circle, FALSE);
    gtk_widget_set_sensitive(app_wdgts->w_btn_pentagon, FALSE);
    gtk_widget_set_sensitive(app_wdgts->w_btn_square, FALSE);
    gtk_widget_set_sensitive(app_wdgts->w_btn_star, FALSE);
    gtk_widget_set_sensitive(app_wdgts->w_btn_heart, FALSE);
    gtk_widget_set_sensitive(app_wdgts->w_btn_triangle, FALSE);
    gtk_widget_set_sensitive(app_wdgts->w_entry_shapes, FALSE);
    gtk_widget_set_sensitive(app_wdgts->w_btn_erase, FALSE);
    gtk_widget_set_sensitive(app_wdgts->w_scale_zoom, FALSE);
    gtk_widget_set_sensitive(app_wdgts->w_btn_apply, TRUE);

    gtk_toggle_button_set_active(
            GTK_TOGGLE_BUTTON(app_wdgts->w_check_oldschool), FALSE);
    gtk_toggle_button_set_active(
            GTK_TOGGLE_BUTTON(app_wdgts->w_check_glowfilter),FALSE);
    gtk_toggle_button_set_active(
            GTK_TOGGLE_BUTTON(app_wdgts->w_check_redflagfilter), FALSE);
    gtk_toggle_button_set_active(
            GTK_TOGGLE_BUTTON(app_wdgts->w_check_summertimefilter), FALSE);
    gtk_toggle_button_set_active(
            GTK_TOGGLE_BUTTON(app_wdgts->w_check_weekdayfilter), FALSE);
    gtk_toggle_button_set_active(
            GTK_TOGGLE_BUTTON(app_wdgts->w_check_vogue), FALSE);
    gtk_toggle_button_set_active(
            GTK_TOGGLE_BUTTON(app_wdgts->w_check_winterfrost), FALSE);
    gtk_toggle_button_set_active(
            GTK_TOGGLE_BUTTON(app_wdgts->w_check_timeoclockfilter), FALSE);

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
    gtk_widget_set_sensitive(app_wdgts->w_check_weekdayfilter, FALSE);
    gtk_widget_set_sensitive(app_wdgts->w_check_vogue, FALSE);
    gtk_widget_set_sensitive(app_wdgts->w_check_winterfrost, FALSE);
    gtk_widget_set_sensitive(app_wdgts->w_check_timeoclockfilter, FALSE);
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
    gtk_widget_set_sensitive(app_wdgts->w_btn_circle, FALSE);
    gtk_widget_set_sensitive(app_wdgts->w_btn_pentagon, FALSE);
    gtk_widget_set_sensitive(app_wdgts->w_btn_square, FALSE);
    gtk_widget_set_sensitive(app_wdgts->w_btn_star, FALSE);
    gtk_widget_set_sensitive(app_wdgts->w_btn_heart, FALSE);
    gtk_widget_set_sensitive(app_wdgts->w_btn_triangle, FALSE);
    gtk_widget_set_sensitive(app_wdgts->w_entry_shapes, FALSE);
    gtk_widget_set_sensitive(app_wdgts->w_btn_erase, FALSE);
    gtk_widget_set_sensitive(app_wdgts->w_scale_zoom, FALSE);
    gtk_widget_set_sensitive(app_wdgts->w_btn_apply, TRUE);

    gtk_toggle_button_set_active(
            GTK_TOGGLE_BUTTON(app_wdgts->w_check_oldschool), FALSE);
    gtk_toggle_button_set_active(
            GTK_TOGGLE_BUTTON(app_wdgts->w_check_glowfilter),FALSE);
    gtk_toggle_button_set_active(
            GTK_TOGGLE_BUTTON(app_wdgts->w_check_redflagfilter), FALSE);
    gtk_toggle_button_set_active(
            GTK_TOGGLE_BUTTON(app_wdgts->w_check_summertimefilter), FALSE);
    gtk_toggle_button_set_active(
            GTK_TOGGLE_BUTTON(app_wdgts->w_check_weekdayfilter), FALSE);
    gtk_toggle_button_set_active(
            GTK_TOGGLE_BUTTON(app_wdgts->w_check_vogue), FALSE);
    gtk_toggle_button_set_active(
            GTK_TOGGLE_BUTTON(app_wdgts->w_check_winterfrost), FALSE);
    gtk_toggle_button_set_active(
            GTK_TOGGLE_BUTTON(app_wdgts->w_check_timeoclockfilter), FALSE);

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
    gtk_widget_set_sensitive(app_wdgts->w_check_weekdayfilter, FALSE);
    gtk_widget_set_sensitive(app_wdgts->w_check_vogue, FALSE);
    gtk_widget_set_sensitive(app_wdgts->w_check_winterfrost, FALSE);
    gtk_widget_set_sensitive(app_wdgts->w_check_timeoclockfilter, FALSE);
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
    gtk_widget_set_sensitive(app_wdgts->w_btn_circle, FALSE);
    gtk_widget_set_sensitive(app_wdgts->w_btn_pentagon, FALSE);
    gtk_widget_set_sensitive(app_wdgts->w_btn_square, FALSE);
    gtk_widget_set_sensitive(app_wdgts->w_btn_star, FALSE);
    gtk_widget_set_sensitive(app_wdgts->w_btn_heart, FALSE);
    gtk_widget_set_sensitive(app_wdgts->w_btn_triangle, FALSE);
    gtk_widget_set_sensitive(app_wdgts->w_entry_shapes, FALSE);
    gtk_widget_set_sensitive(app_wdgts->w_btn_erase, FALSE);
    gtk_widget_set_sensitive(app_wdgts->w_scale_zoom, FALSE);
    gtk_widget_set_sensitive(app_wdgts->w_btn_apply, TRUE);

    gtk_toggle_button_set_active(
            GTK_TOGGLE_BUTTON(app_wdgts->w_check_oldschool), FALSE);
    gtk_toggle_button_set_active(
            GTK_TOGGLE_BUTTON(app_wdgts->w_check_glowfilter),FALSE);
    gtk_toggle_button_set_active(
            GTK_TOGGLE_BUTTON(app_wdgts->w_check_redflagfilter), FALSE);
    gtk_toggle_button_set_active(
            GTK_TOGGLE_BUTTON(app_wdgts->w_check_summertimefilter), FALSE);
    gtk_toggle_button_set_active(
            GTK_TOGGLE_BUTTON(app_wdgts->w_check_weekdayfilter), FALSE);
    gtk_toggle_button_set_active(
            GTK_TOGGLE_BUTTON(app_wdgts->w_check_vogue), FALSE);
    gtk_toggle_button_set_active(
            GTK_TOGGLE_BUTTON(app_wdgts->w_check_winterfrost), FALSE);
    gtk_toggle_button_set_active(
            GTK_TOGGLE_BUTTON(app_wdgts->w_check_timeoclockfilter), FALSE);

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
    gtk_widget_set_sensitive(app_wdgts->w_check_weekdayfilter, FALSE);
    gtk_widget_set_sensitive(app_wdgts->w_check_vogue, FALSE);
    gtk_widget_set_sensitive(app_wdgts->w_check_winterfrost, FALSE);
    gtk_widget_set_sensitive(app_wdgts->w_check_timeoclockfilter, FALSE);
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
    gtk_widget_set_sensitive(app_wdgts->w_btn_circle, FALSE);
    gtk_widget_set_sensitive(app_wdgts->w_btn_pentagon, FALSE);
    gtk_widget_set_sensitive(app_wdgts->w_btn_square, FALSE);
    gtk_widget_set_sensitive(app_wdgts->w_btn_star, FALSE);
    gtk_widget_set_sensitive(app_wdgts->w_btn_heart, FALSE);
    gtk_widget_set_sensitive(app_wdgts->w_btn_triangle, FALSE);
    gtk_widget_set_sensitive(app_wdgts->w_entry_shapes, FALSE);
    gtk_widget_set_sensitive(app_wdgts->w_btn_erase, FALSE);
    gtk_widget_set_sensitive(app_wdgts->w_scale_zoom, FALSE);
    gtk_widget_set_sensitive(app_wdgts->w_btn_apply, TRUE);

    gtk_toggle_button_set_active(
            GTK_TOGGLE_BUTTON(app_wdgts->w_check_oldschool), FALSE);
    gtk_toggle_button_set_active(
            GTK_TOGGLE_BUTTON(app_wdgts->w_check_glowfilter),FALSE);
    gtk_toggle_button_set_active(
            GTK_TOGGLE_BUTTON(app_wdgts->w_check_redflagfilter), FALSE);
    gtk_toggle_button_set_active(
            GTK_TOGGLE_BUTTON(app_wdgts->w_check_summertimefilter), FALSE);
    gtk_toggle_button_set_active(
            GTK_TOGGLE_BUTTON(app_wdgts->w_check_weekdayfilter), FALSE);
    gtk_toggle_button_set_active(
            GTK_TOGGLE_BUTTON(app_wdgts->w_check_vogue), FALSE);
    gtk_toggle_button_set_active(
            GTK_TOGGLE_BUTTON(app_wdgts->w_check_winterfrost), FALSE);
    gtk_toggle_button_set_active(
            GTK_TOGGLE_BUTTON(app_wdgts->w_check_timeoclockfilter), FALSE);

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

int on_btn_apply_clicked(GtkWidget *menuitem, app_widgets *app_wdgts)
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
    gtk_widget_set_sensitive(app_wdgts->w_check_weekdayfilter, TRUE);
    gtk_widget_set_sensitive(app_wdgts->w_check_vogue, TRUE);
    gtk_widget_set_sensitive(app_wdgts->w_check_winterfrost, TRUE);    
    gtk_widget_set_sensitive(app_wdgts->w_check_timeoclockfilter, TRUE);   
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
    gtk_toggle_button_set_active(
            GTK_TOGGLE_BUTTON(app_wdgts->w_check_weekdayfilter), FALSE);
    gtk_toggle_button_set_active(
            GTK_TOGGLE_BUTTON(app_wdgts->w_check_vogue), FALSE);
    gtk_toggle_button_set_active(
            GTK_TOGGLE_BUTTON(app_wdgts->w_check_winterfrost), FALSE);
    gtk_toggle_button_set_active(
            GTK_TOGGLE_BUTTON(app_wdgts->w_check_timeoclockfilter), FALSE);

    app_wdgts->tmp_img = gdk_pixbuf_scale_simple(app_wdgts->tmp_img, app_wdgts->tmp_w,
            app_wdgts->tmp_h, GDK_INTERP_NEAREST);

    gdk_pixbuf_savev(app_wdgts->tmp_img,
            "cache/cp_img.png", "png", NULL, NULL, NULL);
    app_wdgts->gd_img = gdImageCreateFromFile("cache/cp_img.png");
    app_wdgts->gd_w = app_wdgts->tmp_w;
    app_wdgts->gd_h = app_wdgts->tmp_h;
    app_wdgts->insert_img_select = 0;
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
                GTK_TOGGLE_BUTTON(app_wdgts->w_check_weekdayfilter), FALSE);
        gtk_toggle_button_set_active(
                GTK_TOGGLE_BUTTON(app_wdgts->w_check_vogue), FALSE);
        gtk_toggle_button_set_active(
                GTK_TOGGLE_BUTTON(app_wdgts->w_check_winterfrost), FALSE);
        gtk_toggle_button_set_active(
                GTK_TOGGLE_BUTTON(app_wdgts->w_check_timeoclockfilter), FALSE);

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
        gtk_widget_set_sensitive(app_wdgts->w_btn_circle, FALSE);
        gtk_widget_set_sensitive(app_wdgts->w_btn_pentagon, FALSE);
        gtk_widget_set_sensitive(app_wdgts->w_btn_square, FALSE);
        gtk_widget_set_sensitive(app_wdgts->w_btn_star, FALSE);
        gtk_widget_set_sensitive(app_wdgts->w_btn_heart, FALSE);
        gtk_widget_set_sensitive(app_wdgts->w_btn_triangle, FALSE);
        gtk_widget_set_sensitive(app_wdgts->w_entry_shapes, FALSE);
        gtk_widget_set_sensitive(app_wdgts->w_btn_erase, FALSE);
        gtk_widget_set_sensitive(app_wdgts->w_scale_zoom, FALSE);
        gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(
                    app_wdgts->w_check_none), FALSE);
        gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(
                    app_wdgts->w_check_glowfilter),FALSE);
        gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(
                    app_wdgts->w_check_redflagfilter), FALSE);
        gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(
                    app_wdgts->w_check_summertimefilter), FALSE);
        gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(
                    app_wdgts->w_check_weekdayfilter), FALSE);
        gtk_toggle_button_set_active(
                GTK_TOGGLE_BUTTON(app_wdgts->w_check_vogue), FALSE);
        gtk_toggle_button_set_active(
                GTK_TOGGLE_BUTTON(app_wdgts->w_check_winterfrost), FALSE);
        gtk_toggle_button_set_active(
                GTK_TOGGLE_BUTTON(app_wdgts->w_check_timeoclockfilter), FALSE);
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
        gtk_widget_set_sensitive(app_wdgts->w_btn_circle, FALSE);
        gtk_widget_set_sensitive(app_wdgts->w_btn_pentagon, FALSE);
        gtk_widget_set_sensitive(app_wdgts->w_btn_square, FALSE);
        gtk_widget_set_sensitive(app_wdgts->w_btn_star, FALSE);
        gtk_widget_set_sensitive(app_wdgts->w_btn_heart, FALSE);
        gtk_widget_set_sensitive(app_wdgts->w_btn_triangle, FALSE);
        gtk_widget_set_sensitive(app_wdgts->w_entry_shapes, FALSE);
        gtk_widget_set_sensitive(app_wdgts->w_scale_zoom, FALSE);
        gtk_widget_set_sensitive(app_wdgts->w_btn_erase, FALSE);
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
        gtk_toggle_button_set_active(
                GTK_TOGGLE_BUTTON(app_wdgts->w_check_winterfrost), FALSE);
        gtk_toggle_button_set_active(
                GTK_TOGGLE_BUTTON(app_wdgts->w_check_timeoclockfilter), FALSE);
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
        gtk_widget_set_sensitive(app_wdgts->w_btn_circle, FALSE);
        gtk_widget_set_sensitive(app_wdgts->w_btn_pentagon, FALSE);
        gtk_widget_set_sensitive(app_wdgts->w_btn_square, FALSE);
        gtk_widget_set_sensitive(app_wdgts->w_btn_star, FALSE);
        gtk_widget_set_sensitive(app_wdgts->w_btn_heart, FALSE);
        gtk_widget_set_sensitive(app_wdgts->w_btn_triangle, FALSE);
        gtk_widget_set_sensitive(app_wdgts->w_entry_shapes, FALSE);
        gtk_widget_set_sensitive(app_wdgts->w_btn_erase, FALSE);
        gtk_widget_set_sensitive(app_wdgts->w_scale_zoom, FALSE);
        gtk_toggle_button_set_active(
                GTK_TOGGLE_BUTTON(app_wdgts->w_check_none), FALSE);
        gtk_toggle_button_set_active(
                GTK_TOGGLE_BUTTON(app_wdgts->w_check_oldschool), FALSE);
        gtk_toggle_button_set_active(
                GTK_TOGGLE_BUTTON(app_wdgts->w_check_redflagfilter), FALSE);
        gtk_toggle_button_set_active(
                GTK_TOGGLE_BUTTON(app_wdgts->w_check_summertimefilter), FALSE);
        gtk_toggle_button_set_active(
                GTK_TOGGLE_BUTTON(app_wdgts->w_check_weekdayfilter), FALSE);
        gtk_toggle_button_set_active(
                GTK_TOGGLE_BUTTON(app_wdgts->w_check_vogue), FALSE);
        gtk_toggle_button_set_active(
                GTK_TOGGLE_BUTTON(app_wdgts->w_check_winterfrost), FALSE);
        gtk_toggle_button_set_active(
                GTK_TOGGLE_BUTTON(app_wdgts->w_check_timeoclockfilter), FALSE);
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
        gtk_widget_set_sensitive(app_wdgts->w_btn_circle, FALSE);
        gtk_widget_set_sensitive(app_wdgts->w_btn_pentagon, FALSE);
        gtk_widget_set_sensitive(app_wdgts->w_btn_square, FALSE);
        gtk_widget_set_sensitive(app_wdgts->w_btn_star, FALSE);
        gtk_widget_set_sensitive(app_wdgts->w_btn_heart, FALSE);
        gtk_widget_set_sensitive(app_wdgts->w_btn_triangle, FALSE);
        gtk_widget_set_sensitive(app_wdgts->w_entry_shapes, FALSE);
        gtk_widget_set_sensitive(app_wdgts->w_btn_erase, FALSE);
        gtk_widget_set_sensitive(app_wdgts->w_scale_zoom, FALSE);
        gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(
                    app_wdgts->w_check_none), FALSE);
        gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(
                    app_wdgts->w_check_oldschool), FALSE);
        gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(
                    app_wdgts->w_check_glowfilter),FALSE);
        gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(
                    app_wdgts->w_check_summertimefilter), FALSE);
        gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(
                    app_wdgts->w_check_weekdayfilter), FALSE);
        gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(
                    app_wdgts->w_check_vogue), FALSE);
        gtk_toggle_button_set_active(
                GTK_TOGGLE_BUTTON(app_wdgts->w_check_winterfrost), FALSE);
        gtk_toggle_button_set_active(
                GTK_TOGGLE_BUTTON(app_wdgts->w_check_timeoclockfilter), FALSE);
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
        gtk_widget_set_sensitive(app_wdgts->w_btn_circle, FALSE);
        gtk_widget_set_sensitive(app_wdgts->w_btn_pentagon, FALSE);
        gtk_widget_set_sensitive(app_wdgts->w_btn_square, FALSE);
        gtk_widget_set_sensitive(app_wdgts->w_btn_star, FALSE);
        gtk_widget_set_sensitive(app_wdgts->w_btn_heart, FALSE);
        gtk_widget_set_sensitive(app_wdgts->w_btn_triangle, FALSE);
        gtk_widget_set_sensitive(app_wdgts->w_entry_shapes, FALSE);
        gtk_widget_set_sensitive(app_wdgts->w_btn_erase, FALSE);
        gtk_widget_set_sensitive(app_wdgts->w_scale_zoom, FALSE);
        gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(
                    app_wdgts->w_check_none), FALSE);
        gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(
                    app_wdgts->w_check_oldschool), FALSE);
        gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(
                    app_wdgts->w_check_glowfilter),FALSE);
        gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(
                    app_wdgts->w_check_summertimefilter), FALSE);
        gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(
                    app_wdgts->w_check_weekdayfilter), FALSE);
        gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(
                    app_wdgts->w_check_redflagfilter), FALSE);
        gtk_toggle_button_set_active(
                GTK_TOGGLE_BUTTON(app_wdgts->w_check_winterfrost), FALSE);
        gtk_toggle_button_set_active(
                GTK_TOGGLE_BUTTON(app_wdgts->w_check_timeoclockfilter), FALSE);
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

int on_check_winterfrost_toggled(GtkToggleButton *togglebutton,
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
        gtk_widget_set_sensitive(app_wdgts->w_btn_circle, FALSE);
        gtk_widget_set_sensitive(app_wdgts->w_btn_pentagon, FALSE);
        gtk_widget_set_sensitive(app_wdgts->w_btn_square, FALSE);
        gtk_widget_set_sensitive(app_wdgts->w_btn_star, FALSE);
        gtk_widget_set_sensitive(app_wdgts->w_btn_heart, FALSE);
        gtk_widget_set_sensitive(app_wdgts->w_btn_triangle, FALSE);
        gtk_widget_set_sensitive(app_wdgts->w_entry_shapes, FALSE);
        gtk_widget_set_sensitive(app_wdgts->w_btn_erase, FALSE);
        gtk_widget_set_sensitive(app_wdgts->w_scale_zoom, FALSE);
        gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(
                    app_wdgts->w_check_none), FALSE);
        gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(
                    app_wdgts->w_check_oldschool), FALSE);
        gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(
                    app_wdgts->w_check_glowfilter),FALSE);
        gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(
                    app_wdgts->w_check_summertimefilter), FALSE);
        gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(
                    app_wdgts->w_check_weekdayfilter), FALSE);
        gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(
                    app_wdgts->w_check_redflagfilter), FALSE);
        gtk_toggle_button_set_active(
                GTK_TOGGLE_BUTTON(app_wdgts->w_check_vogue), FALSE);
        gtk_toggle_button_set_active(
                GTK_TOGGLE_BUTTON(app_wdgts->w_check_timeoclockfilter), FALSE);
        gtk_widget_set_sensitive(app_wdgts->w_btn_apply, TRUE);

        app_wdgts->gd_img = gdImageCreateFromFile("cache/cp_img.png");
        WinterFrost_Filter(app_wdgts->gd_img,
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

int on_check_timeoclock_toggled(GtkToggleButton *togglebutton,
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
        gtk_widget_set_sensitive(app_wdgts->w_btn_circle, FALSE);
        gtk_widget_set_sensitive(app_wdgts->w_btn_pentagon, FALSE);
        gtk_widget_set_sensitive(app_wdgts->w_btn_square, FALSE);
        gtk_widget_set_sensitive(app_wdgts->w_btn_star, FALSE);
        gtk_widget_set_sensitive(app_wdgts->w_btn_heart, FALSE);
        gtk_widget_set_sensitive(app_wdgts->w_btn_triangle, FALSE);
        gtk_widget_set_sensitive(app_wdgts->w_entry_shapes, FALSE);
        gtk_widget_set_sensitive(app_wdgts->w_btn_erase, FALSE);
        gtk_widget_set_sensitive(app_wdgts->w_scale_zoom, FALSE);
        gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(
                    app_wdgts->w_check_none), FALSE);
        gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(
                    app_wdgts->w_check_oldschool), FALSE);
        gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(
                    app_wdgts->w_check_glowfilter),FALSE);
        gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(
                    app_wdgts->w_check_summertimefilter), FALSE);
        gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(
                    app_wdgts->w_check_weekdayfilter), FALSE);
        gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(
                    app_wdgts->w_check_redflagfilter), FALSE);
        gtk_toggle_button_set_active(
                GTK_TOGGLE_BUTTON(app_wdgts->w_check_vogue), FALSE);

        gtk_toggle_button_set_active(
                GTK_TOGGLE_BUTTON(app_wdgts->w_check_winterfrost), FALSE);

        gtk_widget_set_sensitive(app_wdgts->w_btn_apply, TRUE);

        app_wdgts->gd_img = gdImageCreateFromFile("cache/cp_img.png");
        TimeOClock_Filter(app_wdgts->gd_img,
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

int on_check_weekdayfilter_toggled(GtkToggleButton *togglebutton,
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
        gtk_widget_set_sensitive(app_wdgts->w_btn_circle, FALSE);
        gtk_widget_set_sensitive(app_wdgts->w_btn_pentagon, FALSE);
        gtk_widget_set_sensitive(app_wdgts->w_btn_square, FALSE);
        gtk_widget_set_sensitive(app_wdgts->w_btn_star, FALSE);
        gtk_widget_set_sensitive(app_wdgts->w_btn_heart, FALSE);
        gtk_widget_set_sensitive(app_wdgts->w_btn_triangle, FALSE);
        gtk_widget_set_sensitive(app_wdgts->w_entry_shapes, FALSE);
        gtk_widget_set_sensitive(app_wdgts->w_btn_erase, FALSE);
        gtk_widget_set_sensitive(app_wdgts->w_scale_zoom, FALSE);
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
        gtk_toggle_button_set_active(
                GTK_TOGGLE_BUTTON(app_wdgts->w_check_vogue), FALSE);
        gtk_toggle_button_set_active(
                GTK_TOGGLE_BUTTON(app_wdgts->w_check_winterfrost), FALSE);
        gtk_toggle_button_set_active(
                GTK_TOGGLE_BUTTON(app_wdgts->w_check_timeoclockfilter), FALSE);

        gtk_widget_set_sensitive(app_wdgts->w_btn_apply, TRUE);

        app_wdgts->gd_img = gdImageCreateFromFile("cache/cp_img.png");
        WeekDay_Filter(app_wdgts->gd_img,
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

//#pragma  endregion ImageProcessing

//#pragma  region OthersFunctions

//// Functions
int init_collage_interface(app_widgets *app_wdgts)
{       
    GtkWidget *prev_list[] = {
        app_wdgts->w_prev_img1,
        app_wdgts->w_prev_img2,
        app_wdgts->w_prev_img3,
        app_wdgts->w_prev_img4,
    };

    for(int i = 0; i < 4; i++)
    {
        gtk_image_set_from_icon_name(GTK_IMAGE(prev_list[i]), "unknown", GTK_ICON_SIZE_INVALID);
    }

    gtk_image_set_from_icon_name(GTK_IMAGE(app_wdgts->w_prev_final), "unknown", GTK_ICON_SIZE_INVALID);

    gtk_widget_set_sensitive(app_wdgts->w_btn_collage_delete1, FALSE);
    gtk_widget_set_sensitive(app_wdgts->w_btn_collage_delete2, FALSE);
    gtk_widget_set_sensitive(app_wdgts->w_btn_collage_delete3, FALSE);
    gtk_widget_set_sensitive(app_wdgts->w_btn_collage_delete4, FALSE);
    

    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(
                app_wdgts->w_check_preset1), FALSE);
    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(
                app_wdgts->w_check_preset2), FALSE);
    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(
                app_wdgts->w_check_preset3), FALSE);

    app_wdgts->img1_path = "";
    app_wdgts->img2_path = "";
    app_wdgts->img3_path = "";
    app_wdgts->img4_path = "";

    app_wdgts->img_count = 0;

    return 0;
}

int update_all_presets(app_widgets *app_wdgts, int img_count)
{
    float coef = 5.125;

    char* two_list[] = {
        "src/resources/func_divers/2_pic_collage_1.png",
        "src/resources/func_divers/2_pic_collage_2.png",
    };

    char* three_list[] = {        
        "src/resources/func_divers/3_pic_collage_1.png",
        "src/resources/func_divers/3_pic_collage_2.png",
        "src/resources/func_divers/3_pic_collage_3.png",
    };

    char* four_list[] = {        
        "src/resources/func_divers/4_pic_collage_1.png",
        "src/resources/func_divers/4_pic_collage_2.png",
        "src/resources/func_divers/4_pic_collage_3.png",
    };

    GtkWidget* prev_list[] = {
        app_wdgts->w_prev_preset1,
        app_wdgts->w_prev_preset2,
        app_wdgts->w_prev_preset3,
    };

    GtkWidget* check_list[] = {
        app_wdgts->w_check_preset1,
        app_wdgts->w_check_preset2,
        app_wdgts->w_check_preset3,
    };

    for(int i = 0; i < 3; i++)
    {
        gtk_image_set_from_icon_name(GTK_IMAGE(prev_list[i]), "unknown", GTK_ICON_SIZE_INVALID);
        gtk_widget_set_sensitive(check_list[i], FALSE);
    }

    GdkPixbuf *prev;
    if (img_count > 1){
        for (int i = 0; i < img_count; i++)
        {
            if(i < 3){
                if(img_count == 2)
                    prev = gdk_pixbuf_new_from_file(two_list[i], NULL);
                else if(img_count == 3)
                    prev = gdk_pixbuf_new_from_file(three_list[i], NULL);
                else
                    prev = gdk_pixbuf_new_from_file(four_list[i], NULL);

                prev = gdk_pixbuf_scale_simple(prev, gdk_pixbuf_get_width(prev)/coef,
                        gdk_pixbuf_get_height(prev)/coef, GDK_INTERP_NEAREST);
                gtk_image_set_from_pixbuf(GTK_IMAGE(prev_list[i]), prev);
                gtk_widget_set_sensitive(check_list[i], TRUE);
            }
        }
    }

    return 0;
}

int run_dialog_menu(app_widgets *app_wdgts, GtkWidget *prev_img, int image_number)
{
    // Check return value from Open Image dialog box to 
    // see if user clicked the Open button
    if (gtk_dialog_run(
                GTK_DIALOG (app_wdgts->w_dlg_file_choose)) == GTK_RESPONSE_OK) {

        // Get the file name from the dialog box
        char *file_name = gtk_file_chooser_get_filename(
                GTK_FILE_CHOOSER(app_wdgts->w_dlg_file_choose));
        if (file_name != NULL) {

            switch (image_number)
            {
                case 1:

                    if(strlen(app_wdgts->img1_path) == 0){
                        app_wdgts->img1_path = file_name;
                        gtk_widget_set_sensitive(app_wdgts->w_btn_collage_delete1, TRUE);
                        if(app_wdgts->img_count < 4)
                                app_wdgts->img_count += 1;
                    }
                    else{
                        app_wdgts->img1_path = file_name;
                    }
                    break;

                case 2:
                    
                    if(strlen(app_wdgts->img2_path) == 0){
                        app_wdgts->img2_path = file_name;
                        gtk_widget_set_sensitive(app_wdgts->w_btn_collage_delete2, TRUE);
                        if(app_wdgts->img_count < 4)
                                app_wdgts->img_count += 1;
                    }
                    else{
                        app_wdgts->img2_path = file_name;
                    }                    
                    break;

                case 3:

                    if(strlen(app_wdgts->img3_path) == 0){
                        app_wdgts->img3_path = file_name;
                        gtk_widget_set_sensitive(app_wdgts->w_btn_collage_delete3, TRUE);
                        if(app_wdgts->img_count < 4)
                                app_wdgts->img_count += 1;
                    }
                    else{
                        app_wdgts->img3_path = file_name;
                    }
                    break;

                case 4:

                    if(strlen(app_wdgts->img4_path) == 0){
                        app_wdgts->img4_path = file_name;
                        gtk_widget_set_sensitive(app_wdgts->w_btn_collage_delete4, TRUE);
                        if(app_wdgts->img_count < 4)
                                app_wdgts->img_count += 1;
                    }
                    else{
                        app_wdgts->img4_path = file_name;
                    }
                    break;

                default:
                    break;
            }

            GdkPixbuf *prev =
                gdk_pixbuf_new_from_file(
                        file_name, 
                        NULL
                        );

            int img_w = gdk_pixbuf_get_width(prev);
            int img_h = gdk_pixbuf_get_height(prev);

            int coef, width, height;  
            if(img_w > img_h)
            {
                coef = img_w / 250;
                width = img_w / coef;
                height = img_h / coef;
            }
            else
            {
                coef = img_h / 200;
                width = img_w / coef;
                height = img_h / coef;
            }

            prev = gdk_pixbuf_scale_simple(prev,
                    width,
                    height,
                    GDK_INTERP_NEAREST
                    );

            gtk_image_set_from_pixbuf(
                    GTK_IMAGE(prev_img), 
                    prev
                    );

            update_all_presets(app_wdgts, app_wdgts->img_count);
        }
    }

    // Finished with the "Open Image" dialog box, so hide it
    gtk_widget_hide(app_wdgts->w_dlg_file_choose);

    return 0;
}

int update_prev_final(app_widgets *app_wdgts)
{
    float coef = 3.15;
    GdkPixbuf *prev = gdk_pixbuf_new_from_file("cache/preset_tmp.png", NULL);
    prev = gdk_pixbuf_scale_simple(prev, gdk_pixbuf_get_width(prev)/coef,
            gdk_pixbuf_get_height(prev)/coef, GDK_INTERP_NEAREST);
    gtk_image_set_from_pixbuf(GTK_IMAGE(app_wdgts->w_prev_final), prev);

    return 0;
}

int set_collage_background_color(int r, int g, int b){

        gdImagePtr im = 
                gdImageCreateFromFile("src/resources/func_divers/background.png");
        
        FILE *out = fopen("cache/background.png", "wb");

        int color_dst = gdImageColorAllocate(im, r, g, b);
        int witdh = gdImageSX(im);
        int height = gdImageSY(im);

        for(int x = 0; x < witdh; x++){
                for(int y = 0; y < height; y++)
                {
                        gdImageSetPixel(im, x, y, color_dst);
                }
        }

        gdImagePng(im, out);

        fclose(out);

        return 0;
}

void on_window_main_key_press_event(GtkWidget *widget, GdkEventKey *event, app_widgets *app_wdgts){
    if (widget) NULL;
    switch (event->keyval)
    {
    case GDK_KEY_a:
            on_btn_apply_clicked(app_wdgts->w_btn_apply, app_wdgts);
            break;
    case GDK_KEY_s:
            if(gtk_widget_is_sensitive(app_wdgts->w_menubar_save))
                on_menubar_save_activate(app_wdgts->w_menubar_save, app_wdgts);
            else
                on_menubar_save_as_activate(app_wdgts->w_menubar_save_as, app_wdgts);
            break;
    case GDK_KEY_u:
            on_btn_undo_activate(app_wdgts->w_btn_apply, app_wdgts);
            break;
    default:
            break;
    }
}
///
//

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
    gtk_widget_set_sensitive(app_wdgts->w_check_weekdayfilter, FALSE);
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
    gtk_widget_set_sensitive(app_wdgts->w_check_weekdayfilter, FALSE);
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
    gtk_widget_set_sensitive(app_wdgts->w_check_weekdayfilter, FALSE);
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

int on_entry_shapes_value_changed(GtkWidget *entry_shape,
        app_widgets *app_wdgts)
{
    app_wdgts->pattern_size = gtk_spin_button_get_value(GTK_SPIN_BUTTON(entry_shape));
    return 0;
}

int on_btn_add_activate(GtkMenuItem *menuitem, app_widgets *app_wdgts)
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

            app_wdgts->insert_img = gdImageCreateFromFile(file_name);

            // gets info of the image
            app_wdgts->insert_img_w = gdImageSX(app_wdgts->insert_img) / 2;
            app_wdgts->insert_img_h = gdImageSY(app_wdgts->insert_img) / 2;

            app_wdgts->insert_img_select = 1;
        }
        g_free(file_name);
    }

    // Finished with the "Open Image" dialog box, so hide it
    gtk_widget_hide(app_wdgts->w_dlg_file_choose);

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

int on_btn_circle_focus_in_event(GtkWidget *btn_fill, GdkEventMotion *event,
        app_widgets *app_wdgts)
{
    printf("%ld\n", sizeof(btn_fill));
    printf("%ld\n", sizeof(event));
    printf("%ld\n", sizeof(app_wdgts));
    app_wdgts->focus_motif = 1;
    app_wdgts->pattern_type = "circle";
    return 0;
}

int on_btn_circle_focus_out_event(GtkWidget *btn_fill, GdkEventMotion *event,
        app_widgets *app_wdgts)
{
    printf("%ld\n", sizeof(btn_fill));
    printf("%ld\n", sizeof(event));
    printf("%ld\n", sizeof(app_wdgts));
    app_wdgts->focus_motif = 0;
    return 0;
}

int on_btn_square_focus_in_event(GtkWidget *btn_fill, GdkEventMotion *event,
        app_widgets *app_wdgts)
{
    printf("%ld\n", sizeof(btn_fill));
    printf("%ld\n", sizeof(event));
    printf("%ld\n", sizeof(app_wdgts));
    app_wdgts->focus_motif = 1;
    app_wdgts->pattern_type = "square";
    return 0;
}

int on_btn_square_focus_out_event(GtkWidget *btn_fill, GdkEventMotion *event,
        app_widgets *app_wdgts)
{
    printf("%ld\n", sizeof(btn_fill));
    printf("%ld\n", sizeof(event));
    printf("%ld\n", sizeof(app_wdgts));
    app_wdgts->focus_motif = 0;
    return 0;
}

int on_btn_triangle_focus_in_event(GtkWidget *btn_fill, GdkEventMotion *event,
        app_widgets *app_wdgts)
{
    printf("%ld\n", sizeof(btn_fill));
    printf("%ld\n", sizeof(event));
    printf("%ld\n", sizeof(app_wdgts));
    app_wdgts->focus_motif = 1;
    app_wdgts->pattern_type = "triangle";
    return 0;
}

int on_btn_triangle_focus_out_event(GtkWidget *btn_fill, GdkEventMotion *event,
        app_widgets *app_wdgts)
{
    printf("%ld\n", sizeof(btn_fill));
    printf("%ld\n", sizeof(event));
    printf("%ld\n", sizeof(app_wdgts));
    app_wdgts->focus_motif = 0;
    return 0;
}

int on_btn_pentagon_focus_in_event(GtkWidget *btn_fill, GdkEventMotion *event,
        app_widgets *app_wdgts)
{
    printf("%ld\n", sizeof(btn_fill));
    printf("%ld\n", sizeof(event));
    printf("%ld\n", sizeof(app_wdgts));
    app_wdgts->focus_motif = 1;
    app_wdgts->pattern_type = "pentagon";
    return 0;
}

int on_btn_pentagon_focus_out_event(GtkWidget *btn_fill, GdkEventMotion *event,
        app_widgets *app_wdgts)
{
    printf("%ld\n", sizeof(btn_fill));
    printf("%ld\n", sizeof(event));
    printf("%ld\n", sizeof(app_wdgts));
    app_wdgts->focus_motif = 0;
    return 0;
}

int on_btn_star_focus_in_event(GtkWidget *btn_fill, GdkEventMotion *event,
        app_widgets *app_wdgts)
{
    printf("%ld\n", sizeof(btn_fill));
    printf("%ld\n", sizeof(event));
    printf("%ld\n", sizeof(app_wdgts));
    app_wdgts->focus_motif = 1;
    app_wdgts->pattern_type = "star";
    return 0;
}

int on_btn_star_focus_out_event(GtkWidget *btn_fill, GdkEventMotion *event,
        app_widgets *app_wdgts)
{
    printf("%ld\n", sizeof(btn_fill));
    printf("%ld\n", sizeof(event));
    printf("%ld\n", sizeof(app_wdgts));
    app_wdgts->focus_motif = 0;
    return 0;
}

int on_btn_heart_focus_in_event(GtkWidget *btn_fill, GdkEventMotion *event,
        app_widgets *app_wdgts)
{
    printf("%ld\n", sizeof(btn_fill));
    printf("%ld\n", sizeof(event));
    printf("%ld\n", sizeof(app_wdgts));
    app_wdgts->focus_motif = 1;
    app_wdgts->pattern_type = "heart";
    return 0;
}

int on_btn_heart_focus_out_event(GtkWidget *btn_fill, GdkEventMotion *event,
        app_widgets *app_wdgts)
{
    printf("%ld\n", sizeof(btn_fill));
    printf("%ld\n", sizeof(event));
    printf("%ld\n", sizeof(app_wdgts));
    app_wdgts->focus_motif = 0;
    return 0;
}

int on_btn_collage_activate(GtkWidget *btn_collage,
        app_widgets *app_wdgts)
{
    printf("%ld\n", sizeof(btn_collage));
    init_collage_interface(app_wdgts);
    update_all_presets(app_wdgts, app_wdgts->img_count);
    set_collage_background_color(255, 255, 255);
    gtk_widget_show(app_wdgts->w_window_collage);
    return 0;
}

int on_collage_btn_apply_clicked(GtkWidget *btn_collage_apply,
        app_widgets *app_wdgts)
{
    printf("%ld\n", sizeof(btn_collage_apply));
    // Create the pixbuf image to display
    app_wdgts->tmp_img = gdk_pixbuf_new_from_file("cache/preset_tmp.png", NULL);

    // Save copies of the original files
    gdk_pixbuf_savev(
            app_wdgts->tmp_img, "cache/cp_img.png", "png", NULL, NULL, NULL);
    gdk_pixbuf_savev(
            app_wdgts->tmp_img, "cache/temp_img.png", "png", NULL, NULL, NULL);

    app_wdgts->gd_img = gdImageCreateFromFile("cache/cp_img.png");
    init_main_window(app_wdgts);
    gtk_widget_hide(app_wdgts->w_window_collage);
    return 0;
}

int on_collage_btn_cancel_clicked(GtkWidget *btn_collage_cancel,
        app_widgets *app_wdgts)
{
    printf("%ld\n", sizeof(btn_collage_cancel));
    gtk_widget_hide(app_wdgts->w_window_collage);
    return 0;
}

int on_btn_collage_img1_clicked(GtkWidget *btn_img1, app_widgets *app_wdgts){
    printf("%ld\n", sizeof(btn_img1));
    // Show the "Open Image" dialog box
    gtk_widget_show(app_wdgts->w_dlg_file_choose);
    run_dialog_menu(app_wdgts, app_wdgts->w_prev_img1, 1);\
    return 0;
}

int on_btn_collage_img2_clicked(GtkWidget *btn_img1, app_widgets *app_wdgts){
    printf("%ld\n", sizeof(btn_img1));
    // Show the "Open Image" dialog box
    gtk_widget_show(app_wdgts->w_dlg_file_choose);
    run_dialog_menu(app_wdgts, app_wdgts->w_prev_img2, 2);
    return 0;
}

int on_btn_collage_img3_clicked(GtkWidget *btn_img1, app_widgets *app_wdgts){
    printf("%ld\n", sizeof(btn_img1));
    // Show the "Open Image" dialog box
    gtk_widget_show(app_wdgts->w_dlg_file_choose);
    run_dialog_menu(app_wdgts, app_wdgts->w_prev_img3, 3);
    return 0;
}

int on_btn_collage_img4_clicked(GtkWidget *btn_img4, app_widgets *app_wdgts){
    printf("%ld\n", sizeof(btn_img4));
    // Show the "Open Image" dialog box
    gtk_widget_show(app_wdgts->w_dlg_file_choose);
    run_dialog_menu(app_wdgts, app_wdgts->w_prev_img4, 4);
    return 0;
}

int on_check_preset1_toggled(GtkWidget *btn_preset1, app_widgets *app_wdgts){

    if(gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(
                    btn_preset1)))
    {
        char* paths_list[] = { 
                app_wdgts->img1_path, 
                app_wdgts->img2_path, 
                app_wdgts->img3_path,
                app_wdgts->img4_path
        };

        char* avail_paths[4]; 
        int n = 0;

        for(int i = 0; i < 4; i++)
        {
                if (strlen(paths_list[i]) > 0){
                       avail_paths[n] = paths_list[i];
                       n += 1;
                }
        }
        
        gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(
                    app_wdgts->w_check_preset2), FALSE);
        gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(
                    app_wdgts->w_check_preset3), FALSE);

        switch (app_wdgts->img_count)
        {
            case 2:
                collage("cache/background.png", avail_paths[0], 
                        avail_paths[1], "", "", "cache/preset_tmp.png");
                break;
            case 3:
                collage("cache/background.png", avail_paths[0], 
                        avail_paths[1], avail_paths[2], "", "cache/preset_tmp.png");
                break;
            case 4:
                collage("cache/background.png", avail_paths[0], 
                        avail_paths[1], avail_paths[2], avail_paths[3], "cache/preset_tmp.png");
                break;
            default:
                break;
        }

        update_prev_final(app_wdgts);
    }

    return 0;
}

int on_check_preset2_toggled(GtkWidget *btn_preset2, app_widgets *app_wdgts){

    if(gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(
                    btn_preset2)))
    {
        char* paths_list[] = { 
                app_wdgts->img1_path, 
                app_wdgts->img2_path, 
                app_wdgts->img3_path,
                app_wdgts->img4_path
        };

        char* avail_paths[4]; 
        int n = 0;

        for(int i = 0; i < 4; i++)
        {
                if (strlen(paths_list[i]) > 0){
                       avail_paths[n] = paths_list[i];
                       n += 1;
                }
        }

        gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(
                    app_wdgts->w_check_preset1), FALSE);
        gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(
                    app_wdgts->w_check_preset3), FALSE);

        switch (app_wdgts->img_count)
        {
            case 2:
                collage_2("cache/background.png", avail_paths[0], 
                        avail_paths[1], "", "", "cache/preset_tmp.png");
                break;
            case 3:
                collage_2("cache/background.png", avail_paths[0], 
                        avail_paths[1], avail_paths[2], "", "cache/preset_tmp.png");
                break;
            case 4:
                collage_2("cache/background.png", avail_paths[0], 
                        avail_paths[1], avail_paths[2], avail_paths[3], "cache/preset_tmp.png");
                break;
            default:
                break;
        }

        update_prev_final(app_wdgts);
    }
    return 0;
}

int on_check_preset3_toggled(GtkWidget *btn_preset3, app_widgets *app_wdgts){

    if(gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(
                    btn_preset3)))
    {
        char* paths_list[] = { 
                app_wdgts->img1_path, 
                app_wdgts->img2_path, 
                app_wdgts->img3_path,
                app_wdgts->img4_path
        };

        char* avail_paths[4]; 
        int n = 0;

        for(int i = 0; i < 4; i++)
        {
                if (strlen(paths_list[i]) > 0){
                       avail_paths[n] = paths_list[i];
                       n += 1;
                }
        }

        gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(
                    app_wdgts->w_check_preset2), FALSE);
        gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(
                    app_wdgts->w_check_preset1), FALSE);

        switch (app_wdgts->img_count)
        {
            case 3:
                collage_4("cache/background.png", avail_paths[0], 
                        avail_paths[1], avail_paths[2], "cache/preset_tmp.png");
                break;
            case 4:
                collage_3("cache/background.png", avail_paths[0], 
                        avail_paths[1], avail_paths[2], avail_paths[3], "cache/preset_tmp.png");
                break;
            default:
                break;
        }

        update_prev_final(app_wdgts);
    }

    return 0;
}

int on_btn_collage_delete1_clicked(GtkWidget *btn_collage_delete, app_widgets *app_wdgts)
{
        gtk_image_set_from_icon_name(GTK_IMAGE(app_wdgts->w_prev_img1), "unknown", GTK_ICON_SIZE_INVALID);
        app_wdgts->img1_path = "";
        app_wdgts->img_count -= 1;
        update_all_presets(app_wdgts, app_wdgts->img_count);
        gtk_widget_set_sensitive(btn_collage_delete, FALSE);
        return 0;
}

int on_btn_collage_delete2_clicked(GtkWidget *btn_collage_delete, app_widgets *app_wdgts)
{
        gtk_image_set_from_icon_name(GTK_IMAGE(app_wdgts->w_prev_img2), "unknown", GTK_ICON_SIZE_INVALID);
        app_wdgts->img2_path = "";
        app_wdgts->img_count -= 1;
        update_all_presets(app_wdgts, app_wdgts->img_count);
        gtk_widget_set_sensitive(btn_collage_delete, FALSE);
        return 0;
}

int on_btn_collage_delete3_clicked(GtkWidget *btn_collage_delete, app_widgets *app_wdgts)
{
        gtk_image_set_from_icon_name(GTK_IMAGE(app_wdgts->w_prev_img3), "unknown", GTK_ICON_SIZE_INVALID);
        app_wdgts->img3_path = "";
        app_wdgts->img_count -= 1;
        update_all_presets(app_wdgts, app_wdgts->img_count);
        gtk_widget_set_sensitive(btn_collage_delete, FALSE);
        return 0;
}

int on_btn_collage_delete4_clicked(GtkWidget *btn_collage_delete, app_widgets *app_wdgts)
{
        gtk_image_set_from_icon_name(GTK_IMAGE(app_wdgts->w_prev_img4), "unknown", GTK_ICON_SIZE_INVALID);
        app_wdgts->img4_path = "";
        app_wdgts->img_count -= 1;
        update_all_presets(app_wdgts, app_wdgts->img_count);
        gtk_widget_set_sensitive(btn_collage_delete, FALSE);
        return 0;
}

int on_btn_collage_color_color_set(GtkWidget *btn_collage_color)
{

        GdkRGBA *color_set = malloc(sizeof(GdkRGBA));
        gtk_color_chooser_get_rgba(GTK_COLOR_CHOOSER(btn_collage_color), color_set);

        set_collage_background_color(color_set->red * 255, color_set->green * 255, color_set->blue * 255);

        return 0;
}

int on_btn_txt_apply_clicked(GtkWidget *btn_txt_apply, app_widgets *app_wdgts)
{
        printf("%ld\n", sizeof(btn_txt_apply));
        int size = gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(app_wdgts->w_text_size));
        GdkRGBA *color_set = malloc(sizeof(GdkRGBA));
        gtk_color_chooser_get_rgba(GTK_COLOR_CHOOSER(app_wdgts->w_color_text), color_set);
        int r = color_set->red * 255;
        int g = color_set->green * 255;
        int b = color_set->blue * 255;
        int color = gdImageColorAllocate(app_wdgts->gd_img, r, g, b);
        free(color_set);

        Add_text(app_wdgts->gd_img, app_wdgts->gd_out, "cache/temp_img.png",
                app_wdgts->font_path[app_wdgts->font_index], app_wdgts->tmp_x, 
                app_wdgts->tmp_y + size , 200, 200, color, size, 0, 
                gtk_entry_get_text(GTK_ENTRY(app_wdgts->w_entry_text)));
        app_wdgts->cr_action = 1;
        gtk_widget_queue_draw(app_wdgts->w_drawing_aera);        
        gtk_widget_hide(app_wdgts->w_pop_up_text);
        return 0;
}

int on_btn_txt_cancel_clicked(GtkWidget *btn_txt_cancel, app_widgets *app_wdgts)
{
        printf("%ld\n", sizeof(btn_txt_cancel));
        gtk_widget_hide(app_wdgts->w_pop_up_text);
        return 0;
}

int on_txt_absolute_clicked(GtkWidget *btn_txt, app_widgets *app_wdgts)
{
        printf("%ld\n", sizeof(btn_txt));
        app_wdgts->font_index = 0;
        return 0;
}

int on_txt_arial_clicked(GtkWidget *btn_txt, app_widgets *app_wdgts)
{
        printf("%ld\n", sizeof(btn_txt));
        app_wdgts->font_index = 1;
        return 0;
}

int on_txt_amstajul_clicked(GtkWidget *btn_txt, app_widgets *app_wdgts)
{
        printf("%ld\n", sizeof(btn_txt));
        app_wdgts->font_index = 2;
        return 0;
}

int on_txt_bethanie_clicked(GtkWidget *btn_txt, app_widgets *app_wdgts)
{
        printf("%ld\n", sizeof(btn_txt));

        app_wdgts->font_index = 3;
        return 0;
}

int on_txt_brook_clicked(GtkWidget *btn_txt, app_widgets *app_wdgts)
{
        printf("%ld\n", sizeof(btn_txt));
        app_wdgts->font_index = 4;
        return 0;
}

int on_txt_calibri_clicked(GtkWidget *btn_txt, app_widgets *app_wdgts)
{
        printf("%ld\n", sizeof(btn_txt));
        app_wdgts->font_index = 5;
        return 0;
}

int on_txt_cambria_clicked(GtkWidget *btn_txt, app_widgets *app_wdgts)
{
        printf("%ld\n", sizeof(btn_txt));
        app_wdgts->font_index = 6;
        return 0;
}

int on_txt_caudex_clicked(GtkWidget *btn_txt, app_widgets *app_wdgts)
{
        printf("%ld\n", sizeof(btn_txt));
        app_wdgts->font_index = 7;
        return 0;
}

int on_txt_digit_clicked(GtkWidget *btn_txt, app_widgets *app_wdgts)
{
        printf("%ld\n", sizeof(btn_txt));
        app_wdgts->font_index = 8;
        return 0;
}

int on_txt_karpow_clicked(GtkWidget *btn_txt, app_widgets *app_wdgts)
{
        printf("%ld\n", sizeof(btn_txt));
        app_wdgts->font_index = 9;
        return 0;
}

int on_txt_noto_clicked(GtkWidget *btn_txt, app_widgets *app_wdgts)
{
        printf("%ld\n", sizeof(btn_txt));
        app_wdgts->font_index = 10;
        return 0;
}

int on_txt_oswald_clicked(GtkWidget *btn_txt, app_widgets *app_wdgts)
{
        printf("%ld\n", sizeof(btn_txt));
        app_wdgts->font_index = 11;
        return 0;
}

int on_txt_propaganda_clicked(GtkWidget *btn_txt, app_widgets *app_wdgts)
{
        printf("%ld\n", sizeof(btn_txt));
        app_wdgts->font_index = 12;
        return 0;
}

int on_txt_ristella_clicked(GtkWidget *btn_txt, app_widgets *app_wdgts)
{
        printf("%ld\n", sizeof(btn_txt));
        app_wdgts->font_index = 13;
        return 0;
}

int on_txt_thunder_clicked(GtkWidget *btn_txt, app_widgets *app_wdgts)
{
        printf("%ld\n", sizeof(btn_txt));
        app_wdgts->font_index = 14;
        return 0;
}

int on_txt_times_clicked(GtkWidget *btn_txt, app_widgets *app_wdgts)
{
        printf("%ld\n", sizeof(btn_txt));
        app_wdgts->font_index = 15;
        return 0;
}

int on_txt_vogue_clicked(GtkWidget *btn_txt, app_widgets *app_wdgts)
{
        printf("%ld\n", sizeof(btn_txt));
        app_wdgts->font_index = 16;
        return 0;
}
//
////
//#pragma  endregion OthersFunctions