CXX = g++

CXXFLAGS = -Wall 


BROKER_EXEC = broker
SUBSCRIBER_EXEC = subscriber
PUBLISHER_EXEC = publisher


BROKER_SRC = broker.cpp
SUBSCRIBER_SRC = subscriber.cpp
PUBLISHER_SRC = publisher.cpp

all: $(BROKER_EXEC) $(SUBSCRIBER_EXEC) $(PUBLISHER_EXEC)

$(BROKER_EXEC): $(BROKER_SRC)
	$(CXX) $(CXXFLAGS) $(BROKER_SRC) -o $(BROKER_EXEC) 

$(SUBSCRIBER_EXEC): $(SUBSCRIBER_SRC)
	$(CXX) $(CXXFLAGS) $(SUBSCRIBER_SRC) -o $(SUBSCRIBER_EXEC) 

$(PUBLISHER_EXEC): $(PUBLISHER_SRC)
	$(CXX) $(CXXFLAGS) $(PUBLISHER_SRC) -o $(PUBLISHER_EXEC) 

clean:
	rm -f $(BROKER_EXEC) $(SUBSCRIBER_EXEC) $(PUBLISHER_EXEC)