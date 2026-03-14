#include "ide.h"
#include <stdlib.h>
#include <string.h>
#include <glib.h>

/* Helper functions */
static void create_menubar(ide_window_t* ide);
static void create_toolbar(ide_window_t* ide);
static void create_notebook(ide_window_t* ide);
static void create_statusbar(ide_window_t* ide);

static void on_new_file(GtkWidget* widget, gpointer data);
static void on_open_file(GtkWidget* widget, gpointer data);
static void on_save_file(GtkWidget* widget, gpointer data);
static void on_save_file_as(GtkWidget* widget, gpointer data);
static void on_close_file(GtkWidget* widget, gpointer data);
static void on_compile(GtkWidget* widget, gpointer data);
static void on_run(GtkWidget* widget, gpointer data);
static void on_preview(GtkWidget* widget, gpointer data);
static void on_about(GtkWidget* widget, gpointer data);
static void on_help(GtkWidget* widget, gpointer data);

static void on_text_changed(GtkTextBuffer* buffer, gpointer data);
static void on_window_destroy(GtkWidget* widget, gpointer data);

ide_window_t* ide_create(void) {
    ide_window_t* ide = (ide_window_t*)malloc(sizeof(ide_window_t));
    if (!ide) {
        return NULL;
    }
    
    /* Initialize GTK */
    gtk_init(NULL, NULL);
    
    /* Create main window */
    ide->window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(ide->window), "KenC# IDE");
    gtk_window_set_default_size(GTK_WINDOW(ide->window), 800, 600);
    
    /* Create main vbox */
    ide->vbox = gtk_vbox_new(FALSE, 0);
    gtk_container_add(GTK_CONTAINER(ide->window), ide->vbox);
    
    /* Create UI components */
    create_menubar(ide);
    create_toolbar(ide);
    create_notebook(ide);
    create_statusbar(ide);
    
    /* Set initial state */
    ide->current_file = NULL;
    ide->modified = 0;
    
    /* Connect signals */
    g_signal_connect(ide->window, "destroy", G_CALLBACK(on_window_destroy), ide);
    g_signal_connect(ide->source_buffer, "changed", G_CALLBACK(on_text_changed), ide);
    
    return ide;
}

void ide_show(ide_window_t* ide) {
    if (ide) {
        gtk_widget_show_all(ide->window);
        gtk_main();
    }
}

void ide_free(ide_window_t* ide) {
    if (ide) {
        if (ide->current_file) {
            free(ide->current_file);
        }
        free(ide);
    }
}

