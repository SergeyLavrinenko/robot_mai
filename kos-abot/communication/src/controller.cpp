#include <cstring>
#include <exception>
#include <iostream>
#include <memory>

#include <kos_net.h>

/* Files required for transport initialization. */
#include <coresrv/nk/transport-kos.h>
#include <coresrv/sl/sl_api.h>

/* Description of the server interface used by the `client` entity. */
#include <echo/Configuration.idl.h>
#include <echo/NavigationCommand.idl.h>

#include <assert.h>

#include "subscriber.h"


NkKosTransport transport;
struct echo_Configuration_proxy proxy;


subscriber_confg getSubscriberConfig() {

    echo_Configuration_GetConfiguration_req req;
    echo_Configuration_GetConfiguration_res res;     
    char resBuffer[echo_Configuration_GetConfiguration_res_arena_size];
    struct nk_arena resArena = NK_ARENA_INITIALIZER(
                                resBuffer, resBuffer + sizeof(resBuffer));

    subscriber_confg connectionConfig;

    if (echo_Configuration_GetConfiguration(&proxy.base, &req, NULL, &res, &resArena) == rcOk) {
        
        nk_uint32_t msgLen = 0;
        nk_char_t *msg =  nk_arena_get(
                        nk_char_t, &resArena, &res.result.mqttHost, &msgLen);
        if (msg != RTL_NULL) {
            connectionConfig.host = std::string(msg);
        }
        
        msg =  nk_arena_get(
                        nk_char_t, &resArena, &res.result.mqttUser, &msgLen);
        if (msg != RTL_NULL) {
            connectionConfig.mqttUser = std::string(msg);
        }

        msg =  nk_arena_get(
                        nk_char_t, &resArena, &res.result.mqttPassword, &msgLen);
        if (msg != RTL_NULL) {
            connectionConfig.mqttPassword = std::string(msg);
        }

        msg =  nk_arena_get(
                        nk_char_t, &resArena, &res.result.mqttTopic, &msgLen);
        if (msg != RTL_NULL) {
            connectionConfig.subTopic = std::string(msg);
        }

        connectionConfig.port = (int)res.result.mqttPort;

        std::cout << "[CommunicationController] Recieved mqttConfig: " << std::endl 
            << connectionConfig.host << " " << connectionConfig.port << std::endl 
            << connectionConfig.subTopic << "  U:" << connectionConfig.mqttUser << std::endl;
    } else {
        std::cerr << "[CommunicationController]  Cannot get config from ConfigurationServer....." << std::endl;
    }

    nk_req_reset(&req);

    return connectionConfig;
} 


int main(void)
{
    std::cout << "[CommunicationController] Waiting for network" << std::endl;
    if (!wait_for_network())
    {
        std::cerr << "[CommunicationController]  Error: Wait for network failed!"
                  << std::endl;
        return EXIT_FAILURE;
    }

    std::cout << "[CommunicationController] Initing configuration connection" << std::endl;
    Handle handle = ServiceLocatorConnect("configuration_connection");
    assert(handle != INVALID_HANDLE);

    /* Initialize IPC transport for interaction with the server entity. */
    NkKosTransport_Init(&transport, handle, NK_NULL, 0);

    nk_iid_t riid = ServiceLocatorGetRiid(handle, "echo.Configuration.getConfiguration");
    assert(riid != INVALID_RIID);

    echo_Configuration_proxy_init(&proxy, &transport.base, riid);

    auto connectionConfig = getSubscriberConfig(); 

    std::cout << "[CommunicationController] Initing navigation connection" << std::endl;

    NkKosTransport transportNav;
    struct echo_NavigationCommand_proxy proxyNav;
    Handle handleNav = ServiceLocatorConnect("navigation_connection");
    assert(handleNav != INVALID_HANDLE);

    /* Initialize IPC transport for interaction with the server entity. */
    NkKosTransport_Init(&transportNav, handleNav, NK_NULL, 0);

    nk_iid_t riidNav = ServiceLocatorGetRiid(handleNav, "echo.NavigationCommand.setNavigationCommand");
    assert(riidNav != INVALID_RIID);

    echo_NavigationCommand_proxy_init(&proxyNav, &transportNav.base, riid);

    NavigationMessageSender sender(&transportNav, &proxyNav);
    MessageValidator validator;

    mosqpp::lib_init();
    std::cout << "[CommunicationController]  Creating subscriber" << std::endl;
    auto sub = std::make_unique<Subscriber>(connectionConfig, &validator, &sender);
    if (sub)
    {
        sub->loop_forever();
    }
    mosqpp::lib_cleanup();
    
    return EXIT_SUCCESS;
}
