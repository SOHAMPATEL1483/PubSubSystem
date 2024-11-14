#include <thrift/Thrift.h>
#include <thrift/protocol/TBinaryProtocol.h>
#include <thrift/transport/TSocket.h>
#include <thrift/transport/TBufferTransports.h>
#include "gen-cpp/pubsub_types.h"
#include "gen-cpp/BrokerService.h"

#include <iostream>

using namespace apache::thrift;
using namespace apache::thrift::protocol;
using namespace apache::thrift::transport;
using namespace PubSub;

void publishMessage(const std::string &broker_address, const std::string &topic, const std::string &message)
{
    std::shared_ptr<TTransport> socket(new TSocket(broker_address, 9090));
    std::shared_ptr<TTransport> transport(new TBufferedTransport(socket));
    std::shared_ptr<TProtocol> protocol(new TBinaryProtocol(transport));

    BrokerServiceClient client(protocol);
    transport->open();

    PublishRequest request;
    request.topic = topic;
    request.message_data = message;

    PublishResponse response;
    client.publishMessage(response, request);

    if (response.success)
    {
        std::cout << "Message published successfully to topic: " << topic << std::endl;
    }
    else
    {
        std::cerr << "Failed to publish message" << std::endl;
    }

    transport->close();
}

int main(int argc, char **argv)
{
    if (argc != 4)
    {
        std::cerr << "Usage: " << argv[0] << " <broker_address> <topic> <message>" << std::endl;
        return 1;
    }
    std::string broker_address = argv[1];
    std::string topic = argv[2];
    std::string message = argv[3];

    publishMessage(broker_address, topic, message);
    return 0;
}
