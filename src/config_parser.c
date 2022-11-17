#include "../include/kaibash.h"

void assert_config_label(
        config_labels *label,
        char *current_line, 
        char *current_content
    ) {
        memset(current_content, 0, 10000);

        char c;
        char label[100];
        int i = 0;

        do {
            label[i++] = c = *current_line++;
        } while(!IS_COLON(c) && IS_CAPITAL(c));

        if (STRING_EQ(CERT_HASH, label)) {
            *label = ConfCertHash;
        } else if (STRING_EQ(CERT_FILE, label)) {
            *label = ConfCertFile;
        } else if (STRING_EQ(KEY_FILE, label)) {
            *label = ConfKeyFile;
        } else if (STRING_EQ(PASSWORD, label)) {
            *label = ConfPassword;
        }

        if (IS_SPACE(current_line[0])) {
            current_line++;
        }

        i = 0;
        do {
            current_content[i++] = c = *current_line++;
        } while(!IS_SEMICOLON(c));
}

configp_wrapper_s *parse_config(char const *filepath) {
    config_labels label;
    fstr_s *fs = fstr_new(filepath);
    fstr_list splt = fstr_split(fs, "\\n");
    configp_wrapper_s *configp = malloc(sizeof(configp_wrapper_s));
    char current_content[5000];

    for (int i = 0; i < splt.count; i++) {
        fstr_s *current_line_s = splt.strings[i];
        char *current_line = current_line_s->data;

        assert_config_label(
            &label,
            current_line,
            current_content
        );

        switch (label) {
            case ConfCertHash:
                memcpy(
                    configp->configp.cert_hash,
                    current_content,
                    5000
                );
                break;
            case ConfCertFile:
                char *file_contents = string_from_file(current_content);

                memcpy(
                    configp->configp.cert_file,
                    file_contents,
                    50000
                );
                free(file_contents);
                break;
            case ConfKeyFile:
                char *file_contents = string_from_file(current_content);

                memcpy(
                    configp->configp.key_file,
                    file_contents,
                    50000
                );
                free(file_contents);
                break;
            case ConfPassword:
                memcpy(
                    configp->configp.password,
                    current_content,
                    5000
                );
                break;
        }
    }

    *(configp->ref_counter)++;

    return configp;
}

configp_wrapper_s *copy_config(configp_wrapper_s const *in) {
    configp_wrapper_s *configp_new = malloc(sizeof(configp_wrapper_s));
    *configp_new = *in;
    *(configp_new->ref_counter)++;

    return configp_new;
}

void free_config(configp_wrapper_s *in) {
    *(in->ref_counter)--;

    free(in);
}