static GtkBuilder *builder;
static GtkWidget *window;
static GtkWidget *sign_in_frame;
static GtkWidget *sign_up_frame;
static GtkEntry *email_sign_in;
static GtkEntry *email_sign_up;
static GtkEntry *password_sign_in;
static GtkEntry *password_sign_up;
static GtkEntry *fullname_sign_up;
static GtkWidget *sign_in_btn;
static GtkWidget *sign_up_btn;
static GtkWidget *sign_up_now_btn;
static GtkWidget *sign_in_now_btn;
static GtkWidget *sign_in_check;
static GtkWidget *sign_up_check;

static const char *PASSWORD;
static const char *FULLNAME;
static const char *EMAIL;
static const char *ID;

static int validate_email(char *email)
{
    char *pattern = "^[a-zA-Z0-9._%+-]+@[a-zA-Z0-9.-]+\\.[a-zA-Z]{2,}$";

    regex_t regex;
    int reti = regcomp(&regex, pattern, REG_EXTENDED);
    reti = regexec(&regex, email, 0, NULL, 0);
    if (!reti)
    {
        return 1;
    }
    else if (reti == REG_NOMATCH)
    {
        return 0;
    }

    regfree(&regex);
}

static void on_entry_insert_spacebar(GtkWidget *e)
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
static void on_sign_in_btn_clicked(GtkWidget *e)
{
    char *markup = "";
    const gpointer email = (const gpointer)gtk_entry_get_text(email_sign_in);
    const gpointer pass = (const gpointer)gtk_entry_get_text(password_sign_in);

    if (strlen(email) == 0 || strlen(pass) == 0)
    {
        markup = "<span size='large' foreground='#FF0000'>Please, fill in all the information!</span>";
        gtk_label_set_markup(GTK_LABEL(sign_in_check), markup);
        return;
    }
    else if (strlen(pass) < 8)
    {
        markup = "<span size='large' foreground='#FF0000'>Password must be 8 characters or more!</span>";
        gtk_label_set_markup(GTK_LABEL(sign_in_check), markup);
        return;
    }

    char data[1000];
    sprintf(data, "{\"mail\":\"%s\", \"password\":\"%s\"}", email, pass);

    char *response = request("login", data);

    json_object *root = json_tokener_parse(response);
    if (root == NULL)
    {
        // printf("Loi khi doc JSON\n");
        markup = "<span size='large' foreground='#FF0000'>Something is wrong, please wait!</span>";
        gtk_label_set_markup(GTK_LABEL(sign_in_check), markup);
    }
    else
    {
        // printf("%s\n", response);
        json_object *id_obj;
        if (json_object_object_get_ex(root, "_id", &id_obj))
        {
            json_object *oid_obj;
            if (json_object_object_get_ex(id_obj, "$oid", &oid_obj))
            {
                ID = json_object_get_string(oid_obj);
                // printf("_id: %s\n", ID);
                strcpy(USER.id, ID);
            }
        }

        const char *error = json_object_get_string(json_object_object_get(root, "error"));
        // printf("error: %s\n", error);
        if (error != NULL)
        {
            json_object_put(root);
            char error_status[1000];
            // printf("have error");
            sprintf(error_status, "<span size='large' foreground='#FF0000'>%s</span>", error);
            gtk_label_set_markup(GTK_LABEL(sign_in_check), error_status);
        }
        else
        {
            PASSWORD = json_object_get_string(json_object_object_get(root, "password"));
            // printf("password: %s\n", PASSWORD);

            FULLNAME = json_object_get_string(json_object_object_get(root, "fullname"));
            // printf("fullname: %s\n", FULLNAME);

            EMAIL = json_object_get_string(json_object_object_get(root, "mail"));
            // printf("mail: %s\n", EMAIL);

            json_object_put(root);

            strcpy(USER.fullname, FULLNAME);
            strcpy(USER.mail, EMAIL);
            markup = "<span size='large' foreground='#00FF00'>Sign in done</span>";
            gtk_label_set_markup(GTK_LABEL(sign_in_check), markup);
            // printf("Sign in done\n");
            LOGINED = 1;
            gtk_widget_destroy(window);
            PROCESSINIT();
        }
    }
}
static void on_sign_up_btn_clicked(GtkWidget *e)
{
    char *markup = "";
    const gpointer fullname = (const gpointer)gtk_entry_get_text(fullname_sign_up);
    const gpointer email = (const gpointer)gtk_entry_get_text(email_sign_up);
    const gpointer pass = (const gpointer)gtk_entry_get_text(password_sign_up);
    if (strlen(fullname) == 0 || strlen(email) == 0 || strlen(pass) == 0)
    {
        markup = "<span size='large' foreground='#FF0000'>Please, fill in all the information!</span>";
        gtk_label_set_markup(GTK_LABEL(sign_up_check), markup);
        return;
    }
    else if (!validate_email(email))
    {
        markup = "<span size='large' foreground='#FF0000'>Wrong email format!</span>";
        gtk_label_set_markup(GTK_LABEL(sign_up_check), markup);
        return;
    }
    else if (strlen(pass) < 8)
    {
        markup = "<span size='large' foreground='#FF0000'>Password must be 8 characters or more!</span>";
        gtk_label_set_markup(GTK_LABEL(sign_up_check), markup);
        return;
    }

    char data[1000];
    sprintf(data, "{\"mail\":\"%s\", \"password\":\"%s\", \"fullname\":\"%s\"}", email, pass, fullname);

    char *response = request("register", data);

    json_object *root = json_tokener_parse(response);
    if (root == NULL)
    {
        printf("Loi khi doc JSON\n");
        markup = "<span size='large' foreground='#FF0000'>Something is wrong, please wait!</span>";
        gtk_label_set_markup(GTK_LABEL(sign_up_check), markup);
    }
    else
    {
        printf("%s\n", response);

        const char *error = json_object_get_string(json_object_object_get(root, "error"));
        printf("error: %s\n", error);

        const char *success = json_object_get_string(json_object_object_get(root, "success"));
        printf("success: %s\n", success);

        json_object_put(root);

        if (error != NULL)
        {
            char error_status[1000];
            sprintf(error_status, "<span size='large' foreground='#FF0000'>%s</span>", error);
            gtk_label_set_markup(GTK_LABEL(sign_up_check), error_status);
        }
        else
        {
            char success_status[1000];
            sprintf(success_status, "<span size='large' foreground='#00FF00'>%s</span>", success);
            gtk_label_set_markup(GTK_LABEL(sign_up_check), success_status);
        }
    }
}
static void on_sign_up_now_btn_clicked(GtkWidget *e)
{

    gtk_entry_set_text(email_sign_in, "");
    gtk_entry_set_text(password_sign_in, "");
    gtk_label_set_text(GTK_LABEL(sign_in_check), "");
    gtk_widget_hide(sign_in_frame);
    gtk_widget_show(sign_up_frame);
}
static void on_sign_in_now_btn_clicked(GtkWidget *e)
{
    gtk_entry_set_text(email_sign_up, "");
    gtk_entry_set_text(password_sign_up, "");
    gtk_entry_set_text(fullname_sign_up, "");
    gtk_label_set_text(GTK_LABEL(sign_up_check), "");
    gtk_widget_hide(sign_up_frame);
    gtk_widget_show(sign_in_frame);
}

