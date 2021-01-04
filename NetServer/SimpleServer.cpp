#include <iostream>
#include "william_net.hpp"


enum class CustomMsgtypes:uint32_t
{
    ServerAccept,
    ServerDeny,
    ServerPing,
    MessageALL,
    ServerMessage,
};

class CustomServer: public william::net::server_interface<CustomMsgtypes>{
    public:
    CustomServer(uint16_t port):william::net::server_interface<CustomMsgtypes>(port){

    }

    protected:
    virtual bool OnClientConnect(std::shared_ptr<william::net::connection<CustomMsgtypes>>client){
        return true;
    }
    // Called when a client appears to have disconnected
    virtual void OnClientDisconnect(std::shared_ptr<william::net::connection<CustomMsgtypes>> client){

    }
    // Called when a message arrives
    virtual void OnMessage(std::shared_ptr<william::net::connection<CustomMsgtypes>>client, william::net::message<CustomMsgtypes>msg){

    }

};


int main(){
    CustomServer server(60000);
    server.start();
    while (1)
    {
        server.Update();
    }
    return 0;
    

}

