#include <gtest/gtest.h>
#include "networking/tcp_server.hpp"
#include "test_utils/logging_fixture.hpp"

class TCPServerTest : public LoggingFixture {};

TEST_F(TCPServerTest, ServerCreation) {
    auto serv { networking::TCPServer { *logger_ } };
    serv.listen("lo", 0); // any port
    ASSERT_NE(serv.efd_, -1);
}

TEST_F(TCPServerTest, SingleClient) {
   auto serv { networking::TCPServer { *logger_ } };
   int port { serv.listen("lo") };
   ASSERT_GT(port, 0);

    // Connect to server
    auto client1 { networking::TCPSocket { *logger_ } };
    int new_fd { client1.connect("127.0.0.1", "lo", port, false) };
    ASSERT_EQ(new_fd, client1.fd_);

    // Accept connection on server
    serv.poll();

    // Ensure server is now keeping track of this connection
    ASSERT_EQ(serv.sockets_.size(), 1);

    // Send and receive some data back and forth
    std::string msg { "Hello, world!" };
    client1.send(msg.data(), msg.size() + 1);
    client1.sendAndRecv();

    // Poll several times with a delay between each to give server time to receive data
    for (int i { 0 }; i < 10; i++) {
        serv.poll();
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }

    // This socket should now be tracked as a receive socket
    ASSERT_EQ(serv.receive_sockets_.size(), 1);
    serv.sendAndRecv();

    // Ensure data is received by server
    ASSERT_EQ(serv.receive_sockets_[0]->next_rcv_valid_index_, msg.size() + 1);
    ASSERT_EQ(strcmp(serv.receive_sockets_[0]->recv_buf_.get(), msg.data()), 0);

    // Send from server back to client
    std::string msg2 { "Hello, client!" };
    ASSERT_EQ(serv.sockets_[0]->send(msg2.data(), msg2.size() + 1), msg2.size() + 1);
    ASSERT_EQ(serv.sockets_[0]->sendAndRecv(), false);

    // Poll several times with a delay between each to give client time to receive data
    bool received { false };
    for (int i { 0 }; i < 10; i++) {
        bool ret { client1.sendAndRecv() };
        if (ret) {
            received = true;
            break;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
    serv.poll();

    // Ensure data is received by client
    ASSERT_EQ(received, true);
    ASSERT_EQ(client1.next_rcv_valid_index_, msg2.size() + 1);
    ASSERT_EQ(strcmp(client1.recv_buf_.get(), msg2.data()), 0);
}

