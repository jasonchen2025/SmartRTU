#include "infra_types.h"
#include "infra_defs.h"
#include "infra_string.h"
#include "infra_list.h"
#include "infra_report.h"
#include "infra_sha256.h"

#include "dev_sign_api.h"
#include "mqtt_api.h"
#include "mqtt_wrapper.h"

#ifdef PLATFORM_HAS_DYNMEM
    #ifdef INFRA_MEM_STATS
        #include "infra_mem_stats.h"
        #define mqtt_api_malloc(size)            LITE_malloc(size, MEM_MAGIC, "mqtt-api")
        #define mqtt_api_free(ptr)               LITE_free(ptr)
    #else
        #define mqtt_api_malloc(size)            HAL_Malloc(size)
        #define mqtt_api_free(ptr)               {HAL_Free((void *)ptr);ptr = NULL;}
    #endif

#else
    static iotx_mqtt_param_t g_iotx_mqtt_param;
#endif

#ifdef INFRA_LOG
    #include "infra_log.h"
    #define mqtt_emerg(...)             log_emerg("MQTT", __VA_ARGS__)
    #define mqtt_crit(...)              log_crit("MQTT", __VA_ARGS__)
    #define mqtt_err(...)               log_err("MQTT", __VA_ARGS__)
    #define mqtt_warning(...)           log_warning("MQTT", __VA_ARGS__)
    #define mqtt_info(...)              log_info("MQTT", __VA_ARGS__)
    #define mqtt_debug(...)             log_debug("MQTT", __VA_ARGS__)
#else
    #define mqtt_emerg(...)             do{printf(__VA_ARGS__);printf("\r\n");}while(0)
    #define mqtt_crit(...)              do{printf(__VA_ARGS__);printf("\r\n");}while(0)
    #define mqtt_err(...)               do{printf(__VA_ARGS__);printf("\r\n");}while(0)
    #define mqtt_warning(...)           do{printf(__VA_ARGS__);printf("\r\n");}while(0)
    #define mqtt_info(...)              do{printf(__VA_ARGS__);printf("\r\n");}while(0)
    #define mqtt_debug(...)             do{printf(__VA_ARGS__);printf("\r\n");}while(0)
#endif

static void        *g_mqtt_client = NULL;
iotx_sign_mqtt_t    g_sign_mqtt;
iotx_sign_mqtt_t    g_default_sign;

/* Handle structure of subscribed topic */
typedef struct  {
#ifdef PLATFORM_HAS_DYNMEM
    char *topic_filter;
#else
    char topic_filter[CONFIG_MQTT_TOPIC_MAXLEN];
#endif
    iotx_mqtt_event_handle_func_fpt handle;
    void *user_data;
    iotx_mqtt_qos_t qos;
#ifdef PLATFORM_HAS_DYNMEM
    struct list_head linked_list;
#else
    int used;
#endif
} iotx_mc_offline_subs_t;

typedef struct {
#ifdef PLATFORM_HAS_DYNMEM
    struct list_head offline_sub_list;
#else
    iotx_mc_offline_subs_t offline_sub_list[CONFIG_MQTT_OFFLINE_TOPIC_MAXNUM];
#endif
    void *mutex;
    int init;
} offline_sub_list_t;

static offline_sub_list_t g_mqtt_offline_subs_list = {0};

static iotx_mqtt_param_t *_iotx_mqtt_new_param(void)
{
#ifdef PLATFORM_HAS_DYNMEM
    return (iotx_mqtt_param_t *)mqtt_api_malloc(sizeof(iotx_mqtt_param_t));
#else
    return &g_iotx_mqtt_param;
#endif
}

static void _iotx_mqtt_free_param(iotx_mqtt_param_t *param)
{
#ifdef PLATFORM_HAS_DYNMEM
    mqtt_api_free(param);
#else
    memset(&g_iotx_mqtt_param, 0, sizeof(iotx_mqtt_param_t));
#endif
}

static int _offline_subs_list_init(void)
{
    if (g_mqtt_offline_subs_list.init) {
        return SUCCESS_RETURN;
    }

    memset(&g_mqtt_offline_subs_list, 0, sizeof(offline_sub_list_t));
    g_mqtt_offline_subs_list.init = 1;

#ifdef PLATFORM_HAS_DYNMEM
    INIT_LIST_HEAD(&g_mqtt_offline_subs_list.offline_sub_list);
#endif

    g_mqtt_offline_subs_list.mutex = HAL_MutexCreate();

    return SUCCESS_RETURN;
}

