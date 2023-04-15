static GtkBuilder *builder;
static GtkWidget *window;
static GtkWidget *cancel_button;
static GtkWidget *cancel1_button;
static GtkWidget *create_button;
static GtkWidget *edit_button;
static GtkWidget *profile;
static GtkWidget *create_a_post_btn;
static GtkWidget *dialog_create;
static GtkWidget *dialog_edit;
static GtkListBox *list_post;
static GtkWidget *text_view;
static GtkWidget *text_edit;
static GtkWidget *noti_personal;
static GtkWidget *overlay_personal;
static int test = 1;
static void add_post_to_list_box(const char *content, const char *ID);
static void on_click_create_button();
static void on_click_edit_button(GtkWidget *, gpointer);
static void on_destroy();
static gboolean hide_dialog();
static void create_a_post_button_click();
static void on_delete_button_click(GtkWidget *, gpointer);
static void on_edit_button_click(GtkWidget *, gpointer);
static void create_a_post_button_click();
static void reset_page();
int Personal();

typedef struct
{
    char ID[100];
    char content[4096];
} Editdata;

typedef struct
{
    char ID[100];
    char content[4096];
    char *dir;
    GtkWidget *w;
} Post;

static void handle_back_personal()
{
    gtk_widget_destroy(window);
    gtk_main_quit();
    PROCESSINIT();
}

void edit_content_post(GtkWidget *widget, gpointer data)
{
    if (GTK_IS_LABEL(widget) && gtk_label_get_current_uri(GTK_LABEL(widget)) == NULL && strcmp(gtk_label_get_text(GTK_LABEL(widget)), "EDIT") != 0 && strcmp(gtk_label_get_text(GTK_LABEL(widget)), "DELETE") != 0)
    {
        // printf("%s",gtk_label_get_text(GTK_LABEL(widget)));

        gtk_label_set_text(GTK_LABEL(widget), data);

        GtkTextBuffer *text_buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(text_edit));
        gtk_text_buffer_set_text(text_buffer, data, -1);

        gtk_widget_show_all(widget);
        gtk_widget_show_all(text_edit);
    }
    else if (GTK_IS_CONTAINER(widget))
    {
        gtk_container_foreach(GTK_CONTAINER(widget), (GtkCallback)edit_content_post, data);
    }
}

static void reset_page()
{
    gtk_widget_destroy(window);
    gtk_widget_destroy(dialog_create);
    gtk_widget_destroy(dialog_edit);
    Personal();
}

static gboolean hide_label(GtkWidget *e)
{
    gtk_widget_hide(e);
    return G_SOURCE_REMOVE;
}

static gboolean hide_dialog()
{
    gtk_widget_hide(dialog_create);
    GtkTextBuffer *buffer_1 = gtk_text_view_get_buffer(GTK_TEXT_VIEW(text_view));
    GtkTextIter start_iter, end_iter;
    gtk_text_buffer_get_bounds(buffer_1, &start_iter, &end_iter);
    gtk_text_buffer_delete(buffer_1, &start_iter, &end_iter);

    gtk_widget_hide(dialog_edit);

    return TRUE;
}

static void on_click_create_button()
{
    GtkTextBuffer *text_buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(text_view));
    GtkTextIter start_iter, end_iter;
    gtk_text_buffer_get_bounds(text_buffer, &start_iter, &end_iter);

    char *text = gtk_text_buffer_get_text(text_buffer, &start_iter, &end_iter, FALSE);

    char data[4096];
    sprintf(data, "{\"userId\": \"%s\", \"content\":\"%s\", \"mail\":\"%s\"}", USER.id, text, USER.mail);

    // printf("creating post...\n");
    char *response = request("create-post", data);
    json_object *root = json_tokener_parse(response);

    char *markup = "";

    if (root == NULL)
    {
        // printf("Loi khi doc JSON\n");
        markup = "<span size='large' foreground='#FF0000'>Something is wrong, please wait!</span>";
        gtk_label_set_markup(GTK_LABEL(noti_personal), markup);
        gtk_widget_show(noti_personal);
        g_timeout_add_seconds(2, (GSourceFunc)hide_label, GTK_LABEL(noti_personal));
    }
    else
    {
        const char *error = json_object_get_string(json_object_object_get(root, "error"));
        const char *id = json_object_get_string(json_object_object_get(root, "postId"));
        if (error != NULL)
        {
            char error_status[1000];
            // printf("have error");
            sprintf(error_status, "<span size='large' foreground='#FF0000'>%s</span>", error);
            gtk_label_set_markup(GTK_LABEL(noti_personal), error_status);
            gtk_widget_show(noti_personal);
            g_timeout_add_seconds(2, (GSourceFunc)hide_label, GTK_LABEL(noti_personal));
        }
        else
        {
            markup = "<span size='large' foreground='#00FF00'>Post created successfully</span>";
            gtk_label_set_markup(GTK_LABEL(noti_personal), markup);
            gtk_widget_show(noti_personal);
            g_timeout_add_seconds(2, (GSourceFunc)hide_label, GTK_LABEL(noti_personal));

            add_post_to_list_box(text, id);
        }
    }
    gtk_widget_show_all(GTK_WIDGET(list_post));
    hide_dialog();
    json_object_put(root);
    g_free(text);
}

