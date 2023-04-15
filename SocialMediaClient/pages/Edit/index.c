static GtkBuilder *builder;
static GtkWidget *window;
static GtkEntry *password_edit;
static GtkEntry *fullname_edit;
static GtkWidget *edit_check;
static GtkWidget *save_btn;

static const char *PASSWORD;
static const char *FULLNAME;
static const char *EMAIL;
static const char *ID = USER.id;

static void handle_back_edit()
{
    gtk_widget_destroy(window);
    gtk_main_quit();
    PROCESSINIT();
}

static void on_entry_insert_spacebar_handle(GtkWidget *e)
{
    const gchar *text = gtk_entry_get_text(GTK_ENTRY(e));
    gint length = strlen(text);
    for (gint i = 0; i < length; i++)
    {
        if (text[i] == ' ')
        {
            // Không chấp nhận ký tự Space, xoá nó ra khỏi chuỗi đầu vào
            GtkEntryBuffer *buffer = gtk_entry_get_buffer(GTK_ENTRY(e));
            gtk_editable_delete_text(GTK_EDITABLE(GTK_ENTRY(e)), i, i + 1);
        }
    }
}

static void on_save_btn_clicked(GtkWidget *e)
{
    char *markup = "";
    const gpointer fullname = (const gpointer)gtk_entry_get_text(fullname_edit);
    const gpointer pass = (const gpointer)gtk_entry_get_text(password_edit);
    if (strlen(fullname) == 0 || strlen(pass) == 0)
    {
        markup = "<span foreground='#FF0000'>Please, fill in all the information!</span>";
        gtk_label_set_markup(GTK_LABEL(edit_check), markup);
        return;
    }
    else if (strlen(pass) < 8)
    {
        markup = "<span foreground='#FF0000'>Password must be 8 characters or more!</span>";
        gtk_label_set_markup(GTK_LABEL(edit_check), markup);
        return;
    }

    char data[1000];
    sprintf(data, "{\"_id\":\"%s\", \"password\":\"%s\", \"fullname\":\"%s\"}", ID, pass, fullname);

    char *response = request("edit-account", data);
    printf("%s\n", response);
    json_object *root = json_tokener_parse(response);
    if (root == NULL)
    {
        // printf("Loi khi doc JSON\n");
        markup = "<span foreground='#FF0000'>Something is wrong, please wait!</span>";
        gtk_label_set_markup(GTK_LABEL(edit_check), markup);
    }
    else
    {
        // printf("%s\n", response);

        const char *error = json_object_get_string(json_object_object_get(root, "error"));
        // printf("error: %s\n", error);

        if (error != NULL)
        {
            char error_status[1000];
            sprintf(error_status, "<span foreground='#FF0000'>%s</span>", error);
            gtk_label_set_markup(GTK_LABEL(edit_check), error_status);
        }
        else
        {

            ID = json_object_get_string(json_object_object_get(root, "_id"));

            FULLNAME = json_object_get_string(json_object_object_get(root, "fullname"));

            PASSWORD = json_object_get_string(json_object_object_get(root, "password"));

            if (ID != NULL)
            {
                char success_status[1000];
                sprintf(success_status, "<span foreground='#00FF00'>%s</span>", "Edit personal information done!");
                gtk_label_set_markup(GTK_LABEL(edit_check), success_status);
            }
            else
            {
                markup = "<span foreground='#FF0000'>Something is wrong, please wait!</span>";
                gtk_label_set_markup(GTK_LABEL(edit_check), markup);
            }
        }
        json_object_put(root);
    }
}

int Edit()
{

    builder = gtk_builder_new_from_file("./pages/Edit/Edit.glade");

    window = GTK_WIDGET(gtk_builder_get_object(builder, "edit_window"));
    password_edit = GTK_ENTRY(gtk_builder_get_object(builder, "password_edit"));
    fullname_edit = GTK_ENTRY(gtk_builder_get_object(builder, "fullname_edit"));
    edit_check = GTK_WIDGET(gtk_builder_get_object(builder, "edit_check"));
    save_btn = GTK_WIDGET(gtk_builder_get_object(builder, "save_btn"));
    GtkWidget *btn_back_dashboard = GTK_WIDGET(gtk_builder_get_object(builder, "btn_back_dashboard"));

    GtkCssProvider *css_provider = gtk_css_provider_new();
    gtk_css_provider_load_from_path(css_provider, "./pages/Edit/style.css", NULL);
    css_set(window, css_provider);
    gtk_style_context_add_provider_for_screen(gdk_screen_get_default(), GTK_STYLE_PROVIDER(css_provider), GTK_STYLE_PROVIDER_PRIORITY_USER);

    g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);
    g_signal_connect(save_btn, "clicked", G_CALLBACK(on_save_btn_clicked), NULL);
    g_signal_connect(password_edit, "changed", G_CALLBACK(on_entry_insert_spacebar_handle), NULL);
    g_signal_connect(btn_back_dashboard, "clicked", G_CALLBACK(handle_back_edit), NULL);
    gtk_window_maximize(GTK_WINDOW(window));

    gtk_widget_show_all(window);
    gtk_main();

    return EXIT_SUCCESS;
}