static int _offline_subs_list_deinit(void)
{
#ifdef PLATFORM_HAS_DYNMEM
    iotx_mc_offline_subs_t *node = NULL, *next_node = NULL;
    list_for_each_entry_safe(node, next_node, &g_mqtt_offline_subs_list.offline_sub_list, linked_list,
                             iotx_mc_offline_subs_t) {
        list_del(&node->linked_list);
        mqtt_api_free(node->topic_filter);
        mqtt_api_free(node);
    }
#endif

    if (g_mqtt_offline_subs_list.mutex) {
        HAL_MutexDestroy(g_mqtt_offline_subs_list.mutex);
    }
    memset(&g_mqtt_offline_subs_list, 0, sizeof(offline_sub_list_t));

    return 0;
}

static int iotx_mqtt_offline_subscribe(const char *topic_filter, iotx_mqtt_qos_t qos,
                                       iotx_mqtt_event_handle_func_fpt topic_handle_func, void *pcontext)
{
    int ret;
#ifdef PLATFORM_HAS_DYNMEM
    iotx_mc_offline_subs_t *sub_info = NULL;
#else
    int idx = 0;
#endif

    if (topic_filter == NULL || topic_handle_func == NULL) {
        return NULL_VALUE_ERROR;
    }

    _offline_subs_list_init();

#ifdef PLATFORM_HAS_DYNMEM
    HAL_MutexLock(g_mqtt_offline_subs_list.mutex);
    list_for_each_entry(sub_info, &g_mqtt_offline_subs_list.offline_sub_list, linked_list, iotx_mc_offline_subs_t) {
        if ((strlen(sub_info->topic_filter) == strlen(topic_filter)) &&
            memcmp(sub_info->topic_filter, topic_filter, strlen(topic_filter)) == 0) {
            sub_info->qos = qos;
            sub_info->handle = topic_handle_func;
            sub_info->user_data = pcontext;
            HAL_MutexUnlock(g_mqtt_offline_subs_list.mutex);
            return SUCCESS_RETURN;
        }
    }

    sub_info = mqtt_api_malloc(sizeof(iotx_mc_offline_subs_t));
    if (sub_info == NULL) {
        HAL_MutexUnlock(g_mqtt_offline_subs_list.mutex);
        return ERROR_MALLOC;
    }

    memset(sub_info, 0, sizeof(iotx_mc_offline_subs_t));
    sub_info->topic_filter = mqtt_api_malloc(strlen(topic_filter) + 1);
    if (sub_info->topic_filter == NULL) {
        mqtt_api_free(sub_info);
        HAL_MutexUnlock(g_mqtt_offline_subs_list.mutex);
        return ERROR_MALLOC;
    }
    memset(sub_info->topic_filter, 0, strlen(topic_filter) + 1);
    strncpy(sub_info->topic_filter, topic_filter, strlen(topic_filter));
    sub_info->qos = qos;
    sub_info->handle = topic_handle_func;
    sub_info->user_data = pcontext;
    INIT_LIST_HEAD(&sub_info->linked_list);

    list_add_tail(&sub_info->linked_list, &g_mqtt_offline_subs_list.offline_sub_list);
    HAL_MutexUnlock(g_mqtt_offline_subs_list.mutex);
    ret = SUCCESS_RETURN;
#else
    if (strlen(topic_filter) >= CONFIG_MQTT_TOPIC_MAXLEN) {
        return MQTT_TOPIC_LEN_TOO_SHORT;
    }

    HAL_MutexLock(g_mqtt_offline_subs_list.mutex);
    for (idx = 0; idx < CONFIG_MQTT_OFFLINE_TOPIC_MAXNUM; idx++) {
        if (g_mqtt_offline_subs_list.offline_sub_list[idx].used &&
            (strlen(g_mqtt_offline_subs_list.offline_sub_list[idx].topic_filter) == strlen(topic_filter)) &&
            memcmp(g_mqtt_offline_subs_list.offline_sub_list[idx].topic_filter, topic_filter, strlen(topic_filter)) == 0) {
            g_mqtt_offline_subs_list.offline_sub_list[idx].qos = qos;
            g_mqtt_offline_subs_list.offline_sub_list[idx].handle = topic_handle_func;
            g_mqtt_offline_subs_list.offline_sub_list[idx].user_data = pcontext;
            HAL_MutexUnlock(g_mqtt_offline_subs_list.mutex);
            return SUCCESS_RETURN;
        }
    }
    for (idx = 0; idx < CONFIG_MQTT_OFFLINE_TOPIC_MAXNUM; idx++) {
        if (g_mqtt_offline_subs_list.offline_sub_list[idx].used == 0) {
            memset(&g_mqtt_offline_subs_list.offline_sub_list[idx], 0, sizeof(iotx_mc_offline_subs_t));
            memcpy(g_mqtt_offline_subs_list.offline_sub_list[idx].topic_filter, topic_filter, strlen(topic_filter));
            g_mqtt_offline_subs_list.offline_sub_list[idx].qos = qos;
            g_mqtt_offline_subs_list.offline_sub_list[idx].handle = topic_handle_func;
            g_mqtt_offline_subs_list.offline_sub_list[idx].user_data = pcontext;
            g_mqtt_offline_subs_list.offline_sub_list[idx].used = 1;
            HAL_MutexUnlock(g_mqtt_offline_subs_list.mutex);
            return SUCCESS_RETURN;
        }
    }
    HAL_MutexUnlock(g_mqtt_offline_subs_list.mutex);
    ret = MQTT_OFFLINE_LIST_LEN_TOO_SHORT;
#endif

    return ret;
}