int Auth()
{

    builder = gtk_builder_new_from_file("./pages/Auth/Auth.glade");

    window = GTK_WIDGET(gtk_builder_get_object(builder, "window"));
    sign_in_frame = GTK_WIDGET(gtk_builder_get_object(builder, "sign_in_frame"));
    sign_up_frame = GTK_WIDGET(gtk_builder_get_object(builder, "sign_up_frame"));
    email_sign_in = GTK_ENTRY(gtk_builder_get_object(builder, "email_sign_in"));
    password_sign_in = GTK_ENTRY(gtk_builder_get_object(builder, "password_sign_in"));
    fullname_sign_up = GTK_ENTRY(gtk_builder_get_object(builder, "fullname_sign_up"));
    email_sign_up = GTK_ENTRY(gtk_builder_get_object(builder, "email_sign_up"));
    password_sign_up = GTK_ENTRY(gtk_builder_get_object(builder, "password_sign_up"));
    sign_in_btn = GTK_WIDGET(gtk_builder_get_object(builder, "sign_in_btn"));
    sign_up_btn = GTK_WIDGET(gtk_builder_get_object(builder, "sign_up_btn"));
    sign_up_now_btn = GTK_WIDGET(gtk_builder_get_object(builder, "sign_up_now_btn"));
    sign_in_now_btn = GTK_WIDGET(gtk_builder_get_object(builder, "sign_in_now_btn"));
    sign_in_check = GTK_WIDGET(gtk_builder_get_object(builder, "sign_in_check"));
    sign_up_check = GTK_WIDGET(gtk_builder_get_object(builder, "sign_up_check"));

    // add css
    GtkCssProvider *css_provider = gtk_css_provider_new();
    gtk_css_provider_load_from_path(css_provider, "./pages/Auth/style.css", NULL);
    css_set(window, css_provider);
    gtk_style_context_add_provider_for_screen(gdk_screen_get_default(), GTK_STYLE_PROVIDER(css_provider), GTK_STYLE_PROVIDER_PRIORITY_USER);

    g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), window);
    g_signal_connect(sign_in_btn, "clicked", G_CALLBACK(on_sign_in_btn_clicked), NULL);
    g_signal_connect(sign_up_btn, "clicked", G_CALLBACK(on_sign_up_btn_clicked), NULL);
    g_signal_connect(sign_up_now_btn, "clicked", G_CALLBACK(on_sign_up_now_btn_clicked), NULL);
    g_signal_connect(sign_in_now_btn, "clicked", G_CALLBACK(on_sign_in_now_btn_clicked), NULL);
    g_signal_connect(password_sign_in, "changed", G_CALLBACK(on_entry_insert_spacebar), NULL);
    g_signal_connect(email_sign_in, "changed", G_CALLBACK(on_entry_insert_spacebar), NULL);
    g_signal_connect(email_sign_up, "changed", G_CALLBACK(on_entry_insert_spacebar), NULL);
    g_signal_connect(password_sign_up, "changed", G_CALLBACK(on_entry_insert_spacebar), NULL);

    gtk_widget_show(window);
    gtk_builder_connect_signals(builder, NULL);
    gtk_window_maximize(GTK_WINDOW(window));

    gtk_main();

    return EXIT_SUCCESS;
}
