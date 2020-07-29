#include "uchat_server.h"

static void fill_database_message(t_json_data *json) {
    t_database_query *db = mx_database_query_create();

    db->model_fill_table = mx_model_message_fill_table();
    asprintf(&db->fill_table, "%s, %s, %s, %s",
             db->model_fill_table[1],
             db->model_fill_table[2],
             db->model_fill_table[3],
             db->model_fill_table[4]);
    asprintf(&db->value, "\"%d\", \"%s\", %s, \"%d\"",
             json->message.client1_id,
             json->message.text,
             "datetime('now')",
             json->message.channel_id);
    mx_create_databases(mx_model_message_database(), \
                        mx_model_message_name_table(), db);
    mx_database_query_clean(&db);
}

static void get_message_id_from_database(t_json_data *json) {
    t_database_query *db = mx_database_query_create();

    db->model_fill_table = mx_model_message_fill_table();
    asprintf(&db->fill_table, "%s, %s",
             db->model_fill_table[0],
             db->model_fill_table[3]);
    asprintf(&db->where, "%s=%d AND %s='%s' AND %s=%d",
             db->model_fill_table[1], json->message.client1_id,
             db->model_fill_table[2], json->message.text,
             db->model_fill_table[4], json->message.channel_id);
    db->list = mx_read_database(mx_model_message_database(), \
               mx_model_message_name_table(), db);
    json->message.message_id = mx_atoi(db->list->data);
    json->message.delivery_time = (long)db->list->next->data;
    mx_database_query_clean(&db);
}

t_list *mx_get_user_id_from_database_channels(int channel_id) {
    t_database_query *db = mx_database_query_create();
    t_list *list = NULL;

    db->model_fill_table = mx_model_user_channel_fill_table();
    asprintf(&db->fill_table ,"%s", db->model_fill_table[1]);
    asprintf(&db->where ,"%s=%d",
             db->model_fill_table[2],
             channel_id);
    list = mx_read_database(mx_model_user_channel_database(), \
                            mx_model_user_channel_name_table(), db);
    mx_database_query_clean(&db);
    return list;
}

void mx_fill_login(t_json_data *json, int user_id, int i, int flag) {
    t_database_query *db = mx_database_query_create();
    t_list *list = NULL;

    db->model_fill_table = mx_model_user_fill_table();
    asprintf(&db->fill_table ,"%s", db->model_fill_table[3]);
    asprintf(&db->where ,"%s=%d",
             db->model_fill_table[0],
             user_id);
    list = mx_read_database(mx_model_user_database(), \
                            mx_model_user_name_table(), db);
    if(flag == 0) {
        strcpy(json->pers_info.login, list->data);
    }
    else if (flag == 1) {
        strcpy(json->messages_arr[i].login, list->data);
    }
    mx_del_list(list, mx_list_size(list));
    mx_database_query_clean(&db);
}


void mx_send_message_to_channel(t_list *data, t_clients *client,
                                t_json_data *json, int type, int type_res) {
    t_list *tmp = data;
    char *json_str = NULL;

    client = client->first;
    while (client != NULL) {
        data = tmp;
        while (data != NULL) {
            if (client->user_id != json->message.client1_id
                && client->user_id == mx_atoi(data->data)) {
                json->type = type;
                json_str = mx_json_make_json(type, json);
                mx_logger("JSON write:",  json_str + 4);
                SSL_write(client->ssl, json_str, mx_strlen(json_str + 4) + 4);
                break;
            }
            else if (client->user_id == json->message.client1_id
                     && client->user_id == mx_atoi(data->data)) {
                json->type = type_res;
                json->status = 200;
                json_str = mx_json_make_json(type_res, json);
                mx_logger("JSON write:",  json_str + 4);
                SSL_write(client->ssl, json_str, mx_strlen(json_str + 4) + 4);
                break;
            }
            data = data->next;
        }
        if(client->next != NULL)
            client = client->next;
        else
            break;
    }
}

void mx_controller_message(t_clients *client, t_json_data *json) {
    t_list *data = NULL;

    if (json->new_channel == true)
        mx_controller_new_channel(json);
    mx_fill_login(json, json->message.client1_id, 0, 0);
    fill_database_message(json);
    data = mx_get_user_id_from_database_channels(json->message.channel_id);
    get_message_id_from_database(json);
    mx_send_message_to_channel(data, client, json, JS_MES_IN, JS_MES_OUT);
    mx_del_list(data, mx_list_size(data));
}
