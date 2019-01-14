#include <stdlib.h>
#include <stdio.h>
#include <sys/time.h>
#include <string.h>
#include <gtk/gtk.h>
#include "validation.h"

static GtkWidget *hash_field;
static GtkWidget *window;
static char* inserted_hash_code;

long long unsigned initial_time;
struct timeval t;

void start(){
    gettimeofday(&t, NULL);
    initial_time = t.tv_sec;
}

long long unsigned stop(){
    gettimeofday(&t, NULL);
    long long unsigned final_time = t.tv_sec;
    return final_time - initial_time;
}

void do_get_hash(GtkWidget *get_hash, gpointer data) {
    inserted_hash_code = strdup((char *)gtk_entry_get_text(GTK_ENTRY(hash_field)));
    gtk_widget_destroy(window);
}

void on_delete_event(){
    gtk_widget_destroy(window);
    gtk_main_quit();
}

void new_error_window() {
    GtkWidget *grid, *get_hash, *label;

    window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);
    //g_signal_connect (window, "delete-event", G_CALLBACK (on_delete_event), NULL);

    gtk_window_set_position(GTK_WINDOW(window), GTK_WIN_POS_CENTER);
    gtk_window_set_default_size(GTK_WINDOW(window), 500, 500);
    //gtk_window_move (GTK_WINDOW(window),30,60);
    gtk_window_set_resizable(GTK_WINDOW(window), FALSE);
    gtk_window_set_title(GTK_WINDOW(window), "Authentication");


    grid = gtk_grid_new();
    gtk_container_add(GTK_CONTAINER(window), grid);

    label = gtk_label_new("\n\n Acesso negado.\n\n");
    gtk_grid_attach(GTK_GRID(grid), label, 0, 0, 1, 1);

    gtk_widget_show_all(window);
    gtk_main();

}

void new_validation_window(){
    GtkWidget *grid, *get_hash, *label;

    window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);
    //g_signal_connect (window, "delete-event", G_CALLBACK (on_delete_event), NULL);

    gtk_window_set_position(GTK_WINDOW(window), GTK_WIN_POS_CENTER);
    gtk_window_set_default_size(GTK_WINDOW(window), 400, 300);
    //gtk_window_move (GTK_WINDOW(window),30,60);
    gtk_window_set_resizable(GTK_WINDOW(window), FALSE);
    gtk_window_set_title(GTK_WINDOW(window), "Authentication");


    grid = gtk_grid_new();
    gtk_container_add(GTK_CONTAINER(window), grid);

    label = gtk_label_new("\n    Foi enviado um email para <inserir email aqui>.\n    Insira em baixo o código recebido.\n ");
    gtk_grid_attach(GTK_GRID(grid), label, 0, 0, 1, 1);

    hash_field = gtk_entry_new();
    gtk_entry_set_text(GTK_ENTRY(hash_field), "");
    gtk_entry_set_max_length(GTK_ENTRY(hash_field), HASH_CODE_SIZE);
    gtk_grid_attach(GTK_GRID(grid), hash_field, 0, 1, 1, 1);

    get_hash = gtk_button_new_with_label("Confirm");
    g_signal_connect(get_hash, "clicked", G_CALLBACK(do_get_hash), NULL);
    gtk_grid_attach(GTK_GRID(grid), get_hash, 2, 1, 1, 1);

    gtk_widget_show_all(window);
    gtk_main();
}

int validate(char* real_hash_code){
    long long unsigned tt;
    inserted_hash_code = NULL;

    start();
    new_validation_window();
    tt = stop();

    // User left without entering code
    if(inserted_hash_code == NULL) return 0;

    printf("Vou comparar %s e %s\n", real_hash_code, inserted_hash_code);

    // Exceeded timeout limit or inserted code is different from the real one
    if(tt > TIMEOUT || strncmp(real_hash_code, inserted_hash_code, HASH_CODE_SIZE) != 0){
        free(inserted_hash_code);
        return 0;
    }

    free(inserted_hash_code);
    return 1;
}

/*
int main(int argc, char **argv) {
    gtk_init(NULL,NULL);
    while(1){
        int res = validate("diogo");
        fprintf(stderr,"Res: %d\n",res);
        if(res) break;
    }

    return 0;
}
*/
