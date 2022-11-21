#include "../include/kaibash.h"

http_header_s *parse_single_header(char const *line) {
    char c;
    char name[100];
    char value[10000];

    memset(name, 0, 100);
    memset(value, 0, 10000);

    int reached_colon = 0;
    int i = 0;
    int j = 0;

    while (c = *line++) {        
        if (c == ':' && reached_colon == 0) {
            reached_colon = 1;
        }

        switch (reached_colon) {
            case 0:
                name[i++] = c;
            case 1:
                value[j++] = c;
        }          
    }

    http_header_s *header = (http_header_s *)malloc(sizeof(http_header_s));
    memcpy(header->name, name, 100);
    memcpy(header->value, value, 10000);

    return header;
}

http_frame_s *parse_http_frame(char *text) {
    ok_array *split_til_body = ok_array_new(text, "\n\r\n\r");

    if (split_til_body->length > 2) {
        fprintf(stderr, "Error: more than 2 sections separated with \n\r\n\r, `%d`", split_til_body->length);
        exit(1);
    }
    
    char *header_section = split_til_body->elements[0];
    char *body_section = split_til_body->elements[1];

    ok_array *header_lines = ok_array_new(header_section, "\n\r");

    http_header_s **headers = (http_header_s **)malloc(sizeof(http_header_s) * header_lines->length);

    for (int i = 0; i < header_lines->length; i++) {
        char *line = header_lines->elements[i];
        
        headers[i] = parse_single_header(line);
    }

    http_frame_s *frame = (http_frame_s *)malloc(sizeof(http_frame_s) + sizeof(body_section) + sizeof(headers));

    strncat(frame->body, body_section, strlen(body_section));
    memcpy(frame->headers, headers, sizeof(headers));

    *(frame->ref_counter++);

    ok_array_free(split_til_body);
    ok_array_free(header_lines);
    free(headers);
    
    return frame;
}

http_frame_s *copy_http_frame(http_frame_s *in) {
    http_frame_s *new_copy = (http_frame_s *)malloc(sizeof(in));
    *new_copy = *in;

    *(new_copy->ref_counter++);

    return new_copy;
}

void free_http_frame(http_frame_s *in) {
    *(in->ref_counter--);
    
    if (*in->ref_counter == 0) {
        free(in->body);
        free(in->headers);
    }

    free(in);    
}