void PROCESSINIT()
{
    if (LOGINED == 0)
    {
        Auth();
    }
    else
    {
        Dashboard();
    }
}

void css_set(GtkWidget *g_widget, GtkCssProvider *provider)
{
    GtkStyleContext *context = gtk_widget_get_style_context(g_widget);
    gtk_style_context_add_provider(context, GTK_STYLE_PROVIDER(provider), GTK_STYLE_PROVIDER_PRIORITY_USER);
}