static void create_menubar(ide_window_t* ide) {
    ide->menubar = gtk_menu_bar_new();
    gtk_box_pack_start(GTK_BOX(ide->vbox), ide->menubar, FALSE, FALSE, 0);
    
    /* File menu */
    GtkWidget* file_menu = gtk_menu_new();
    GtkWidget* file_item = gtk_menu_item_new_with_label("File");
    gtk_menu_item_set_submenu(GTK_MENU_ITEM(file_item), file_menu);
    
    GtkWidget* new_item = gtk_menu_item_new_with_label("New");
    GtkWidget* open_item = gtk_menu_item_new_with_label("Open");
    GtkWidget* save_item = gtk_menu_item_new_with_label("Save");
    GtkWidget* save_as_item = gtk_menu_item_new_with_label("Save As");
    GtkWidget* close_item = gtk_menu_item_new_with_label("Close");
    GtkWidget* separator = gtk_separator_menu_item_new();
    GtkWidget* quit_item = gtk_menu_item_new_with_label("Quit");
    
    gtk_menu_shell_append(GTK_MENU_SHELL(file_menu), new_item);
    gtk_menu_shell_append(GTK_MENU_SHELL(file_menu), open_item);
    gtk_menu_shell_append(GTK_MENU_SHELL(file_menu), save_item);
    gtk_menu_shell_append(GTK_MENU_SHELL(file_menu), save_as_item);
    gtk_menu_shell_append(GTK_MENU_SHELL(file_menu), close_item);
    gtk_menu_shell_append(GTK_MENU_SHELL(file_menu), separator);
    gtk_menu_shell_append(GTK_MENU_SHELL(file_menu), quit_item);
    
    /* Edit menu */
    GtkWidget* edit_menu = gtk_menu_new();
    GtkWidget* edit_item = gtk_menu_item_new_with_label("Edit");
    gtk_menu_item_set_submenu(GTK_MENU_ITEM(edit_item), edit_menu);
    
    /* Build menu */
    GtkWidget* build_menu = gtk_menu_new();
    GtkWidget* build_item = gtk_menu_item_new_with_label("Build");
    gtk_menu_item_set_submenu(GTK_MENU_ITEM(build_item), build_menu);
    
    GtkWidget* compile_item = gtk_menu_item_new_with_label("Compile");
    GtkWidget* run_item = gtk_menu_item_new_with_label("Run");
    GtkWidget* preview_item = gtk_menu_item_new_with_label("Preview");
    
    gtk_menu_shell_append(GTK_MENU_SHELL(build_menu), compile_item);
    gtk_menu_shell_append(GTK_MENU_SHELL(build_menu), run_item);
    gtk_menu_shell_append(GTK_MENU_SHELL(build_menu), preview_item);
    
    /* Help menu */
    GtkWidget* help_menu = gtk_menu_new();
    GtkWidget* help_item = gtk_menu_item_new_with_label("Help");
    gtk_menu_item_set_submenu(GTK_MENU_ITEM(help_item), help_menu);
    
    GtkWidget* help_content_item = gtk_menu_item_new_with_label("Help");
    GtkWidget* about_item = gtk_menu_item_new_with_label("About");
    
    gtk_menu_shell_append(GTK_MENU_SHELL(help_menu), help_content_item);
    gtk_menu_shell_append(GTK_MENU_SHELL(help_menu), about_item);
    
    /* Add items to menubar */
    gtk_menu_shell_append(GTK_MENU_SHELL(ide->menubar), file_item);
    gtk_menu_shell_append(GTK_MENU_SHELL(ide->menubar), edit_item);
    gtk_menu_shell_append(GTK_MENU_SHELL(ide->menubar), build_item);
    gtk_menu_shell_append(GTK_MENU_SHELL(ide->menubar), help_item);
    
    /* Connect signals */
    g_signal_connect(new_item, "activate", G_CALLBACK(on_new_file), ide);
    g_signal_connect(open_item, "activate", G_CALLBACK(on_open_file), ide);
    g_signal_connect(save_item, "activate", G_CALLBACK(on_save_file), ide);
    g_signal_connect(save_as_item, "activate", G_CALLBACK(on_save_file_as), ide);
    g_signal_connect(close_item, "activate", G_CALLBACK(on_close_file), ide);
    g_signal_connect(compile_item, "activate", G_CALLBACK(on_compile), ide);
    g_signal_connect(run_item, "activate", G_CALLBACK(on_run), ide);
    g_signal_connect(preview_item, "activate", G_CALLBACK(on_preview), ide);
    g_signal_connect(help_content_item, "activate", G_CALLBACK(on_help), ide);
    g_signal_connect(about_item, "activate", G_CALLBACK(on_about), ide);
    g_signal_connect(quit_item, "activate", G_CALLBACK(gtk_main_quit), NULL);
}