static void create_a_post_button_click()
{
    gtk_widget_show_all(dialog_create);
}

static void on_click_edit_button(GtkWidget *e, gpointer post_pointer)
{
    Post *post_data = post_pointer;
    char *id = post_data->ID;
    char *content = post_data->content;
    char *dir = post_data->dir;

    GtkWidget *widget = post_data->w;
    GtkTextBuffer *text_buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(text_edit));
    GtkTextIter start_iter, end_iter;
    gtk_text_buffer_get_bounds(text_buffer, &start_iter, &end_iter);
    char *text = gtk_text_buffer_get_text(text_buffer, &start_iter, &end_iter, FALSE);
    content = text;
    char data[5000];
    sprintf(data, "{\"_id\":\"%s\",\"content\":\"%s\"}", id, text);
    // printf("Editing, Please Wait\n");
    char *response = request("edit-post", data);

    char *markup = "";
    hide_dialog();

    json_object *root = json_tokener_parse(response);
    if (root == NULL)
    {
        // printf("Loi khi doc JSON\n");
        markup = "<span size='large' foreground='#FF0000'>Something is wrong, please wait!</span>";
        gtk_label_set_markup(GTK_LABEL(noti_personal), markup);
        gtk_widget_show(noti_personal);
        g_timeout_add_seconds(2, (GSourceFunc)hide_label, GTK_LABEL(noti_personal));
    }

    const char *error = json_object_get_string(json_object_object_get(root, "error"));
    // printf("error: %s\n", error);
    if (error != NULL)
    {
        char error_status[1000];
        // printf("have error");
        sprintf(error_status, "<span size='large' foreground='#FF0000'>%s</span>", error);
        gtk_label_set_markup(GTK_LABEL(noti_personal), error_status);
        gtk_widget_show(noti_personal);
        g_timeout_add_seconds(2, (GSourceFunc)hide_label, GTK_LABEL(noti_personal));
    }
    else
    {
        markup = "<span size='large' foreground='#00FF00'>Post edited successfully</span>";
        gtk_label_set_markup(GTK_LABEL(noti_personal), markup);
        gtk_widget_show(noti_personal);
        g_timeout_add_seconds(2, (GSourceFunc)hide_label, GTK_LABEL(noti_personal));
        GtkWidget *parent = gtk_widget_get_parent(widget);
        GtkWidget *container = gtk_widget_get_parent(parent);
        strcpy(dir, text);
        edit_content_post(container, text);
    }
    gtk_widget_show_all(GTK_WIDGET(list_post));
    json_object_put(root);
    g_free(text);
}

static void on_edit_button_click(GtkWidget *e, gpointer edit_post_pointer)
{
    Editdata *edit_post = edit_post_pointer;
    char *ID = edit_post->ID;
    char *content = edit_post->content;

    GtkTextBuffer *text_buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(text_edit));
    gtk_text_buffer_set_text(text_buffer, content, -1);

    // printf("content: %s\n",content);

    gtk_widget_show_all(dialog_edit);
    Post *post_data = malloc(sizeof(Post));
    strcpy(post_data->ID, ID);
    strcpy(post_data->content, content);
    post_data->dir = edit_post->content;
    post_data->w = e;

    g_signal_handlers_destroy(edit_button);
    g_signal_connect(edit_button, "clicked", G_CALLBACK(on_click_edit_button), (gpointer)post_data);
}

