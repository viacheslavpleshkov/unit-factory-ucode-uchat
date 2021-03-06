#include "uchat_client.h"

char *mx_json_message_edit_out_request(t_json_data *data) {
    char *s;

    *(int *)s = asprintf(&s, mx_get_json_format_template(JS_MES_EDIT_OUT),
        JS_MES_EDIT_OUT, data->message.message_id, data->message.text,
        data->message.client1_id, data->user_id, data->token) - 4;
    return s;
}