static int iotx_mqtt_deal_offline_subs(void *client)
{
#ifdef PLATFORM_HAS_DYNMEM
    iotx_mc_offline_subs_t *node = NULL, *next_node = NULL;
#else
    int idx;
#endif
    if (g_mqtt_offline_subs_list.init == 0) {
        return SUCCESS_RETURN;
    }

    HAL_MutexLock(g_mqtt_offline_subs_list.mutex);
#ifdef PLATFORM_HAS_DYNMEM
    list_for_each_entry_safe(node, next_node, &g_mqtt_offline_subs_list.offline_sub_list, linked_list,
                             iotx_mc_offline_subs_t) {
        list_del(&node->linked_list);
        wrapper_mqtt_subscribe(client, node->topic_filter, node->qos, node->handle, node->user_data);
        mqtt_api_free(node->topic_filter);
        mqtt_api_free(node);
    }
#else
    for (idx = 0; idx < CONFIG_MQTT_OFFLINE_TOPIC_MAXNUM; idx++) {
        if (g_mqtt_offline_subs_list.offline_sub_list[idx].used) {
            wrapper_mqtt_subscribe(client, g_mqtt_offline_subs_list.offline_sub_list[idx].topic_filter,
                                   g_mqtt_offline_subs_list.offline_sub_list[idx].qos,
                                   g_mqtt_offline_subs_list.offline_sub_list[idx].handle,
                                   g_mqtt_offline_subs_list.offline_sub_list[idx].user_data);
            g_mqtt_offline_subs_list.offline_sub_list[idx].used = 0;
        }
    }
#endif
    HAL_MutexUnlock(g_mqtt_offline_subs_list.mutex);

    _offline_subs_list_deinit();

    return SUCCESS_RETURN;
}

static void iotx_mqtt_report_funcs(void *pclient)
{
    int                 err;

    iotx_mqtt_deal_offline_subs(pclient);

#ifndef ATHOST_MQTT_REPORT_DISBALED
    iotx_set_report_func(IOT_MQTT_Publish_Simple);
    /* report module id */
    err = iotx_report_mid(pclient);
    if (SUCCESS_RETURN != err) {
#ifdef DEBUG_REPORT_MID_DEVINFO_FIRMWARE
        mqtt_err("failed to report mid");
#endif
    }

    /* report device info */
    err = iotx_report_devinfo(pclient);
    if (SUCCESS_RETURN != err) {
#ifdef DEBUG_REPORT_MID_DEVINFO_FIRMWARE
        mqtt_err("failed to report devinfo");
#endif
    }

    /* report firmware version */
    err = iotx_report_firmware_version(pclient);
    if (SUCCESS_RETURN != err) {
#ifdef DEBUG_REPORT_MID_DEVINFO_FIRMWARE
        mqtt_err("failed to report firmware version");
#endif
    }
#endif
}

