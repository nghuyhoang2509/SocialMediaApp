static void add_mail_to_list(const char *mail);

static GtkListBox *list_mail;
static GtkWidget *window;

static void handle_back_everyone()
{
    gtk_widget_destroy(window);
    gtk_main_quit();
    PROCESSINIT();
}

static void on_row_click(GtkWidget *list, GtkWidget *row)
{
    GtkLabel *label_mail_everyone = GTK_LABEL(gtk_bin_get_child(GTK_BIN(row)));
    const char *mail = gtk_label_get_text(label_mail_everyone);
    gtk_main_quit();
    gtk_widget_destroy(window);
    strcpy(mail_selected, mail);
    Chat();
}

static void add_mail_to_list(const char *mail)
{
    if (strstr(USER.mail, mail) == NULL)
    {
        GtkWidget *label = gtk_label_new(mail);
        GtkWidget *row = gtk_list_box_row_new();
        GtkStyleContext *context = gtk_widget_get_style_context(row);
        gtk_style_context_add_class(context, "container_item");

        gtk_container_add(GTK_CONTAINER(row), label);
        gtk_list_box_insert(list_mail, row, -1);
    }
}

int Everyone()
{
    char data[2];
    sprintf(data, "{}");
    char *response = request("everyone", data);
    json_object *root = json_tokener_parse(response);
    json_object *mail_obj = json_object_object_get(root, "mail");
    int mail_array_len = json_object_array_length(mail_obj);

    GtkBuilder *builder = gtk_builder_new_from_file("./pages/Everyone/Everyone.glade");

    window = GTK_WIDGET(gtk_builder_get_object(builder, "everyone_page"));
    GtkWidget *btn_back_dashboard = GTK_WIDGET(gtk_builder_get_object(builder, "btn_back_dashboard"));

    GtkCssProvider *provider = gtk_css_provider_new();
    gtk_css_provider_load_from_path(provider, "./pages/Everyone/style.css", NULL);

    GtkWidget *label_name = GTK_WIDGET(gtk_builder_get_object(builder, "label_name"));
    GtkWidget *list_box = GTK_WIDGET(gtk_builder_get_object(builder, "list_box"));

    list_mail = GTK_LIST_BOX(list_box);
    for (int i = 0; i < mail_array_len; i++)
    {
        json_object *mail_value = json_object_array_get_idx(mail_obj, i);
        add_mail_to_list(json_object_get_string(mail_value));
    }
    json_object_put(root);

    css_set(window, provider);
    gtk_style_context_add_provider_for_screen(gdk_screen_get_default(), GTK_STYLE_PROVIDER(provider), GTK_STYLE_PROVIDER_PRIORITY_USER);

    g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);
    g_signal_connect(btn_back_dashboard, "clicked", G_CALLBACK(handle_back_everyone), NULL);
    g_signal_connect(list_box, "row-activated", G_CALLBACK(on_row_click), NULL);

    gtk_window_maximize(GTK_WINDOW(window));

    gtk_widget_show_all(window);

    gtk_main();

    return EXIT_SUCCESS;
}