CXXFLAGS =	-O3 -Wall -fmessage-length=0

OBJS =		simple_am.o SwimClient.o

LIBS = -lboost_system -lpthread

TARGET =	simple_am

TEST = $(TARGET)_test
TEST_OBJS = SwimClient.o SwimClientTest.o

$(TARGET):	$(OBJS)
	$(CXX) -o $(TARGET) $(OBJS) $(LIBS)

all:	$(TARGET)

test: 	$(TEST)
	./$(TEST)

$(TEST):	$(TEST_OBJS)
	$(CXX) -o $(TEST) $(TEST_OBJS) $(LIBS)

clean:
	rm -f $(OBJS) $(TEST_OBJS) $(TARGET) $(TEST)
