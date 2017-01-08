CXX=g++

INCPATH=-I. -I.. -I$(PROTOBUF_DIR)/include -I$(BOOST_ROOT)
CXXFLAGS += $(OPT) -pipe -Wno-unused-local-typedefs -Wno-unused-but-set-variable -Wno-literal-suffix -Wall -std=c++11 -ggdb -fPIC -D_GNU_SOURCE -D__STDC_LIMIT_MACROS $(INCPATH)

LIBRARY=$(PROTOBUF_DIR)/lib/libprotobuf.a -L$(BOOST_ROOT)/stage/lib/ 
LDFLAGS = -lboost_system -lboost_thread -lprotobuf

PROTO_SRC=Auth.proto 
PROTO_OBJ=$(patsubst %.proto,%.pb.o,$(PROTO_SRC))
PROTO_OPTIONS=--proto_path=. --proto_path=$(PROTOBUF_DIR)/include
#PROTO_OPTIONS=--proto_path=../ 

BASE_OBJ=Session.o AuthServer.o

BIN=AuthServer

all: $(BIN)

clean:
	@rm -f $(BASE_OBJ) *.o *.pb.* $(BIN)
	@ctags -R .

rebuild: clean all

AuthServer: $(PROTO_OBJ) $(BASE_OBJ) Main.o
	$(CXX) $^ -o $@ $(LIBRARY) $(LDFLAGS)

#%.pb.cc: ../%.proto
%.pb.cc: %.proto
	protoc $(PROTO_OPTIONS) --cpp_out=. $<

%.pb.o: %.pb.cc
	$(CXX) $(CXXFLAGS) -c $< -o $@

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@