static void create_toolbar(ide_window_t* ide) {
    ide->toolbar = gtk_toolbar_new();
    gtk_box_pack_start(GTK_BOX(ide->vbox), ide->toolbar, FALSE, FALSE, 0);
    
    /* New file button */
    GtkToolItem* new_button = gtk_tool_button_new_from_stock(GTK_STOCK_NEW);
    gtk_toolbar_insert(GTK_TOOLBAR(ide->toolbar), new_button, -1);
    g_signal_connect(new_button, "clicked", G_CALLBACK(on_new_file), ide);
    
    /* Open file button */
    GtkToolItem* open_button = gtk_tool_button_new_from_stock(GTK_STOCK_OPEN);
    gtk_toolbar_insert(GTK_TOOLBAR(ide->toolbar), open_button, -1);
    g_signal_connect(open_button, "clicked", G_CALLBACK(on_open_file), ide);
    
    /* Save file button */
    GtkToolItem* save_button = gtk_tool_button_new_from_stock(GTK_STOCK_SAVE);
    gtk_toolbar_insert(GTK_TOOLBAR(ide->toolbar), save_button, -1);
    g_signal_connect(save_button, "clicked", G_CALLBACK(on_save_file), ide);
    
    /* Separator */
    GtkToolItem* separator = gtk_separator_tool_item_new();
    gtk_toolbar_insert(GTK_TOOLBAR(ide->toolbar), separator, -1);
    
    /* Compile button */
    GtkToolItem* compile_button = gtk_tool_button_new_from_stock(GTK_STOCK_EXECUTE);
    gtk_toolbar_insert(GTK_TOOLBAR(ide->toolbar), compile_button, -1);
    g_signal_connect(compile_button, "clicked", G_CALLBACK(on_compile), ide);
    
    /* Run button */
    GtkToolItem* run_button = gtk_tool_button_new_from_stock(GTK_STOCK_MEDIA_PLAY);
    gtk_toolbar_insert(GTK_TOOLBAR(ide->toolbar), run_button, -1);
    g_signal_connect(run_button, "clicked", G_CALLBACK(on_run), ide);
    
    /* Preview button */
    GtkToolItem* preview_button = gtk_tool_button_new_from_stock(GTK_STOCK_PRINT_PREVIEW);
    gtk_toolbar_insert(GTK_TOOLBAR(ide->toolbar), preview_button, -1);
    g_signal_connect(preview_button, "clicked", G_CALLBACK(on_preview), ide);
}

static void create_notebook(ide_window_t* ide) {
    ide->notebook = gtk_notebook_new();
    gtk_box_pack_start(GTK_BOX(ide->vbox), ide->notebook, TRUE, TRUE, 0);
    
    /* Source view */
    GtkWidget* source_scroll = gtk_scrolled_window_new(NULL, NULL);
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(source_scroll), 
                                  GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
    
    ide->source_view = gtk_text_view_new();
    ide->source_buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(ide->source_view));
    gtk_container_add(GTK_CONTAINER(source_scroll), ide->source_view);
    
    GtkWidget* source_label = gtk_label_new("Source");
    gtk_notebook_append_page(GTK_NOTEBOOK(ide->notebook), source_scroll, source_label);
    
    /* Output view */
    GtkWidget* output_scroll = gtk_scrolled_window_new(NULL, NULL);
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(output_scroll), 
                                  GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
    
    ide->output_view = gtk_text_view_new();
    ide->output_buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(ide->output_view));
    gtk_text_view_set_editable(GTK_TEXT_VIEW(ide->output_view), FALSE);
    gtk_container_add(GTK_CONTAINER(output_scroll), ide->output_view);
    
    GtkWidget* output_label = gtk_label_new("Output");
    gtk_notebook_append_page(GTK_NOTEBOOK(ide->notebook), output_scroll, output_label);
    
    /* Preview view */
    GtkWidget* preview_scroll = gtk_scrolled_window_new(NULL, NULL);
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(preview_scroll), 
                                  GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
    
    ide->preview_view = gtk_text_view_new();
    ide->preview_buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(ide->preview_view));
    gtk_text_view_set_editable(GTK_TEXT_VIEW(ide->preview_view), FALSE);
    gtk_container_add(GTK_CONTAINER(preview_scroll), ide->preview_view);
    
    GtkWidget* preview_label = gtk_label_new("Preview");
    gtk_notebook_append_page(GTK_NOTEBOOK(ide->notebook), preview_scroll, preview_label);
}

static void create_statusbar(ide_window_t* ide) {
    ide->statusbar = gtk_statusbar_new();
    gtk_box_pack_end(GTK_BOX(ide->vbox), ide->statusbar, FALSE, FALSE, 0);
    
    ide_set_status(ide, "Ready");
}