/************************  Public Interface ************************/
void *IOT_MQTT_Construct(iotx_mqtt_param_t *pInitParams)
{
    void                   *pclient;
    int                     err;
    int                     ret;
    iotx_mqtt_param_t      *mqtt_params = NULL;

    do {
        iotx_dev_meta_info_t    meta;

        mqtt_params = _iotx_mqtt_new_param();
        if (mqtt_params == NULL) {
            return NULL;
        }

        memset(&meta, 0, sizeof(iotx_dev_meta_info_t));
        HAL_GetProductKey(meta.product_key);
        HAL_GetDeviceName(meta.device_name);
        HAL_GetDeviceSecret(meta.device_secret);

        memset(&g_default_sign, 0, sizeof(iotx_sign_mqtt_t));

        ret = IOT_Sign_MQTT(IOTX_CLOUD_REGION_SHANGHAI, &meta, &g_default_sign);
        if (ret != SUCCESS_RETURN) {
            _iotx_mqtt_free_param(mqtt_params);
            return NULL;
        }

        /* Initialize MQTT parameter */
        memset(mqtt_params, 0x0, sizeof(iotx_mqtt_param_t));

        mqtt_params->port = g_default_sign.port;
        mqtt_params->host = g_default_sign.hostname;
        mqtt_params->client_id = g_default_sign.clientid;
        mqtt_params->username = g_default_sign.username;
        mqtt_params->password = g_default_sign.password;
#ifdef SUPPORT_TLS
        {
            extern const char *iotx_ca_crt;
            mqtt_params->pub_key = iotx_ca_crt;
					  printf("__Initialize MQTT parameter��SUPPORT_TLS\r\n"); //20190420print
        }
#endif
        mqtt_params->request_timeout_ms    = CONFIG_MQTT_REQUEST_TIMEOUT;
        mqtt_params->clean_session         = 0;
        mqtt_params->keepalive_interval_ms = CONFIG_MQTT_KEEPALIVE_INTERVAL * 1000;
        mqtt_params->read_buf_size         = CONFIG_MQTT_MESSAGE_MAXLEN;
        mqtt_params->write_buf_size        = CONFIG_MQTT_MESSAGE_MAXLEN;
        mqtt_params->handle_event.h_fp     = NULL;
        mqtt_params->handle_event.pcontext = NULL;
				//20190420print  
				printf("__IOT_Sign_MQTT(_SHANGHAI, &meta, &g_default_sign),then copy g_default_sign to qtt_params****\r\n"); //20190420print
				printf("mqtt_params->port = %d\r\n",mqtt_params->port);
				printf("mqtt_params->host = %s\r\n",mqtt_params->host);
				printf("mqtt_params->client_id = %s\r\n",mqtt_params->client_id);
				printf("mqtt_params->username = %s\r\n",mqtt_params->username);
				printf("mqtt_params->password = %s\r\n",mqtt_params->password);
				printf("mqtt_params->pub_key for TLS = %s\r\n",mqtt_params->pub_key);
				printf("mqtt_params->request_timeout_ms = %d\r\n",mqtt_params->request_timeout_ms);
				printf("mqtt_params->clean_session= %d\r\n",mqtt_params->clean_session);
				printf("mqtt_params->keepalive_interval_ms= %d\r\n",mqtt_params->keepalive_interval_ms);
				printf("mqtt_params->read_buf_size= %d\r\n",mqtt_params->read_buf_size);
				printf("mqtt_params->write_buf_size= %d\r\n",mqtt_params->write_buf_size);
    
    } while (0);

    if (pInitParams == NULL) {
        if (g_mqtt_client != NULL) {
            mqtt_err("Already exist default MQTT connection, won't proceed another one");
            _iotx_mqtt_free_param(mqtt_params);
            return NULL;
        }

        pInitParams = mqtt_params;
    } else {
        if (g_mqtt_client != NULL) {
            IOT_MQTT_Destroy(&g_mqtt_client);
        }
    }

    if (!pInitParams->client_id) {
        pInitParams->client_id = mqtt_params->client_id;
        mqtt_warning("Using default client_id: %s", pInitParams->client_id);
    }

    if (!pInitParams->host) {
        pInitParams->host = mqtt_params->host;
        mqtt_warning("Using default host: %s", pInitParams->host);
    }

    if (!pInitParams->username) {
        pInitParams->username = mqtt_params->username;
        mqtt_warning("Using default username: %s", pInitParams->username);
    }

    if (!pInitParams->password) {
        pInitParams->password = mqtt_params->password;
        mqtt_warning("Using default password: %s", pInitParams->password);
    }

    if (pInitParams->request_timeout_ms < CONFIG_MQTT_REQ_TIMEOUT_MIN ||
        pInitParams->request_timeout_ms > CONFIG_MQTT_REQ_TIMEOUT_MAX) {

        mqtt_warning("Using default request_timeout_ms: %d, configured value(%d) out of [%d, %d]",
                     mqtt_params->request_timeout_ms,
                     pInitParams->request_timeout_ms,
                     CONFIG_MQTT_REQ_TIMEOUT_MIN,
                     CONFIG_MQTT_REQ_TIMEOUT_MAX);

        pInitParams->request_timeout_ms = mqtt_params->request_timeout_ms;
    }

    if (pInitParams->keepalive_interval_ms < CONFIG_MQTT_KEEPALIVE_INTERVAL_MIN * 1000 ||
        pInitParams->keepalive_interval_ms > CONFIG_MQTT_KEEPALIVE_INTERVAL_MAX * 1000) {

        mqtt_warning("Using default keepalive_interval_ms: %d, configured value(%d) out of [%d, %d]",
                     mqtt_params->keepalive_interval_ms,
                     pInitParams->keepalive_interval_ms,
                     CONFIG_MQTT_KEEPALIVE_INTERVAL_MIN * 1000,
                     CONFIG_MQTT_KEEPALIVE_INTERVAL_MAX * 1000);

        pInitParams->keepalive_interval_ms = mqtt_params->keepalive_interval_ms;
    }

    if (!pInitParams->port) {
        pInitParams->port = mqtt_params->port;
        mqtt_warning("Using default port: %d", pInitParams->port);
    }

    if (!pInitParams->read_buf_size) {
        pInitParams->read_buf_size = mqtt_params->read_buf_size;
        mqtt_warning("Using default read_buf_size: %d", pInitParams->read_buf_size);
    }

    if (!pInitParams->write_buf_size) {
        pInitParams->write_buf_size = mqtt_params->write_buf_size;
        mqtt_warning("Using default write_buf_size: %d", pInitParams->write_buf_size);
    }
		//20190420print  
				printf("__@@@@copy  mqtt_params to pInitParams****\r\n"); //20190420print
				printf("pInitParams->port = %d\r\n",pInitParams->port);
				printf("pInitParams->host = %s\r\n",pInitParams->host);
				printf("pInitParams->client_id = %s\r\n",pInitParams->client_id);
				printf("pInitParams->username = %s\r\n",pInitParams->username);
				printf("pInitParams->password = %s\r\n",pInitParams->password);
				printf("pInitParams->pub_key for TLS = %s\r\n",pInitParams->pub_key);
				printf("pInitParams->request_timeout_ms = %d\r\n",pInitParams->request_timeout_ms);
				printf("pInitParams->clean_session= %d\r\n",pInitParams->clean_session);
				printf("pInitParams->keepalive_interval_ms= %d\r\n",pInitParams->keepalive_interval_ms);
				printf("pInitParams->read_buf_size= %d\r\n",pInitParams->read_buf_size);
				printf("pInitParams->write_buf_size= %d\r\n",pInitParams->write_buf_size);

#if 0
    if (pInitParams->host == NULL || pInitParams->client_id == NULL ||
        pInitParams->username == NULL || pInitParams->password == NULL ||
        pInitParams->port == 0 || !strlen(pInitParams->host)) {
        mqtt_err("init params is not complete");
        if (mqtt_params != NULL) {
            _iotx_mqtt_free_param(mqtt_params);

        }
        return NULL;
    }
#endif
     //20190420print  --** ִ��GPRS���� ͨ�� +SAPBR: 11,"10.180.55.195"
		//<AT+SAPBR=3,1,"Contype","GPRS"��,<AT+SAPBR=3,1,"APN","CMNET",AT+SAPBR=1,1,;AT+SAPBR=2,1��ѯ��+SAPBR: 1,1,"10.10.255.195 
		//<AT+CPIN?�� +CPIN: READY �� <AT+CSQ ��+CSQ: 29,0�� <AT+CREG?��+CREG: 0,1 ע��ɹ��
		//<AT+CGATT ���+CGATT: 1 GPRS attached�ɹ���  <AT+SAPBR=2,1 ,, +SAPBR: 11,"10.180.55.195"
    pclient = wrapper_mqtt_init(pInitParams);/*����Ҫ�Ƕ�sim800����һЩ�ײ��at�������� step1-1*/
    if (pclient == NULL) {
        if (mqtt_params != NULL) {
            _iotx_mqtt_free_param(mqtt_params);
            mqtt_err("wrapper_mqtt_init error");
            return NULL;
        }
    }
    //����GPRS-TCP���� -Start_Gprs_TCP() 20190422   step1-2 
		 printf("__@@@@GPRS-TCP��ʼ����****\r\n"); //20190422print
		if(Start_Gprs_TCP(pInitParams))        //����TCP����	
		{ 
			printf("__@@@@GPRS-TCP����ʧ�ܦ****\r\n"); //20190422print 
		} 
		else 
		  printf("__@@@@GPRS-TCP���ӳɹ�****\r\n"); //20190422print
		
		
		//
    err = wrapper_mqtt_connect(pclient);
    if (SUCCESS_RETURN != err) {
        if (MQTT_CONNECT_BLOCK != err) {
            mqtt_err("wrapper_mqtt_connect failed");
            wrapper_mqtt_release(&pclient);
            return NULL;
        }
    }

#ifndef ASYNC_PROTOCOL_STACK
    iotx_mqtt_report_funcs(pclient);
#endif

    g_mqtt_client = pclient;

    _iotx_mqtt_free_param(mqtt_params);
    return pclient;
}

