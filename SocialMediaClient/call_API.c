

struct memory_struct
{
    char *memory;
    size_t size;
};
static size_t write_memory_callback(void *contents, size_t size, size_t nmemb, void *userp)
{
    size_t realsize = size * nmemb;
    struct memory_struct *mem = (struct memory_struct *)userp;

    char *ptr = realloc(mem->memory, mem->size + realsize + 1);
    if (!ptr)
    {
        /* out of memory! */
        printf("not enough memory (realloc returned NULL)\n");
        return 0;
    }

    mem->memory = ptr;
    memcpy(&(mem->memory[mem->size]), contents, realsize);
    mem->size += realsize;
    mem->memory[mem->size] = 0;

    return realsize;
}
/*
    Cách sử dụng:
    * Thêm trong file tasks.json "${fileDirname}/call_API.c", vào trong args
    * Khai báo trong index.c: include <"call_API.h">
    * method: login, register, create-post, edit-post, delete-post, all-post, post, everyone
    * data là body của request. VD: {"mail":"huytest@gmail.com", "password":"okokokok1"}
    * VD cách sử dụng API login:

    char data[1000];

    sprintf(data, "{\"mail\":\"%s\", \"password\":\"%s\"}", email, pass);
    char *response = request("login", data);
    Ở đây response sẽ chứa json kết quả trả về

*/
char *request(char *method, char *data)
{
    CURL *curl;
    CURLcode res;

    // JSON data to be sent in the request body
    struct memory_struct chunk;

    chunk.memory = malloc(1); /* will be grown as needed by the realloc above */
    chunk.size = 0;
    // Initialize libcurl
    curl = curl_easy_init();
    curl_global_init(CURL_GLOBAL_ALL);
    if (curl)
    {
        // Set the URL for the request
        char URL[100];
        sprintf(URL, "http://nghuyhoang2509.click/%s", method);
        curl_easy_setopt(curl, CURLOPT_URL, URL);

        // Set the HTTP headers
        struct curl_slist *headers = NULL;
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

        // Set the request method to POST
        curl_easy_setopt(curl, CURLOPT_POST, 1L);

        // Set the request body
        curl_mime *mime;
        curl_mimepart *part;
        mime = curl_mime_init(curl);
        part = curl_mime_addpart(mime);
        curl_mime_name(part, "data");
        curl_mime_data(part, data, CURL_ZERO_TERMINATED);
        curl_easy_setopt(curl, CURLOPT_MIMEPOST, mime);

        // Set up the write callback function to handle the response
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_memory_callback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)&chunk);

        // Perform the request
        res = curl_easy_perform(curl);
        if (res != CURLE_OK)
            fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
        else
        {
            return chunk.memory;
            // printf("test: %s",chunk.memory);
        }

        // Clean up
        curl_mime_free(mime);
        curl_easy_cleanup(curl);
        curl_slist_free_all(headers);
    }
}