void ide_new_file(ide_window_t* ide) {
    if (ide->modified) {
        GtkWidget* dialog = gtk_message_dialog_new(GTK_WINDOW(ide->window), 
                                                 GTK_DIALOG_DESTROY_WITH_PARENT, 
                                                 GTK_MESSAGE_WARNING, 
                                                 GTK_BUTTONS_YES_NO, 
                                                 "Save changes to current file?");
        int response = gtk_dialog_run(GTK_DIALOG(dialog));
        gtk_widget_destroy(dialog);
        
        if (response == GTK_RESPONSE_YES) {
            ide_save_file(ide);
        }
    }
    
    gtk_text_buffer_set_text(ide->source_buffer, "", 0);
    if (ide->current_file) {
        free(ide->current_file);
        ide->current_file = NULL;
    }
    ide->modified = 0;
    
    gchar* title = g_strdup_printf("KenC# IDE - Untitled");
    gtk_window_set_title(GTK_WINDOW(ide->window), title);
    g_free(title);
    
    ide_set_status(ide, "New file created");
}

void ide_open_file(ide_window_t* ide, const char* filename) {
    if (ide->modified) {
        GtkWidget* dialog = gtk_message_dialog_new(GTK_WINDOW(ide->window), 
                                                 GTK_DIALOG_DESTROY_WITH_PARENT, 
                                                 GTK_MESSAGE_WARNING, 
                                                 GTK_BUTTONS_YES_NO, 
                                                 "Save changes to current file?");
        int response = gtk_dialog_run(GTK_DIALOG(dialog));
        gtk_widget_destroy(dialog);
        
        if (response == GTK_RESPONSE_YES) {
            ide_save_file(ide);
        }
    }
    
    if (!filename) {
        GtkWidget* dialog = gtk_file_chooser_dialog_new("Open File", 
                                                      GTK_WINDOW(ide->window), 
                                                      GTK_FILE_CHOOSER_ACTION_OPEN, 
                                                      GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL, 
                                                      GTK_STOCK_OPEN, GTK_RESPONSE_ACCEPT, 
                                                      NULL);
        
        GtkFileFilter* filter = gtk_file_filter_new();
        gtk_file_filter_set_name(filter, "KenC# Files (*.kenp)");
        gtk_file_filter_add_pattern(filter, "*.kenp");
        gtk_file_chooser_add_filter(GTK_FILE_CHOOSER(dialog), filter);
        
        int response = gtk_dialog_run(GTK_DIALOG(dialog));
        if (response == GTK_RESPONSE_ACCEPT) {
            filename = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(dialog));
        }
        gtk_widget_destroy(dialog);
        
        if (response != GTK_RESPONSE_ACCEPT) {
            return;
        }
    }
    
    FILE* file = fopen(filename, "r");
    if (!file) {
        GtkWidget* dialog = gtk_message_dialog_new(GTK_WINDOW(ide->window), 
                                                 GTK_DIALOG_DESTROY_WITH_PARENT, 
                                                 GTK_MESSAGE_ERROR, 
                                                 GTK_BUTTONS_OK, 
                                                 "Cannot open file: %s", filename);
        gtk_dialog_run(GTK_DIALOG(dialog));
        gtk_widget_destroy(dialog);
        return;
    }
    
    fseek(file, 0, SEEK_END);
    long size = ftell(file);
    fseek(file, 0, SEEK_SET);
    
    char* content = (char*)malloc(size + 1);
    if (!content) {
        fclose(file);
        return;
    }
    
    fread(content, 1, size, file);
    content[size] = '\0';
    fclose(file);
    
    gtk_text_buffer_set_text(ide->source_buffer, content, -1);
    free(content);
    
    if (ide->current_file) {
        free(ide->current_file);
    }
    ide->current_file = g_strdup(filename);
    ide->modified = 0;
    
    gchar* title = g_strdup_printf("KenC# IDE - %s", g_path_get_basename(filename));
    gtk_window_set_title(GTK_WINDOW(ide->window), title);
    g_free(title);
    
    ide_set_status(ide, g_strdup_printf("Opened file: %s", g_path_get_basename(filename)));
}