int IOT_MQTT_Destroy(void **phandler)
{
    void *client;
    if (phandler != NULL) {
        client = *phandler;
        *phandler = NULL;
    } else {
        client = g_mqtt_client;
    }

    if (client == NULL) {
        mqtt_err("handler is null");
        return NULL_VALUE_ERROR;
    }

    wrapper_mqtt_release(&client);
    g_mqtt_client = NULL;

    return SUCCESS_RETURN;
}

int IOT_MQTT_Yield(void *handle, int timeout_ms)
{
    void *pClient = (handle ? handle : g_mqtt_client);
    return wrapper_mqtt_yield(pClient, timeout_ms);
}

/* check whether MQTT connection is established or not */
int IOT_MQTT_CheckStateNormal(void *handle)
{
    void *pClient = (handle ? handle : g_mqtt_client);
    if (pClient == NULL) {
        mqtt_err("handler is null");
        return NULL_VALUE_ERROR;
    }

    return wrapper_mqtt_check_state(pClient);
}

int IOT_MQTT_Subscribe(void *handle,
                       const char *topic_filter,
                       iotx_mqtt_qos_t qos,
                       iotx_mqtt_event_handle_func_fpt topic_handle_func,
                       void *pcontext)
{
    void *client = handle ? handle : g_mqtt_client;

    if (client == NULL) { /* do offline subscribe */
        return iotx_mqtt_offline_subscribe(topic_filter, qos, topic_handle_func, pcontext);
    }

    if (topic_filter == NULL || strlen(topic_filter) == 0 || topic_handle_func == NULL) {
        mqtt_err("params err");
        return NULL_VALUE_ERROR;
    }

#ifdef SUB_PERSISTENCE_ENABLED
    if (qos > IOTX_MQTT_QOS3_SUB_LOCAL) {
        mqtt_warning("Invalid qos(%d) out of [%d, %d], using %d",
                     qos,
                     IOTX_MQTT_QOS0, IOTX_MQTT_QOS3_SUB_LOCAL, IOTX_MQTT_QOS0);
        qos = IOTX_MQTT_QOS0;
    }
#else
    if (qos > IOTX_MQTT_QOS2) {
        mqtt_warning("Invalid qos(%d) out of [%d, %d], using %d",
                     qos,
                     IOTX_MQTT_QOS0, IOTX_MQTT_QOS2, IOTX_MQTT_QOS0);
        qos = IOTX_MQTT_QOS0;
    }
#endif

    return wrapper_mqtt_subscribe(client, topic_filter, qos, topic_handle_func, pcontext);
}

