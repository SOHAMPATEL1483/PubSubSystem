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
    std::mutex mutex_;

public:
    void registerSubscriber(SubscriptionResponse &_return, const SubscriptionRequest &request) override
    {
        std::lock_guard<std::mutex> lock(mutex_);
        topic_subscribers[request.topic].insert(request.subscriber_id);
        std::cout << "Subscriber registered successfully for topic: " << request.topic << std::endl;
        _return.success = true;
        _return.message = "Subscriber registered successfully.";
    }

    void publishMessage(PublishResponse &_return, const PublishRequest &request) override
    {
        std::lock_guard<std::mutex> lock(mutex_);
        topic_messages[request.topic].push_back(request.message_data);
        _return.success = true;
    }

    void notifySubscribers(NotifyResponse &_return, const NotifyRequest &request) override
    {
        std::lock_guard<std::mutex> lock(mutex_);
        if (topic_subscribers.find(request.topic) == topic_subscribers.end())
        {
            _return.success = false;
            return;
        }

        for (const auto &subscriber : topic_subscribers[request.topic])
        {
            std::cout << "Notifying subscriber " << subscriber << " about topic: " << request.topic << " with message: " << request.message_data << "\n";
        }

        _return.success = true;
    }

    void syncBrokers(SyncResponse &_return, const SyncRequest &request) override
    {
        std::lock_guard<std::mutex> lock(mutex_);
        for (const auto &topic : request.topics)
        {
            if (topic_messages.find(topic) == topic_messages.end())
            {
                topic_messages[topic] = {};
            }
        }

        for (const auto &log_entry : request.message_logs)
        {
            topic_messages[log_entry.first].push_back(log_entry.second);
        }

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
