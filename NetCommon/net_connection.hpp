
#pragma once
#include "net_common.hpp"
#include "net_tsqueue.hpp"
#include "net_message.hpp"

namespace william{
    namespace net{
        template<typename T>
        class connection: public std::enable_shared_from_this<connection<T>>{
            public:
                enum class Owner{
                    server,
                    client
                };
                connection(Owner parent, asio::io_context& asioContext, asio::ip::tcp::socket socket, tsqueue<owned_message<T>>& qInMsg)
                    :m_asioContext(asioContext),m_socket(std::move(socket)),m_qMessageIn(qInMsg)
                {
                    m_nOwnertype = parent;
                }
                virtual ~connection(){
                }
            public:
                void ConnectToClient(uint32_t uid = 0){
                    if(m_nOwnertype == Owner::server){
                        if(m_socket.is_open()){
                            id = uid;
                            ReadHeader();
                        }
                    }
                }
                bool ConnectToServer();
                bool Disconnect(){
                    if(IsConnected())
                     asio::post(m_asioContext, [this](){m_socket.close();});
                }
                bool IsConnected()const{
                    return m_socket.is_open();
                };
                uint32_t GetID()const
                {
                    return id;
                };
            public:
                bool Send(const message<T>&msg){
                    asio::post(m_asioContext,
                        [this,msg](){
                            bool bWritingMessage = !m_qMessageOut.empty();
                            m_qMessageOut.push_back(msg);
                            if(!bWritingMessage){
                                WriteHeader();
                            }
                        });
                        
                }
            private:
                //ASYNC - Prime context ready to read a message header
                void ReadHeader(){
                    asio::async_read(m_socket, asio::buffer(&m_msgTemporaryIn.header, sizeof(message_header<T>)),
                    [this](std::error_code ec, std::size_t length){
                        if(!ec){
                            if(m_msgTemporaryIn.header.size > 0){
                                m_msgTemporaryIn.body.resize(m_msgTemporaryIn.header.size);
                                ReadBody();
                            }
                            else{
                                AddToInComingMessageQueue();
                            }
                        }
                        else{
                            std::cout << "[" << id << "] Read Header Fail.\n";
                            m_socket.close();
                        }
                    });
                }
                //ASYNC - Prime context ready to read a message body
                void ReadBody(){
                    asio::async_read(m_socket, asio::buffer(m_msgTemporaryIn.body.data(), m_msgTemporaryIn.body.size()),
                    [this](std::error_code ec, std::size_t length){
                        if(!ec){
                            AddToInComingMessageQueue();
                        }
                        else{
                            std::cout << "[" << id << "] Read Body Fail.\n";
                            m_socket.close();
                        }
                    });

                }
                //ASYNC - Prime context ready to write a message header
                void WriteHeader(){
                    asio::async_write(m_socket, asio::buffer(&m_qMessageOut.front().header, sizeof(message_header<T>)),
                    [this](std::error_code ec, std::size_t length){
                        if(!ec){
                            if(m_qMessageOut.front().body.size()>0){
                                WriteBody();
                            }
                            else{
                                m_qMessageOut.pop_front();
                                if(!m_qMessageOut.empty())WriteHeader();
                            }
                        }
                    });
                }

                //ASYNC - Prime context ready to write a message body
                void WriteBody(){
                    asio::async_write(m_socket, asio::buffer(&m_qMessageOut.front().body.data(), m_qMessageOut.front().body.size()),
                    [this](std::error_code ec, std::size_t length){
                        if(!ec){
                            m_qMessageOut.pop_front();
                            if(!m_qMessageOut.empty()){
                                WriteHeader();
                            }
                            else{
                                std::cout <<"["<< id<<"] Write Body Fail.\n";
                                m_socket.close();
                            }
                        }
                    });
                }
                void AddToInComingMessageQueue(){
                    if(m_nOwnertype == Owner::server){
                        m_qMessageIn.push_back({this->shared_from_this(),m_msgTemporaryIn});
                    }else{
                        m_qMessageIn.push_back({nullptr, m_msgTemporaryIn});
                    }
                    ReadHeader();
                };


            protected:
                //Each connection has a unique socket to a remote 
                asio::ip::tcp::socket m_socket;
                //This context is shared with the whole asio instance
                asio::io_context& m_asioContext;
                //This queue holds all messages to be sent to the remote side
                //of this connection
                tsqueue<message<T>> m_qMessageOut;
                //This queue holds all message that have been recieved from
                //the remote side of this connection. Note it is a reference
                // as the "owner" of this connection is expected to provide a queue
                tsqueue<owned_message<T>>& m_qMessageIn;
                message<T> m_msgTemporaryIn;
                //The "owner" decides how some of the connection behaves
                Owner m_nOwnertype = Owner::server;
                uint32_t id = 0;
        };
    }
}