void ide_save_file(ide_window_t* ide) {
    if (!ide->current_file) {
        ide_save_file_as(ide);
        return;
    }
    
    GtkTextIter start, end;
    gtk_text_buffer_get_bounds(ide->source_buffer, &start, &end);
    gchar* content = gtk_text_buffer_get_text(ide->source_buffer, &start, &end, FALSE);
    
    FILE* file = fopen(ide->current_file, "w");
    if (!file) {
        GtkWidget* dialog = gtk_message_dialog_new(GTK_WINDOW(ide->window), 
                                                 GTK_DIALOG_DESTROY_WITH_PARENT, 
                                                 GTK_MESSAGE_ERROR, 
                                                 GTK_BUTTONS_OK, 
                                                 "Cannot save file: %s", ide->current_file);
        gtk_dialog_run(GTK_DIALOG(dialog));
        gtk_widget_destroy(dialog);
        g_free(content);
        return;
    }
    
    fprintf(file, "%s", content);
    fclose(file);
    g_free(content);
    
    ide->modified = 0;
    
    gchar* title = g_strdup_printf("KenC# IDE - %s", g_path_get_basename(ide->current_file));
    gtk_window_set_title(GTK_WINDOW(ide->window), title);
    g_free(title);
    
    ide_set_status(ide, g_strdup_printf("Saved file: %s", g_path_get_basename(ide->current_file)));
}

void ide_save_file_as(ide_window_t* ide) {
    GtkWidget* dialog = gtk_file_chooser_dialog_new("Save File As", 
                                                  GTK_WINDOW(ide->window), 
                                                  GTK_FILE_CHOOSER_ACTION_SAVE, 
                                                  GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL, 
                                                  GTK_STOCK_SAVE, GTK_RESPONSE_ACCEPT, 
                                                  NULL);
    
    GtkFileFilter* filter = gtk_file_filter_new();
    gtk_file_filter_set_name(filter, "KenC# Files (*.kenp)");
    gtk_file_filter_add_pattern(filter, "*.kenp");
    gtk_file_chooser_add_filter(GTK_FILE_CHOOSER(dialog), filter);
    
    if (ide->current_file) {
        gtk_file_chooser_set_filename(GTK_FILE_CHOOSER(dialog), ide->current_file);
    } else {
        gtk_file_chooser_set_current_name(GTK_FILE_CHOOSER(dialog), "untitled.kenp");
    }
    
    int response = gtk_dialog_run(GTK_DIALOG(dialog));
    if (response == GTK_RESPONSE_ACCEPT) {
        const char* filename = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(dialog));
        
        if (ide->current_file) {
            free(ide->current_file);
        }
        ide->current_file = g_strdup(filename);
        
        ide_save_file(ide);
    }
    
    gtk_widget_destroy(dialog);
}

void ide_close_file(ide_window_t* ide) {
    if (ide->modified) {
        GtkWidget* dialog = gtk_message_dialog_new(GTK_WINDOW(ide->window), 
                                                 GTK_DIALOG_DESTROY_WITH_PARENT, 
                                                 GTK_MESSAGE_WARNING, 
                                                 GTK_BUTTONS_YES_NO, 
                                                 "Save changes to current file?");
        int response = gtk_dialog_run(GTK_DIALOG(dialog));
        gtk_widget_destroy(dialog);
        
        if (response == GTK_RESPONSE_YES) {
            ide_save_file(ide);
        }
    }
    
    gtk_text_buffer_set_text(ide->source_buffer, "", 0);
    if (ide->current_file) {
        free(ide->current_file);
        ide->current_file = NULL;
    }
    ide->modified = 0;
    
    gtk_window_set_title(GTK_WINDOW(ide->window), "KenC# IDE");
    ide_set_status(ide, "File closed");
}

