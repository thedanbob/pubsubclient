SRC_PATH=./src
BLD_PATH=./build
BLD_DIRS=$(patsubst ../src/%/., build/%, $(wildcard ../src/*/.))
OUT_PATH=./bin

TEST_SRC=$(wildcard ${SRC_PATH}/*_spec.cpp)
TEST_BIN= $(TEST_SRC:${SRC_PATH}/%.cpp=${OUT_PATH}/%)

VPATH=${SRC_PATH}
SHIM_SRC=$(wildcard ${SRC_PATH}/lib/*.cpp)
SHIM_OBJS=$(patsubst %.cpp, %.o, ${SHIM_SRC})

PSC_SRC=$(wildcard ../src/*.cpp ../src/*/*.cpp)
PSC_OBJS=$(patsubst ../src/%.cpp, build/%.o, ${PSC_SRC})

CXXFLAGS=-I${SRC_PATH}/lib -I../include -g

all: $(TEST_BIN)

$(SHIM_OBJS): %.o: %.cpp
	${CXX} ${CXXFLAGS} -c $^ -o $@

$(PSC_OBJS): ${BLD_PATH}/%.o: ../src/%.cpp
	mkdir -p ${BLD_DIRS}
	${CXX} ${CXXFLAGS} -c $^ -o $@

${OUT_PATH}/%: ${SRC_PATH}/%.cpp ${SHIM_OBJS} ${PSC_OBJS}
	mkdir -p ${OUT_PATH}
	${CXX} ${CXXFLAGS} $^ -o $@

clean:
	@rm -f ${OUT_PATH}/* ${SHIM_OBJS} ${PSC_OBJS}
