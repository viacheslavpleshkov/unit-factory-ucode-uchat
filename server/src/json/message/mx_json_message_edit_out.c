#include "uchat_server.h"

char *mx_json_message_edit_out_response(t_json_data *data) {
    char *s;

    *(int *)s = asprintf(&s, mx_get_json_format_template(JS_MES_EDIT_OUT),
            JS_MES_EDIT_OUT, data->status,
            data->message.message_id) - 4;
    return s;
}