#define SUBSCRIBE_SYNC_TIMEOUT_MAX 10000
int IOT_MQTT_Subscribe_Sync(void *handle,
                            const char *topic_filter,
                            iotx_mqtt_qos_t qos,
                            iotx_mqtt_event_handle_func_fpt topic_handle_func,
                            void *pcontext,
                            int timeout_ms)
{
    void *client = handle ? handle : g_mqtt_client;

    if (client == NULL) { /* do offline subscribe */
        return iotx_mqtt_offline_subscribe(topic_filter, qos, topic_handle_func, pcontext);
    }
    if (timeout_ms > SUBSCRIBE_SYNC_TIMEOUT_MAX) {
        timeout_ms = SUBSCRIBE_SYNC_TIMEOUT_MAX;
    }

    if (topic_filter == NULL || strlen(topic_filter) == 0 || topic_handle_func == NULL) {
        mqtt_err("params err");
        return NULL_VALUE_ERROR;
    }

#ifdef SUB_PERSISTENCE_ENABLED
    if (qos > IOTX_MQTT_QOS3_SUB_LOCAL) {
        mqtt_warning("Invalid qos(%d) out of [%d, %d], using %d",
                     qos,
                     IOTX_MQTT_QOS0, IOTX_MQTT_QOS3_SUB_LOCAL, IOTX_MQTT_QOS0);
        qos = IOTX_MQTT_QOS0;
    }
#else
    if (qos > IOTX_MQTT_QOS2) {
        mqtt_warning("Invalid qos(%d) out of [%d, %d], using %d",
                     qos,
                     IOTX_MQTT_QOS0, IOTX_MQTT_QOS2, IOTX_MQTT_QOS0);
        qos = IOTX_MQTT_QOS0;
    }
#endif

    return wrapper_mqtt_subscribe_sync(client, topic_filter, qos, topic_handle_func, pcontext, timeout_ms);
}

