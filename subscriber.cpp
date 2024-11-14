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

void registerSubscriber(const std::string &broker_address, const std::string &topic, const std::string &subscriber_id)
{
    std::shared_ptr<TTransport> socket(new TSocket(broker_address, 9090));
    std::shared_ptr<TTransport> transport(new TBufferedTransport(socket));
    std::shared_ptr<TProtocol> protocol(new TBinaryProtocol(transport));

    BrokerServiceClient client(protocol);
    transport->open();

    SubscriptionRequest request;
    request.topic = topic;
    request.subscriber_id = subscriber_id;

    SubscriptionResponse response;
    client.registerSubscriber(response, request);

    if (response.success)
    {
        std::cout << "Subscriber registered successfully for topic: " << topic << std::endl;
    }
    else
    {
        std::cerr << "Failed to register subscriber" << std::endl;
    }

    transport->close();
}

int main(int argc, char **argv)
{
    if (argc != 4)
    {
        std::cerr << "Usage: " << argv[0] << " <broker_address> <topic> <subscriber_id>" << std::endl;
        return 1;
    }
    std::string broker_address = argv[1];
    std::string topic = argv[2];
    std::string subscriber_id = argv[3];

    registerSubscriber(broker_address, topic, subscriber_id);
    return 0;
}