static void on_delete_button_click(GtkWidget *e, gpointer edit_post_pointer)
{
    Editdata *edit_post = edit_post_pointer;
    char *id = edit_post->ID;
    printf("%s\n",id);
    char data[100];
    sprintf(data, "{\"_id\":\"%s\"}", id);
    // printf("Deleting, Please Wait\n");
    char *response = request("delete-post", data);

    char *markup = "";
    hide_dialog();

    json_object *root = json_tokener_parse(response);
    if (root == NULL)
    {
        // printf("Loi khi doc JSON\n");
        markup = "<span size='large' foreground='#FF0000'>Something is wrong, please wait!</span>";
        gtk_label_set_markup(GTK_LABEL(noti_personal), markup);
        gtk_widget_show(noti_personal);
        g_timeout_add_seconds(2, (GSourceFunc)hide_label, GTK_LABEL(noti_personal));
    }

    const char *error = json_object_get_string(json_object_object_get(root, "error"));
    // printf("error: %s\n", error);
    if (error != NULL)
    {
        char error_status[1000];
        // printf("have error");
        sprintf(error_status, "<span size='large' foreground='#FF0000'>%s</span>", error);
        gtk_label_set_markup(GTK_LABEL(noti_personal), error_status);
        gtk_widget_show(noti_personal);
        g_timeout_add_seconds(2, (GSourceFunc)hide_label, GTK_LABEL(noti_personal));
    }
    else
    {
        markup = "<span size='large' foreground='#00FF00'>Post deleted successfully</span>";
        gtk_label_set_markup(GTK_LABEL(noti_personal), markup);
        gtk_widget_show(noti_personal);
        g_timeout_add_seconds(2, (GSourceFunc)hide_label, GTK_LABEL(noti_personal));
        GtkWidget *parent = gtk_widget_get_parent(e);
        GtkWidget *container = gtk_widget_get_parent(gtk_widget_get_parent(parent));
        gtk_widget_destroy(container);
    }
    gtk_widget_show_all(GTK_WIDGET(list_post));
    json_object_put(root);
}

static void add_post_to_list_box(const char *content, const char *ID)
{
    GtkWidget *row = gtk_list_box_row_new();
    GtkWidget *label_content = gtk_label_new(content);
    GtkWidget *box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 2);
    GtkWidget *box_group_btn = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 2);
    GtkWidget *edit_btn = gtk_button_new_with_label("EDIT");
    GtkWidget *delete_btn = gtk_button_new_with_label("DELETE");
    GtkWidget *btn_back_dashboard = GTK_WIDGET(gtk_builder_get_object(builder, "btn_back_dashboard"));

    GtkStyleContext *context = gtk_widget_get_style_context(row);
    GtkStyleContext *context_label_content = gtk_widget_get_style_context(label_content);
    GtkStyleContext *context_edit_btn = gtk_widget_get_style_context(edit_btn);
    GtkStyleContext *context_delete_btn = gtk_widget_get_style_context(delete_btn);
    gtk_style_context_add_class(context, "post");
    gtk_style_context_add_class(context_edit_btn, "h2");
    gtk_style_context_add_class(context_delete_btn, "h2");
    gtk_style_context_add_class(context_label_content, "body");

    gtk_box_set_homogeneous(GTK_BOX(box_group_btn), TRUE);
    gtk_box_pack_start(GTK_BOX(box_group_btn), edit_btn, FALSE, TRUE, 0);
    gtk_box_pack_start(GTK_BOX(box_group_btn), delete_btn, FALSE, TRUE, 0);

    gtk_label_set_line_wrap(GTK_LABEL(label_content), TRUE);
    gtk_label_set_line_wrap_mode(GTK_LABEL(label_content), PANGO_WRAP_WORD);

    gtk_box_pack_start(GTK_BOX(box), box_group_btn, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(box), label_content, FALSE, FALSE, 0);

    gtk_widget_set_halign(row, GTK_ALIGN_CENTER);
    gtk_widget_set_valign(row, GTK_ALIGN_CENTER);
    gtk_container_add(GTK_CONTAINER(row), box);

    Editdata *edit_post = malloc(sizeof(Editdata));
    strcpy(edit_post->ID, ID);
    strcpy(edit_post->content, content);

    g_signal_connect(delete_btn, "clicked", G_CALLBACK(on_delete_button_click), (gpointer)edit_post);
    g_signal_connect(edit_btn, "clicked", G_CALLBACK(on_edit_button_click), (gpointer)edit_post);

    gtk_list_box_prepend(list_post, row);
    gtk_widget_show_all(GTK_WIDGET(list_post));
}

