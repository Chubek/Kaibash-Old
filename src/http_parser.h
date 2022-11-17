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

http_frame_s *parse_http_frame(char const *text) {
    fstr_s *fstr = fstr_new_from_buffer(text);
    fstr_list split_til_body = fstr_split(fstr, "\n\r\n\r");

    if (split_til_body.count > 2) {
        fprintf(stderr, "Error: more than 2 sections separated with \n\r\n\r, `%d`", split_til_body.count);
        exit(1);
    }
    
    char *header_section = split_til_body.strings[0]->data;
    char *body_section = split_til_body.strings[1]->data;

    fstr_s *header_fstr = fstr_new_from_buffer(header_section);
    fstr_list header_lines = fstr_split(header_fstr, "\n");

    http_header_s **headers = (http_header_s **)malloc(sizeof(http_header_s) * header_lines.count + sizeof(headers));

    for (int i = 0; i < header_lines.count; i++) {
        char *line = header_lines.strings[i]->data;
        
        headers[i] = parse_single_header(line);
    }

    http_frame_s *frame = (http_frame_s *)malloc(sizeof(http_frame_s) + sizeof(body_section));

    strncat(frame->body, body_section, strlen(body_section));
    memset(frame->headers, headers, sizeof(headers));

    *(frame->ref_counter++);

    return frame;
}