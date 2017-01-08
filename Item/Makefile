CXX=g++

INCPATH=-I. -I.. -I$(PROTOBUF_DIR)/include -I$(BOOST_ROOT)
CXXFLAGS += $(OPT) -pipe -Wno-unused-local-typedefs -Wno-unused-but-set-variable -Wno-literal-suffix -Wall -std=c++11 -ggdb -fPIC -D_GNU_SOURCE -D__STDC_LIMIT_MACROS $(INCPATH)

LIBRARY=-L$(PROTOBUF_DIR)/lib/libprotobuf.a -L$(BOOST_ROOT)/stage/lib/
LDFLAGS = -lboost_system -lboost_thread

PROTO_SRC=
PROTO_OBJ=$(patsubst %.proto,%.pb.o,$(PROTO_SRC))
PROTO_OPTIONS=--proto_path=. --proto_path=$(PROTOBUF_DIR)/include

BASE_OBJ=Item.o Item_Potion.o Item_Equipment.o

TARGET=$(BASE_OBJ)

all: $(TARGET)

clean:
	@rm -f $(TARGET) *.o *.pb.*
	@ctags -R .

rebuild: clean all

%.pb.cc: %.proto
	$(PROTOBUF_DIR)/bin/protoc $(PROTO_OPTIONS) --cpp_out=. $<

%.pb.o: %.pb.cc
	$(CXX) $(CXXFLAGS) -c $< -o $@

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@
