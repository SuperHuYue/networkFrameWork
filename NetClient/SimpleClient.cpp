#include <iostream>
#include <william_net.hpp>

enum class CustomerType : uint32_t{
    FireBullet,
    MovePlayer
};

int main(){
    william::net::message<CustomerType> msg;
    msg.header.id = CustomerType::FireBullet;
    int a= 22;
    bool b = true;
    float c = 3.1415926f;
    struct{
        float x;
        float y;
    }d[5];
    msg<<a<<b<<c<<d;
    a= 99;
    b= false;
    c=99.0f;
    msg>>d>>c>>b>>a;
    std::cout << a<<" "<<b <<" "<< c<<std::endl;
    return 0;
}
