#include "uchat_server.h"

static void change_message_database(t_json_data *json) {
    t_database_query *db = mx_database_query_create();

    db->model_fill_table = mx_model_message_fill_table();
    asprintf(&db->set, "%s='%s'",
            db->model_fill_table[2],
            json->message.text);
    asprintf(&db->where, "%s=%d AND %s=%d",
            db->model_fill_table[0],
            json->message.message_id,
            db->model_fill_table[1],
            json->message.client1_id);
    mx_update_database(mx_model_message_database(), \
                        mx_model_message_name_table(), db);
    mx_database_query_clean(&db);
}

static void get_channel_id(t_json_data *json, t_json_data *json_res) {
    t_database_query *db = mx_database_query_create();

    db->model_fill_table = mx_model_message_fill_table();
    db->fill_table = mx_strdup("channel_id");
    asprintf(&db->where, "%s=%d AND %s=%d",
             db->model_fill_table[0],
             json->message.message_id,
             db->model_fill_table[1],
             json->message.client1_id);
    db->list = mx_read_database(mx_model_message_database(), \
                                mx_model_message_name_table(), db);
    json_res->message.channel_id = mx_atoi(db->list->data);
    mx_database_query_clean(&db);
}



void mx_controller_edit_message(t_json_data *json, t_clients *client) {
    t_list *data = NULL;
    t_json_data json_res = {.type = JS_MES_EDIT_IN,
                            .message.channel_id = json->message.channel_id,
                            .message.message_id = json->message.message_id,
                            .message.client1_id = json->message.client1_id
                            };

    json_res.message.text = mx_strdup(json->message.text);
    client = client->first;
    change_message_database(json);
    get_channel_id(json, &json_res);
    data = mx_get_user_id_from_database_channels(json_res.message.channel_id);
    mx_send_message_to_channel(data, client, &json_res, \
                          JS_MES_EDIT_IN, JS_MES_EDIT_OUT);
    mx_strdel(&json_res.message.text);
}
