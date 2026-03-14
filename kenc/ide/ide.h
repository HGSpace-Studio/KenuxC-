#ifndef KENC_IDE_H
#define KENC_IDE_H

#include <gtk/gtk.h>

typedef struct {
    GtkWidget* window;
    GtkWidget* vbox;
    GtkWidget* menubar;
    GtkWidget* toolbar;
    GtkWidget* notebook;
    GtkWidget* source_view;
    GtkWidget* output_view;
    GtkWidget* preview_view;
    GtkWidget* statusbar;
    GtkTextBuffer* source_buffer;
    GtkTextBuffer* output_buffer;
    GtkTextBuffer* preview_buffer;
    char* current_file;
    int modified;
} ide_window_t;

ide_window_t* ide_create(void);
void ide_show(ide_window_t* ide);
void ide_free(ide_window_t* ide);

void ide_new_file(ide_window_t* ide);
void ide_open_file(ide_window_t* ide, const char* filename);
void ide_save_file(ide_window_t* ide);
void ide_save_file_as(ide_window_t* ide);
void ide_close_file(ide_window_t* ide);

void ide_compile(ide_window_t* ide);
void ide_run(ide_window_t* ide);
void ide_preview(ide_window_t* ide);

void ide_show_about(ide_window_t* ide);
void ide_show_help(ide_window_t* ide);

void ide_append_output(ide_window_t* ide, const char* text);
void ide_clear_output(ide_window_t* ide);
void ide_set_status(ide_window_t* ide, const char* status);

#endif