int IOT_MQTT_Unsubscribe(void *handle, const char *topic_filter)
{
    void *client = handle ? handle : g_mqtt_client;


    if (client == NULL || topic_filter == NULL || strlen(topic_filter) == 0) {
        mqtt_err("params err");
        return NULL_VALUE_ERROR;
    }

    return wrapper_mqtt_unsubscribe(client, topic_filter);
}

int IOT_MQTT_Publish(void *handle, const char *topic_name, iotx_mqtt_topic_info_pt topic_msg)
{
    void *client = handle ? handle : g_mqtt_client;
    int                 rc = -1;

    if (client == NULL || topic_name == NULL || strlen(topic_name) == 0) {
        mqtt_err("params err");
        return NULL_VALUE_ERROR;
    }

    rc = wrapper_mqtt_publish(client, topic_name, topic_msg);
    return rc;
}

int IOT_MQTT_Publish_Simple(void *handle, const char *topic_name, int qos, void *data, int len)
{
    iotx_mqtt_topic_info_t mqtt_msg;
    void *client = handle ? handle : g_mqtt_client;
    int rc = -1;

    if (client == NULL || topic_name == NULL || strlen(topic_name) == 0) {
        mqtt_err("params err");
        return NULL_VALUE_ERROR;
    }

    memset(&mqtt_msg, 0x0, sizeof(iotx_mqtt_topic_info_t));

    mqtt_msg.qos         = qos;
    mqtt_msg.retain      = 0;
    mqtt_msg.dup         = 0;
    mqtt_msg.payload     = (void *)data;
    mqtt_msg.payload_len = len;

    rc = wrapper_mqtt_publish(client, topic_name, &mqtt_msg);

    if (rc < 0) {
        mqtt_err("IOT_MQTT_Publish failed\n");
        return -1;
    }

    return rc;
}

int IOT_MQTT_Nwk_Event_Handler(void *handle, iotx_mqtt_nwk_event_t event, iotx_mqtt_nwk_param_t *param)
{
#ifdef ASYNC_PROTOCOL_STACK
    void *client = handle ? handle : g_mqtt_client;
    int rc = -1;

    if (client == NULL || event >= IOTX_MQTT_SOC_MAX || param == NULL) {
        mqtt_err("params err");
        return NULL_VALUE_ERROR;
    }

    rc = wrapper_mqtt_nwk_event_handler(client, event, param);

    if (rc < 0) {
        mqtt_err("IOT_MQTT_Nwk_Event_Handler failed\n");
        return -1;
    }

    switch (event) {
        case IOTX_MQTT_SOC_CONNECTED: {
            iotx_mqtt_report_funcs(client);
        }
        break;
        default: {
        }
        break;
    }

    return rc;
#else
    return -1;
#endif
}
