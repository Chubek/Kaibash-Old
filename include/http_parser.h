typedef struct {
    char name[100];
    char value[10000];
} http_header_s;

typedef struct {
    http_header_s **headers;
    char *body;
    int *ref_counter;
} http_frame_s;


extern http_header_s *parse_single_header(char const *line);
extern http_frame_s *parse_http_frame(char const *text);
extern http_frame_s *copy_http_frame(http_frame_s *in);
void free_http_frame(http_frame_s *in);