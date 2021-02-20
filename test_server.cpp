#include "Network.h"

int main()
{
    // Network::run_server(5555);


    // std::vector<std::string> users = Network::find_active_users(5555);
    // for (auto &user : users)
    // {
    //     std::cout << user;
    // }


    std::cout << Network::send_message("192.168.20.24", 5555, "gomoku") << std::endl;
}