#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <memory>
#include <iostream>

/* Files required for transport initialization. */
#include <coresrv/nk/transport-kos.h>
#include <coresrv/sl/sl_api.h>
#include <rtl/string.h>
#include <rtl/stdio.h>

/* EDL description of the server entity. */
#include <echo/NavigationController.edl.h>
#include <assert.h>
#include "navigation.h"


/* Type of interface implementing object. */
typedef struct INavigationCommandImpl {
    struct echo_NavigationCommand base;     /* Base interface of object */
    NavigationController *navCtr;
} INavigationCommandImpl;


/* Ping method implementation. */
static nk_err_t NavigationCommand_impl(struct echo_NavigationCommand *self,
                          const struct echo_NavigationCommand_SetNavigationCommand_req *req,
                          const struct nk_arena *req_arena,
                          struct echo_NavigationCommand_SetNavigationCommand_res *res,
                          struct nk_arena *res_arena) {
                            
    std::cerr << "[NavigationController] Got msg cmd: " << (int)req->value.command << " dur ms:"
        << req->value.durationMs << "and speed "<< (int) req->value.speed <<std::endl;

    INavigationCommandImpl *cmd = (INavigationCommandImpl *)self;
    if (req->value.command == 4) {
        res->result = cmd->navCtr->stopTask();
        return NK_EOK;
    }
    if (req->value.command == 0) {
        res->result = cmd->navCtr->startManualTask(GO_FORWARD, req->value.durationMs, req->value.speed);
        return NK_EOK;
    }
    if (req->value.command == 1) {
        res->result = cmd->navCtr->startManualTask(GO_BACKWARD, req->value.durationMs, req->value.speed);
        return NK_EOK;
    }   
    if (req->value.command == 2) {
        res->result = cmd->navCtr->startManualTask(GO_LEFT, req->value.durationMs, req->value.speed);
        return NK_EOK;
    }
    if (req->value.command == 3) {
        res->result = cmd->navCtr->startManualTask(GO_RIGHT, req->value.durationMs, req->value.speed);
        return NK_EOK;
    }
    res->result = 0;

    return NK_EOK;
}

#define SET_RECTANGLE(v_) { v_ = ((double)(req->value.v_))/1000.0; }

/* Ping method implementation. */
static nk_err_t NavigationAutoCommand_impl(struct echo_NavigationCommand *self,
                          const struct echo_NavigationCommand_SetAutoNavigationCommand_req *req,
                          const struct nk_arena *req_arena,
                          struct echo_NavigationCommand_SetAutoNavigationCommand_res *res,
                          struct nk_arena *res_arena) {
                            
    std::cerr << "[NavigationController] Got Auto command message...." <<std::endl;

    INavigationCommandImpl *cmd = (INavigationCommandImpl *)self;

    cur_pos curPos; 
    point target; 
    rectangle screnCoords;
    rectangle realCoords;
    
    SET_RECTANGLE(realCoords.bl.x)
    SET_RECTANGLE(realCoords.bl.y)
    SET_RECTANGLE(realCoords.br.x)
    SET_RECTANGLE(realCoords.br.y)
    SET_RECTANGLE(realCoords.tl.x)
    SET_RECTANGLE(realCoords.tl.y)
    SET_RECTANGLE(realCoords.tr.x)
    SET_RECTANGLE(realCoords.tr.y)
    SET_RECTANGLE(target.x)
    SET_RECTANGLE(target.y)
    SET_RECTANGLE(curPos.up.x)
    SET_RECTANGLE(curPos.up.y)
    SET_RECTANGLE(curPos.down.x)
    SET_RECTANGLE(curPos.down.y)

    res->result = cmd->navCtr->startAutoTask( curPos, target, screnCoords, realCoords);

    return NK_EOK;
}

static struct echo_NavigationCommand *CreateINavigationCommandImpl(NavigationController* navPtr) {
    /* Table of implementations of IPing interface methods. */
    static const struct echo_NavigationCommand_ops ops = {
        .SetNavigationCommand = NavigationCommand_impl,
        .SetAutoNavigationCommand = NavigationAutoCommand_impl,
    };

    /* Interface implementing object. */
    static struct INavigationCommandImpl impl = {
        .base = {&ops},
        .navCtr = {navPtr}
    };

    return &impl.base;
}

/* Server entry point. */
int main(void) {
    GpioHandle gpioHandle;
    
    if (GpioInit())
    {
        fprintf(stderr, "GpioInit failed\n");
        assert(false);
    }

    /* initialize and setup gpio0 port */
    if (GpioOpenPort("gpio0", &gpioHandle) || gpioHandle == GPIO_INVALID_HANDLE)
    {
        fprintf(stderr, "GpioOpenPort failed\n");
        assert(false);
    }

    GpioSetMode(gpioHandle, GPIO_PIN_NUM_IN1, GPIO_DIR_OUT);
    GpioSetMode(gpioHandle, GPIO_PIN_NUM_IN2, GPIO_DIR_OUT);
    GpioSetMode(gpioHandle, GPIO_PIN_NUM_IN3, GPIO_DIR_OUT);
    GpioSetMode(gpioHandle, GPIO_PIN_NUM_IN4, GPIO_DIR_OUT);
    GpioSetMode(gpioHandle, GPIO_PIN_NUM_ENA, GPIO_DIR_OUT);
    GpioSetMode(gpioHandle, GPIO_PIN_NUM_ENB, GPIO_DIR_OUT);

    NkKosTransport transport;
    ServiceId iid;

    Handle handle = ServiceLocatorRegister("navigation_connection", NULL, 0, &iid);
    assert(handle != INVALID_HANDLE);

    NkKosTransport_Init(&transport, handle, NK_NULL, 0);

   
    echo_NavigationController_entity_req req;
    char req_buffer[echo_NavigationController_entity_req_arena_size];
    struct nk_arena req_arena = NK_ARENA_INITIALIZER(req_buffer,
                                        req_buffer + sizeof(req_buffer));

    echo_NavigationController_entity_res res;
    char res_buffer[echo_NavigationController_entity_res_arena_size];
    struct nk_arena res_arena = NK_ARENA_INITIALIZER(res_buffer,
                                        res_buffer + sizeof(res_buffer));

    echo_NavigationCommand_component component;
    std::unique_ptr<NavigationController> navController = std::make_unique<NavigationController>(&gpioHandle);
    echo_NavigationCommand_component_init(&component, CreateINavigationCommandImpl(navController.get()));

    echo_NavigationController_entity entity;
    echo_NavigationController_entity_init(&entity, &component);

    fprintf(stderr, "Navigation server started\n");

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
            echo_NavigationController_entity_dispatch(&entity, &req.base_, &req_arena,
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
    if(GpioClosePort(gpioHandle)){
        fprintf(stderr, "GpioClosePort failed\n");
        assert(false);
    }

    return EXIT_SUCCESS;
}