void ide_compile(ide_window_t* ide) {
    if (!ide->current_file) {
        ide_save_file_as(ide);
        if (!ide->current_file) {
            return;
        }
    } else if (ide->modified) {
        ide_save_file(ide);
    }
    
    ide_clear_output(ide);
    ide_set_status(ide, "Compiling...");
    
    gchar* command = g_strdup_printf("kenc -v %s", ide->current_file);
    FILE* pipe = popen(command, "r");
    if (!pipe) {
        ide_append_output(ide, "Error: Cannot execute compiler\n");
        ide_set_status(ide, "Compilation failed");
        g_free(command);
        return;
    }
    
    char buffer[1024];
    while (fgets(buffer, sizeof(buffer), pipe) != NULL) {
        ide_append_output(ide, buffer);
    }
    
    int status = pclose(pipe);
    if (status == 0) {
        ide_set_status(ide, "Compilation successful");
        
        /* Generate preview */
        gchar* ken_file = g_strdup_printf("%s.ken", g_path_get_basename(ide->current_file));
        FILE* ken = fopen(ken_file, "r");
        if (ken) {
            fseek(ken, 0, SEEK_END);
            long size = ftell(ken);
            fseek(ken, 0, SEEK_SET);
            
            char* content = (char*)malloc(size + 1);
            if (content) {
                fread(content, 1, size, ken);
                content[size] = '\0';
                gtk_text_buffer_set_text(ide->preview_buffer, content, -1);
                free(content);
            }
            fclose(ken);
        }
        g_free(ken_file);
    } else {
        ide_set_status(ide, "Compilation failed");
    }
    
    g_free(command);
}

void ide_run(ide_window_t* ide) {
    ide_compile(ide);
    
    if (!ide->current_file) {
        return;
    }
    
    gchar* ken_file = g_strdup_printf("%s.ken", g_path_get_basename(ide->current_file));
    gchar* command = g_strdup_printf("./%s", ken_file);
    
    ide_append_output(ide, "\nRunning program...\n");
    ide_set_status(ide, "Running...");
    
    FILE* pipe = popen(command, "r");
    if (!pipe) {
        ide_append_output(ide, "Error: Cannot run program\n");
        ide_set_status(ide, "Run failed");
        g_free(ken_file);
        g_free(command);
        return;
    }
    
    char buffer[1024];
    while (fgets(buffer, sizeof(buffer), pipe) != NULL) {
        ide_append_output(ide, buffer);
    }
    
    pclose(pipe);
    ide_set_status(ide, "Run completed");
    
    g_free(ken_file);
    g_free(command);
}

void ide_preview(ide_window_t* ide) {
    if (!ide->current_file) {
        ide_save_file_as(ide);
        if (!ide->current_file) {
            return;
        }
    } else if (ide->modified) {
        ide_save_file(ide);
    }
    
    gchar* ken_file = g_strdup_printf("%s.ken", g_path_get_basename(ide->current_file));
    FILE* ken = fopen(ken_file, "r");
    if (ken) {
        fseek(ken, 0, SEEK_END);
        long size = ftell(ken);
        fseek(ken, 0, SEEK_SET);
        
        char* content = (char*)malloc(size + 1);
        if (content) {
            fread(content, 1, size, ken);
            content[size] = '\0';
            gtk_text_buffer_set_text(ide->preview_buffer, content, -1);
            free(content);
            ide_set_status(ide, "Preview generated");
        }
        fclose(ken);
    } else {
        ide_append_output(ide, "Error: Cannot open generated file\n");
        ide_set_status(ide, "Preview failed");
    }
    
    g_free(ken_file);
}

void ide_show_about(ide_window_t* ide) {
    GtkWidget* dialog = gtk_about_dialog_new();
    gtk_about_dialog_set_program_name(GTK_ABOUT_DIALOG(dialog), "KenC# IDE");
    gtk_about_dialog_set_version(GTK_ABOUT_DIALOG(dialog), "1.0.0");
    gtk_about_dialog_set_copyright(GTK_ABOUT_DIALOG(dialog), "Copyright (c) 2026 KenuxOS Project");
    gtk_about_dialog_set_comments(GTK_ABOUT_DIALOG(dialog), 
                                 "Graphical IDE for KenC# Programming Language\n" 
                                 "Supports syntax highlighting, compilation, and preview");
    
    gtk_dialog_run(GTK_DIALOG(dialog));
    gtk_widget_destroy(dialog);
}

