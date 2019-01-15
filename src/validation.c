#include <stdlib.h>
#include <stdio.h>
#include <sys/time.h>
#include <string.h>
#include <gtk/gtk.h>
#include "validation.h"

static GtkWidget *hash_field;
static GtkWidget *window;

static char* g_inserted_hash_code;
int g_elapsed_time = 0;

void on_click_event(GtkWidget *get_hash, gpointer data) {
    g_inserted_hash_code = strdup((char *)gtk_entry_get_text(GTK_ENTRY(hash_field)));
    gtk_widget_destroy(window);
}

void on_delete_event(){
    gtk_widget_destroy(window);
    gtk_main_quit();
}

void new_alert_window(char* message) {
    GtkWidget *grid, *get_hash, *label;

    window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);
    //g_signal_connect (window, "delete-event", G_CALLBACK (on_delete_event), NULL);

    gtk_window_set_position(GTK_WINDOW(window), GTK_WIN_POS_CENTER);
    gtk_window_set_default_size(GTK_WINDOW(window), 800, 800);
    gtk_window_set_resizable(GTK_WINDOW(window), FALSE);
    gtk_window_set_title(GTK_WINDOW(window), "Authentication");


    grid = gtk_grid_new();
    gtk_container_add(GTK_CONTAINER(window), grid);

    size_t needed_bytes = snprintf(NULL, 0, "\n\n     %s    \n\n", message);
    char* formated_message = (char*) malloc(sizeof(char) * needed_bytes);

    sprintf(formated_message, "\n\n     %s      \n\n", message);

    label = gtk_label_new(formated_message);
    gtk_grid_attach(GTK_GRID(grid), label, 0, 0, 1, 1);

    gtk_widget_show_all(window);
    gtk_main();
}

gboolean time_handler(GtkWidget *widget) {

    g_elapsed_time++;

    size_t needed_bytes = snprintf(NULL, 0 , "(%d seconds remaining) Authenticator", TIMEOUT - g_elapsed_time);
    char* title = (char*) malloc(sizeof(char) * needed_bytes);
    sprintf(title, "(%d seconds remaining) Authentication", TIMEOUT - g_elapsed_time);

    gtk_window_set_title(GTK_WINDOW(window), title);

    // If true returned, function keeps getting called every second. If false, no more called
    if (g_elapsed_time != TIMEOUT) {
        return TRUE;
    } else {
        gtk_widget_destroy(widget);
        return FALSE;
    }
}

void new_validation_window(char* email){

    GtkWidget *grid, *get_hash, *label;

    window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);

    gtk_window_set_position(GTK_WINDOW(window), GTK_WIN_POS_CENTER);
    gtk_window_set_default_size(GTK_WINDOW(window), 500, 300);
    gtk_window_set_resizable(GTK_WINDOW(window), FALSE);
    gtk_window_set_title(GTK_WINDOW(window), "Authentication");

    g_timeout_add(1000, (GSourceFunc) time_handler, (gpointer) window);
    time_handler(window);

    grid = gtk_grid_new();
    gtk_container_add(GTK_CONTAINER(window), grid);

    size_t needed_bytes = snprintf(NULL, 0 , "\n   Email sent to  %s.\n     Insert the received code below.\n", email);
    char* description = (char*) malloc(sizeof(char) * needed_bytes);

    sprintf(description, "\n    Email sent to %s.\n    Insert the received code below. \n", email);

    label = gtk_label_new(description);
    gtk_grid_attach(GTK_GRID(grid), label, 0, 0, 1, 1);

    hash_field = gtk_entry_new();
    gtk_entry_set_text(GTK_ENTRY(hash_field), "");
    gtk_entry_set_max_length(GTK_ENTRY(hash_field), HASH_CODE_SIZE);
    gtk_grid_attach(GTK_GRID(grid), hash_field, 0, 1, 1, 1);

    get_hash = gtk_button_new_with_label("Confirm");
    g_signal_connect(get_hash, "clicked", G_CALLBACK(on_click_event), NULL);
    gtk_grid_attach(GTK_GRID(grid), get_hash, 2, 1, 1, 1);

    gtk_widget_show_all(window);
    gtk_main();
}

int validate(char* real_hash_code, char* email){
    g_elapsed_time = 0;
    g_inserted_hash_code = NULL;

    new_validation_window(email);

    int return_code;

    // Test for exceeded timeout limit or inserted code different
    // from one sent through e-mail
    if (g_elapsed_time == TIMEOUT) {
        return_code = TIMEOUT_EXCEEDED;
    } else if(g_inserted_hash_code == NULL || strncmp(real_hash_code, g_inserted_hash_code, HASH_CODE_SIZE) != 0){
        return_code = INVALID_CODE;
    } else {
        return_code = VALID_CODE;
    }

    free(g_inserted_hash_code);
    return return_code;
}
