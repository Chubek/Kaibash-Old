#include <stdlib.h>
#include <string.h>
#include <msquic.h>
#include <glib.h>
#include <unistd.h>
#include <ctype.h>
#include <unistd.h>
#include <sys/types.h>
#include <pwd.h>

#define _GNU_SOURCE //asks stdio.h to include asprintf
#include <stdio.h>

#include "kquic.h"
#include "fstr.h"
#include "string_utilities.h"
#include "utils.h"
#include "config_parser.h"