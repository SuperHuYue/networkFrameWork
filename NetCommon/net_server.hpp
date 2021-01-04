#pragma once

#include "net_common.hpp"
#include "net_tsqueue.hpp"
#include "net_message.hpp"
#include "net_connection.hpp"

namespace william{
    namespace net{
        template<typename T>
        class server_interface{
        public:
            server_interface(uint16_t port)
            :m_asioAcceptor(m_asioContext, asio::ip::tcp::endpoint(asio::ip::tcp::v4(), port))
            {

            }
            virtual ~server_interface(){
                stop();
            }
            bool start(){
                try{
                    WaitForClientConnection();
                    m_threadContext = std::thread([this](){
                        m_asioContext.run();
                    });
                }
                catch(std::exception& e){
                    //something prohibited the server from listening
                    std::cerr<< "[SERVER] Exception: "<<e.what()<<"\n";
                    return false;
                }
                std::cout << "[SERVER] Started! \n";
                return true;

            }
            void stop(){
                // Request the context to stop
                m_asioContext.stop();
                //Tidy up the context thread
                if(m_threadContext.joinable())m_threadContext.join();
                // Inform someone,anybody if they care...
                std::cout <<"[SERVER] Stopped!\n";
            }
            //ASYNC - Instruct asio to wait for connection
            void WaitForClientConnection(){
                m_asioAcceptor.async_accept(
                    [this](std::error_code ec, asio::ip::tcp::socket socket){
                        if(!ec){
                            std::cout <<"[SERVER] New Connection" << socket.remote_endpoint() <<"\n";
                            std::shared_ptr<connection<T>> newconn = 
                                std::make_shared<connection<T>>(connection<T>::Owner::server,
                                m_asioContext,std::move(socket),m_qMessageIn);
                            if(OnClientConnect(newconn)){
                                //Connection allowed, so add to container of new connections
                                m_deqConnections.push_back(std::move(newconn));
                                m_deqConnections.back()->ConnectToClient(nIDCounter++);
                                std::cout << "[" << m_deqConnections.back()->GetID()<<"] Connection Approved\n";
                            }
                            else{
                                std::cout << "[-----] Connection Denied\n";
                            }

                        }
                    }
                );
            }
            // Send a message to a specific client
            void MessageClient(std::shared_ptr<connection<T>> client, const message<T>&msg){
                if(client && client->IsConnected()){
                    client->Send(msg);
                }
                else{
                    OnClientDisConnect(client);
                    client.reset();
                    m_deqConnections.erase(
                        std::remove(m_deqConnections.begin(),m_deqConnections.end(), client), m_deqConnections.end());
                }
            }
            // Send message to all clients
            void MessageAllClients(const message<T>&msg, std::shared_ptr<connection<T>>pIgnord=nullptr){
                bool bInvalidClientExists = false;
                for(auto& client: m_deqConnections){
                    //check client is connected...
                    if(client && client->IsConnected()){
                        //.. it is !
                        if(client != pIgnord)
                            client->Send(msg);
                    }
                    else{
                        OnClientDisConnect(client);
                        client.reset();
                        bInvalidClientExists = true;
                        // m_deqConnections.erase(
                        //     std::remove(m_deqConnections.begin(),m_deqConnections.end(),client),m_deqConnections.end()
                        // );
                    }
                }
                if(bInvalidClientExists)
                    m_deqConnections.erase(
                        std::remove(m_deqConnections.begin(), m_deqConnections.end(),nullptr),m_deqConnections.end()
                    );

            }
            void Update(size_t nMaxMessages =  -1, bool bwait =false){
                if(bwait)m_qMessageIn.wait();
                size_t nMessageCount = 0;
                while(nMessageCount < nMaxMessages && !m_qMessageIn.empty()){
                    // Grab the front message
                    auto msg = m_qMessageIn.pop_front();
                    //pass to message handler
                    OnMessage(msg.remote, msg.msg);
                    nMessageCount++;
                }
            }
        protected:
            //Called when a client connects, you can vote the connection by return false
            virtual bool OnClientConnect(std::shared_ptr<connection<T>> client){
                return false;
            }
            
            //Called when a client appears to have disconnected
            virtual void OnClientDisConnect(std::shared_ptr<connection<T>> client){

            }
            // Called when a message arrives
            virtual void OnMessage(std::shared_ptr<connection<T>>client, message<T>& msg){

            }
        protected:
            // Thread Safe Queue for incoming message packets
            tsqueue<owned_message<T>>m_qMessageIn;

            //Container of active validated connections
            std::deque<std::shared_ptr<connection<T>>> m_deqConnections;
            
            // Order of declaration is important - it is also the order of initialisation
            asio::io_context m_asioContext;
            std::thread m_threadContext;
            // These things need an asio context
            asio::ip::tcp::acceptor m_asioAcceptor;

            //Clients will be identified in the "wider system"via an id
            uint32_t nIDCounter = 10000;

        };
    }
}
 