#include <thrift/Thrift.h>
#include <thrift/protocol/TBinaryProtocol.h>
#include <thrift/transport/TServerSocket.h>
#include <thrift/transport/TBufferTransports.h>
#include <thrift/server/TThreadedServer.h>
#include "gen-cpp/pubsub_types.h"
#include "gen-cpp/BrokerService.h"

#include <iostream>
#include <map>
#include <set>
#include <vector>
#include <mutex>

using namespace apache::thrift;
using namespace apache::thrift::protocol;
using namespace apache::thrift::transport;
using namespace apache::thrift::server;
using namespace PubSub;

class BrokerServiceHandler : public BrokerServiceIf
{
private:
    std::map<std::string, std::set<std::string>> topic_subscribers;
    std::map<std::string, std::vector<std::string>> topic_messages;
    std::map<std::string, std::map<std::string, size_t>> last_read_index; // topic -> (subscriber_id -> last index)
    std::mutex mutex_;

public:
    void registerSubscriber(SubscriptionResponse &_return, const SubscriptionRequest &request) override
    {
        std::lock_guard<std::mutex> lock(mutex_);
        topic_subscribers[request.topic].insert(request.subscriber_id);
        last_read_index[request.topic][request.subscriber_id] = 0;
        std::cout << "Subscriber " << request.subscriber_id << " registered for topic: " << request.topic << std::endl;
        _return.success = true;
        _return.message = "Subscriber registered successfully.";
    }

    void publishMessage(PublishResponse &_return, const PublishRequest &request) override
    {
        std::lock_guard<std::mutex> lock(mutex_);
        topic_messages[request.topic].push_back(request.message_data);
        std::cout << "Message published to topic: " << request.topic << " - " << request.message_data << std::endl;
        _return.success = true;
    }

    void getMessages(GetMessagesResponse &_return, const GetMessagesRequest &request) override
    {
        std::lock_guard<std::mutex> lock(mutex_);
        const std::string &topic = request.topic;

        if (topic_messages.find(topic) == topic_messages.end() || topic_subscribers[topic].find(request.subscriber_id) == topic_subscribers[topic].end())
        {
            _return.success = false;
            _return.message = "No messages for the requested topic or subscriber is not registered.";
            return;
        }

        size_t last_index = last_read_index[topic][request.subscriber_id];
        if (last_index >= topic_messages[topic].size())
        {
            _return.success = true;
            _return.message = "No new messages.";
            return;
        }

        // Collect new messages for the subscriber
        _return.messages.assign(topic_messages[topic].begin() + last_index, topic_messages[topic].end());
        last_read_index[topic][request.subscriber_id] = topic_messages[topic].size();
        _return.success = true;
    }
};

int main()
{
    int port = 9090;
    std::shared_ptr<TServerTransport> serverTransport(new TServerSocket(port));
    std::shared_ptr<TTransportFactory> transportFactory(new TBufferedTransportFactory());
    std::shared_ptr<TProtocolFactory> protocolFactory(new TBinaryProtocolFactory());

    // Create a shared pointer for the handler
    std::shared_ptr<BrokerServiceHandler> handler(new BrokerServiceHandler());

    // Pass the shared pointer to the processor
    std::shared_ptr<TProcessor> processor(new BrokerServiceProcessor(handler));

    TThreadedServer server(processor, serverTransport, transportFactory, protocolFactory);
    std::cout << "Broker server started on port " << port << std::endl;
    server.serve();
    return 0;
}
