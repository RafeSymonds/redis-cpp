#include "replication/replica_role.hpp"
#include "net/socket.hpp"
#include "resp/encode.hpp"
#include "resp/parser.hpp"

auto ReplicaRole::connect(uint16_t port) const -> void {
    auto connection = Socket::createClient(master.hostname, master.port);

    connection.sendString(resp::array({"PING"}));
    resp::parse(connection);

    connection.sendString(resp::array({"REPLCONF", "listening-port", std::to_string(port)}));
    resp::parse(connection);

    connection.sendString(resp::array({"REPLCONF", "capa", "psync2"}));
    resp::parse(connection);

    connection.sendString(resp::array({"PSYNC", "?", "-1"}));
    resp::parse(connection);

    connection.close();
}