void ide_show_help(ide_window_t* ide) {
    GtkWidget* dialog = gtk_message_dialog_new(GTK_WINDOW(ide->window), 
                                             GTK_DIALOG_DESTROY_WITH_PARENT, 
                                             GTK_MESSAGE_INFO, 
                                             GTK_BUTTONS_OK, 
                                             "KenC# IDE Help\n\n" 
                                             "- File > New: Create new file\n" 
                                             "- File > Open: Open existing file\n" 
                                             "- File > Save: Save current file\n" 
                                             "- Build > Compile: Compile current file\n" 
                                             "- Build > Run: Compile and run current file\n" 
                                             "- Build > Preview: Show generated code\n");
    
    gtk_dialog_run(GTK_DIALOG(dialog));
    gtk_widget_destroy(dialog);
}

void ide_append_output(ide_window_t* ide, const char* text) {
    GtkTextIter end;
    gtk_text_buffer_get_end_iter(ide->output_buffer, &end);
    gtk_text_buffer_insert(ide->output_buffer, &end, text, -1);
}

void ide_clear_output(ide_window_t* ide) {
    gtk_text_buffer_set_text(ide->output_buffer, "", 0);
}

void ide_set_status(ide_window_t* ide, const char* status) {
    guint context_id = gtk_statusbar_get_context_id(GTK_STATUSBAR(ide->statusbar), "status");
    gtk_statusbar_pop(GTK_STATUSBAR(ide->statusbar), context_id);
    gtk_statusbar_push(GTK_STATUSBAR(ide->statusbar), context_id, status);
}

static void on_new_file(GtkWidget* widget, gpointer data) {
    ide_new_file((ide_window_t*)data);
}

static void on_open_file(GtkWidget* widget, gpointer data) {
    ide_open_file((ide_window_t*)data, NULL);
}

static void on_save_file(GtkWidget* widget, gpointer data) {
    ide_save_file((ide_window_t*)data);
}

static void on_save_file_as(GtkWidget* widget, gpointer data) {
    ide_save_file_as((ide_window_t*)data);
}

static void on_close_file(GtkWidget* widget, gpointer data) {
    ide_close_file((ide_window_t*)data);
}

static void on_compile(GtkWidget* widget, gpointer data) {
    ide_compile((ide_window_t*)data);
}

static void on_run(GtkWidget* widget, gpointer data) {
    ide_run((ide_window_t*)data);
}

static void on_preview(GtkWidget* widget, gpointer data) {
    ide_preview((ide_window_t*)data);
}

static void on_about(GtkWidget* widget, gpointer data) {
    ide_show_about((ide_window_t*)data);
}

static void on_help(GtkWidget* widget, gpointer data) {
    ide_show_help((ide_window_t*)data);
}

static void on_text_changed(GtkTextBuffer* buffer, gpointer data) {
    ide_window_t* ide = (ide_window_t*)data;
    ide->modified = 1;
    
    if (ide->current_file) {
        gchar* title = g_strdup_printf("KenC# IDE - %s*", g_path_get_basename(ide->current_file));
        gtk_window_set_title(GTK_WINDOW(ide->window), title);
        g_free(title);
    } else {
        gtk_window_set_title(GTK_WINDOW(ide->window), "KenC# IDE - Untitled*");
    }
}

static void on_window_destroy(GtkWidget* widget, gpointer data) {
    ide_window_t* ide = (ide_window_t*)data;
    ide_free(ide);
    gtk_main_quit();
}

int main(int argc, char** argv) {
    ide_window_t* ide = ide_create();
    if (!ide) {
        fprintf(stderr, "Error: Failed to create IDE window\n");
        return 1;
    }
    
    ide_show(ide);
    return 0;
}