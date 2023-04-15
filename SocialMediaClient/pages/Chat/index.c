static GtkBuilder *builder;
static GtkWidget *window;
static GtkCssProvider *provider;
static GtkListBox *listbox;
static GtkWidget *input_message;
static GtkWidget *label_mail_partner;

static void
add_item_to_list_box(const char *message, int message_type);
static void handle_back_chat()
{
    pthread_cancel(ID_THREAD_SOCKET);
    gtk_widget_destroy(window);
    closesocket(server_fd);
    gtk_main_quit();
    PROCESSINIT();
}

static void notify_offline()
{
    gtk_label_set_text(GTK_LABEL(label_mail_partner), "User are not online, messages will not send");
    gtk_widget_show_all(label_mail_partner);
}

static void handle_back_to_everyone()
{
    pthread_cancel(ID_THREAD_SOCKET);
    closesocket(server_fd);
    gtk_widget_destroy(window);
    gtk_main_quit();
    Everyone();
}

static void btn_send_message_clicked(GtkWidget *e)
{
    const char *message_send = gtk_entry_get_text((GTK_ENTRY(input_message)));
    if (send_data_socket(message_send, mail_selected) == 0)
    {
        add_item_to_list_box(message_send, 1);
        gtk_entry_set_text((GTK_ENTRY(input_message)), "");
        gtk_widget_show_all(GTK_WIDGET(listbox));
    }
}

void recieve_message(const char *recieve_message_text)
{
    add_item_to_list_box(recieve_message_text, 0);
    gtk_widget_show_all(GTK_WIDGET(listbox));
}

static void add_item_to_list_box(const char *message, int message_type)
{
    GtkWidget *label = gtk_label_new(message);
    GtkWidget *row = gtk_list_box_row_new();
    GtkStyleContext *context = gtk_widget_get_style_context(row);
    gtk_style_context_add_class(context, "container_message");

    if (message_type == 0)
    {
        gtk_widget_set_halign(row, GTK_ALIGN_START);
        gtk_style_context_add_class(context, "partner_message");
    }
    else
    {

        gtk_widget_set_halign(row, GTK_ALIGN_END);
        gtk_style_context_add_class(context, "my_message");
    }
    gtk_label_set_line_wrap(GTK_LABEL(label), TRUE);
    gtk_label_set_line_wrap_mode(GTK_LABEL(label), PANGO_WRAP_WORD);
    gtk_container_add(GTK_CONTAINER(row), label);
    gtk_list_box_insert(listbox, row, -1);
}

int Chat()
{

    builder = gtk_builder_new_from_file("./pages/Chat/Chat.glade");

    window = GTK_WIDGET(gtk_builder_get_object(builder, "chat_page"));

    provider = gtk_css_provider_new();
    gtk_css_provider_load_from_path(provider, "./pages/Chat/style.css", NULL);
    GtkWidget *btn_back_dashboard = GTK_WIDGET(gtk_builder_get_object(builder, "btn_back_dashboard"));
    GtkWidget *btn_back_everyone = GTK_WIDGET(gtk_builder_get_object(builder, "btn_back_everyone"));

    label_mail_partner = GTK_WIDGET(gtk_builder_get_object(builder, "mail_partner"));
    gtk_label_set_text(GTK_LABEL(label_mail_partner), mail_selected);
    GtkWidget *box = GTK_WIDGET(gtk_builder_get_object(builder, "box"));
    GtkWidget *container_list_message = GTK_WIDGET(gtk_builder_get_object(builder, "container_list_message"));
    input_message = GTK_WIDGET(gtk_builder_get_object(builder, "input_message"));
    GtkWidget *btn_send_message = GTK_WIDGET(gtk_builder_get_object(builder, "btn_send_message"));

    listbox = GTK_LIST_BOX(container_list_message);

    css_set(window, provider);
    gtk_style_context_add_provider_for_screen(gdk_screen_get_default(), GTK_STYLE_PROVIDER(provider), GTK_STYLE_PROVIDER_PRIORITY_USER);

    g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);
    g_signal_connect(btn_send_message, "clicked", G_CALLBACK(btn_send_message_clicked), NULL);
    g_signal_connect(btn_back_dashboard, "clicked", G_CALLBACK(handle_back_chat), NULL);
    g_signal_connect(btn_back_everyone, "clicked", G_CALLBACK(handle_back_to_everyone), NULL);
    gtk_window_maximize(GTK_WINDOW(window));
    gtk_widget_show_all(window);
    Connect_socket();
    gtk_main();
}
