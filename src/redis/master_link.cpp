#include <iostream>
#include <print>
#include <stdexcept>
#include <string>

#include "redis/master_link.hpp"
#include "redis/redis.hpp"
#include "resp/encode.hpp"
#include "resp/parser.hpp"
#include "utils/string_utils.hpp"

auto MasterLink::handshake() -> void {
    connection.sendString(resp::array({"PING"}));
    resp::parse(connection);

    connection.sendString(resp::array({"REPLCONF", "listening-port", std::to_string(listeningPort)}));
    resp::parse(connection);

    connection.sendString(resp::array({"REPLCONF", "capa", "psync2"}));
    resp::parse(connection);

    connection.sendString(resp::array({"PSYNC", "?", "-1"}));
    resp::parse(connection); // +FULLRESYNC <replid> <offset>
}

// The RDB arrives as $<length>\r\n<bytes> with no trailing CRLF, so resp::parse
// (which consumes a CRLF after bulk strings) would eat the next command's first bytes.
auto MasterLink::receiveRDB() -> std::string {
    if (connection.recvByte() != '$') {
        throw std::runtime_error("expected RDB transfer");
    }

    auto lengthLine = connection.recvUntil(resp::endsWithCRLF);
    auto length = std::stoull(lengthLine);

    std::string rdb(length, '\0');
    if (connection.recvBytes(rdb.data(), length) != static_cast<ssize_t>(length)) {
        throw std::runtime_error("RDB transfer cut short");
    }

    return rdb;
}

auto MasterLink::run() -> void {
    try {
        handshake();
        receiveRDB();

        while (true) {
            auto args = resp::parse(connection);
            auto reply = redis.execute(args, Origin::MASTER);

            // The only command a replica answers on the replication connection.
            bool isGetAck = args.size() >= 2 && toUpper(args[0]) == "REPLCONF" && toUpper(args[1]) == "GETACK";

            if (isGetAck && reply.has_value()) {
                connection.sendString(*reply);
            }
        }
    } catch (const std::exception &e) {
        std::println(std::cerr, "Master link closed: {}", e.what());
        std::cerr.flush();
    }
}
