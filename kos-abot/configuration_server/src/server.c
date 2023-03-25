#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

/* Files required for transport initialization. */
#include <coresrv/nk/transport-kos.h>
#include <coresrv/sl/sl_api.h>
#include <rtl/string.h>
#include <rtl/stdio.h>

/* EDL description of the server entity. */
#include <echo/ConfigurationServer.edl.h>
#include <assert.h>


const char *host = "192.168.1.4";
const u_int16_t port = 1883;
const char *subTopic = "abot/command/alex";
const char *mqttUser = "";
const char *mqttPassword = "";


/* Type of interface implementing object. */
typedef struct IConfigurationImpl {
    struct echo_Configuration base;     /* Base interface of object */
} IConfigurationImpl;


#define FILL_MSG(str, putTo) { \
    message  = nk_arena_alloc(nk_char_t, \
                                       res_arena, \
                                       putTo, \
                                       (strlen(str) + 1)); \
    assert (message != RTL_NULL); \
    rtl_snprintf(message, (strlen(str) + 1), "%s", str); \
}


/* Ping method implementation. */
static nk_err_t Configuration_impl(struct echo_Configuration *self,
                          const struct echo_Configuration_GetConfiguration_req *req,
                          const struct nk_arena *req_arena,
                          struct echo_Configuration_GetConfiguration_res *res,
                          struct nk_arena *res_arena) {
    res->result.mqttPort = port;
    nk_char_t *message = NULL; 
    
    FILL_MSG(host, &res->result.mqttHost)
    FILL_MSG(subTopic, &res->result.mqttTopic)
    FILL_MSG(mqttUser, &res->result.mqttUser)
    FILL_MSG(mqttPassword, &res->result.mqttPassword)

    return NK_EOK;
}


static struct echo_Configuration *CreateIConfigurationImpl() {
    /* Table of implementations of IPing interface methods. */
    static const struct echo_Configuration_ops ops = {
        .GetConfiguration = Configuration_impl
    };

    /* Interface implementing object. */
    static struct IConfigurationImpl impl = {
        .base = {&ops}
    };

    return &impl.base;
}

/* Server entry point. */
int main(void) {
    NkKosTransport transport;
    ServiceId iid;

    /* Get server IPC handle of "server_connection". */
    Handle handle = ServiceLocatorRegister("configuration_connection", NULL, 0, &iid);
    assert(handle != INVALID_HANDLE);

    /* Initialize transport to client. */
    NkKosTransport_Init(&transport, handle, NK_NULL, 0);

   
    echo_ConfigurationServer_entity_req req;
    char req_buffer[echo_ConfigurationServer_entity_req_arena_size];
    struct nk_arena req_arena = NK_ARENA_INITIALIZER(req_buffer,
                                        req_buffer + sizeof(req_buffer));

    /* Prepare response structures: constant part and arena. */
    echo_ConfigurationServer_entity_res res;
    char res_buffer[echo_ConfigurationServer_entity_res_arena_size];
    struct nk_arena res_arena = NK_ARENA_INITIALIZER(res_buffer,
                                        res_buffer + sizeof(res_buffer));

    /**
     * Initialize ping component dispatcher. 3 is the value of the step,
     * which is the number by which the input value is increased.
     */
    echo_Configuration_component component;
    echo_Configuration_component_init(&component, CreateIConfigurationImpl());

    /* Initialize server entity dispatcher. */
    echo_ConfigurationServer_entity entity;
    echo_ConfigurationServer_entity_init(&entity, &component);

    fprintf(stderr, "Configuration server started\n");

    /* Dispatch loop implementation. */
    do
    {
        /* Flush request/response buffers. */
        nk_req_reset(&req);
        nk_arena_reset(&req_arena);
        nk_arena_reset(&res_arena);

        /* Wait for request to server entity. */
        if (nk_transport_recv(&transport.base,
                              &req.base_,
                              &req_arena) != NK_EOK) {
            fprintf(stderr, "nk_transport_recv error\n");
        } else {
            echo_ConfigurationServer_entity_dispatch(&entity, &req.base_, &req_arena,
                                        &res.base_, &res_arena);
        }

        /* Send response. */
        if (nk_transport_reply(&transport.base,
                               &res.base_,
                               &res_arena) != NK_EOK) {
            fprintf(stderr, "nk_transport_reply error\n");
        }
    }
    while (true);

    return EXIT_SUCCESS;
}
