#include "uchat_client.h"

char *mx_json_message_delete_in_response(t_json_data *data) {
    char *s;

    *(int *)s = asprintf(&s, mx_get_json_format_template(JS_MES_DEL_IN),
                       JS_MES_DEL_IN, data->status, data->message.message_id,
                       data->message.channel_id, data->user_id, data->token) - 4;
    return s;
}
