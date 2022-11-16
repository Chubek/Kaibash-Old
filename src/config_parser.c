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

    for (int i = 0; i < splt.count; i++);


}