int Personal()
{

    builder = gtk_builder_new_from_file("./pages/Personal/Personal.glade");

    window = GTK_WIDGET(gtk_builder_get_object(builder, "personal"));
    GtkWidget *btn_back_dashboard = GTK_WIDGET(gtk_builder_get_object(builder, "btn_back_dashboard"));

    dialog_create = GTK_WIDGET(gtk_builder_get_object(builder, "create_post_dialog"));

    dialog_edit = GTK_WIDGET(gtk_builder_get_object(builder, "edit_post_dialog"));

    cancel_button = GTK_WIDGET(gtk_builder_get_object(builder, "btn_cancel"));

    cancel1_button = GTK_WIDGET(gtk_builder_get_object(builder, "btn_cancel1"));

    create_button = GTK_WIDGET(gtk_builder_get_object(builder, "btn_create"));

    edit_button = GTK_WIDGET(gtk_builder_get_object(builder, "btn_edit"));

    create_a_post_btn = GTK_WIDGET(gtk_builder_get_object(builder, "create_a_post_button"));

    profile = GTK_WIDGET(gtk_builder_get_object(builder, "profile"));

    text_view = GTK_WIDGET(gtk_builder_get_object(builder, "text_view_post"));

    text_edit = GTK_WIDGET(gtk_builder_get_object(builder, "text_edit_post"));

    noti_personal = GTK_WIDGET(gtk_builder_get_object(builder, "noti_personal"));

    overlay_personal = GTK_WIDGET(gtk_builder_get_object(builder, "overlay_personal"));

    GtkWidget *list_box = GTK_WIDGET(gtk_builder_get_object(builder, "list_box"));
    list_post = GTK_LIST_BOX(list_box);

    char data[100];
    sprintf(data, "{\"userId\": \"%s\"}", USER.id);
    // printf("Loading personal, Please Wait\n");
    char *response = request("post", data);

    json_object *root = json_tokener_parse(response);
    json_object *post_obj = json_object_object_get(root, "posts");
    int post_array_len = json_object_array_length(post_obj);
    for (int i = 0; i < post_array_len; i++)
    {
        json_object *post_string = json_object_array_get_idx(post_obj, i);
        json_object *post_json = json_tokener_parse(json_object_get_string(post_string));
        json_object *content_post;
        json_object *id_obj;
        json_object *oid_obj;
        json_object_object_get_ex(post_json, "content", &content_post);
        json_object_object_get_ex(post_json, "_id", &id_obj);
        json_object_object_get_ex(id_obj, "$oid", &oid_obj);
        // printf("%s\n%s\n====================\n", json_object_get_string(content_post), json_object_get_string(oid_obj));
        add_post_to_list_box(json_object_get_string(content_post), json_object_get_string(oid_obj));
    }
    json_object_put(root);

    GtkCssProvider *provider = gtk_css_provider_new();
    gtk_css_provider_load_from_path(provider, "./pages/Personal/style.css", NULL);
    css_set(window, provider);
    gtk_style_context_add_provider_for_screen(gdk_screen_get_default(), GTK_STYLE_PROVIDER(provider), GTK_STYLE_PROVIDER_PRIORITY_USER);

    g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);
    g_signal_connect(create_a_post_btn, "clicked", G_CALLBACK(create_a_post_button_click), NULL);
    g_signal_connect(cancel_button, "clicked", G_CALLBACK(hide_dialog), NULL);
    g_signal_connect(cancel1_button, "clicked", G_CALLBACK(hide_dialog), NULL);
    g_signal_connect(create_button, "clicked", G_CALLBACK(on_click_create_button), NULL);
    g_signal_connect(dialog_create, "delete_event", G_CALLBACK(hide_dialog), NULL);
    g_signal_connect(dialog_edit, "delete_event", G_CALLBACK(hide_dialog), NULL);
    g_signal_connect(btn_back_dashboard, "clicked", G_CALLBACK(handle_back_personal), NULL);

    gtk_window_maximize(GTK_WINDOW(window));
    gtk_widget_show_all(window);
    gtk_main();
    return EXIT_SUCCESS;
}
