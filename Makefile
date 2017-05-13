CXX=g++

INCPATH=-I. -I.. -IItem -INetWork -I$(BEHAVIORPATH) -I$(PROTOBUF_DIR)/include -I$(BOOST_ROOT) -I ../ThirdParty/hiredis -I ../ThirdParty/spdlog/include -I ../ThirdParty/pbjson/src
CXXFLAGS += $(OPT) -pipe -Wno-unused-local-typedefs -Wno-unused-but-set-variable -Wno-literal-suffix -Wall -std=c++11 -ggdb -fPIC -D_GNU_SOURCE -D__STDC_LIMIT_MACROS $(INCPATH)

LIBRARY=$(PROTOBUF_DIR)/lib/libprotobuf.a -L$(BOOST_ROOT)/stage/lib/ ../ThirdParty/hiredis/libhiredis.so
LDFLAGS = -lboost_system -lboost_thread -lboost_filesystem -lboost_date_time

PROTO_SRC=P_Asset.proto P_Protocol.proto P_Server.proto
PROTO_OBJ=$(patsubst %.proto,%.pb.o,$(PROTO_SRC))
PROTO_OPTIONS=--proto_path=. --proto_path=$(PROTOBUF_DIR)/include

BASE_OBJ=WorldSession.o MessageDispatcher.o Protocol.o Player.o World.o Asset.o Room.o Game.o Config.o TaskScheduler.o PlayerMatch.o MXLog.o MessageFormat.o pbjson.o
SUB_OBJ=Item/*.o

BIN=GameServer

all: $(BIN)

clean:
	@rm -f $(BIN)  *.o *.pb.*

rebuild: clean all

GameServer: $(PROTO_OBJ) $(BASE_OBJ) $(SUB_OBJ) Main.o
	$(CXX) $^ -o $@ $(LIBRARY) $(LDFLAGS)

%.pb.cc: %.proto
	protoc $(PROTO_OPTIONS) --cpp_out=. $<

%.pb.o: %.pb.cc
	$(CXX) $(CXXFLAGS) -c $< -o $@

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@
