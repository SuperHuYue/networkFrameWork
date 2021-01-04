#pragma once
#include "net_common.hpp"

namespace william{
    namespace net{
        template<typename T>
        class tsqueue
        {
        public:
            tsqueue()=default;
            tsqueue(const tsqueue<T>&)=delete;
            virtual ~tsqueue(){clear();}
        public:
            //Returns and maintains item at front of Queue
            const T& front(){
                std::scoped_lock lock(muxQueue);
                return deQueue.front();
            }
            //Returns and maintains item at back of Queue
            const T& back(){
                std::scoped_lock lock(muxQueue);
                return deQueue.back();
            }
            //Add an item to back of Queue
            void push_back(const T& item){
                std::scoped_lock lock(muxQueue);
                deQueue.emplace_back(std::move(item));
            }
            //Add an item to front of Queue
            void push_front(const T& item){
                std::scoped_lock lock(muxQueue);
                deQueue.emplace_front(std::move(item));
            }

            //Return true if Queue has no item
            bool empty(){
                std::scoped_lock lock(muxQueue);
                return deQueue.empty();
            }
            //Return number of items in Queue
            size_t count(){
                std::scoped_lock lock(muxQueue);
                return deQueue.size();
            }
            //Clears Queue
            void clear(){
                std::scoped_lock lock(muxQueue);
                deQueue.clear();
            }
            //Removes and returns item from front of Queue
            T pop_front(){
                std::scoped_lock lock(muxQueue);
                auto t = std::move(deQueue.front());
                deQueue.pop_front();
                return t;
            }
            //Removes and returns item from back of Queue
            T pop_back(){
                std::scoped_lock lock(muxQueue);
                auto t = std::move(deQueue.back());
                deQueue.pop_back();
                return t;
            }
        protected:
            std::mutex muxQueue;
            std::deque<T>deQueue;


        };
    }
}