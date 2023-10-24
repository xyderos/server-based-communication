CXX := $(shell which clang)

# just a clean way to distinguish the two deployment environments
DEVELOPMENT_FLAGS := -g -Weverything -gdwarf-4 -fPIC
PRODUCTION_FLAGS := -O3 -fPIC

ifeq "$(DEPLOYMENT)" "production"
    CXXFLAGS:= $(PRODUCTION_FLAGS)
else
    CXXFLAGS:= $(DEVELOPMENT_FLAGS)
endif

# language specific
SOURCE_FILES_SUFFIX := .c
HEADER_SUFFIX := .h
OBJECT_SUFFIX := .o

# source
NAME_OF_THE_LIBRARY := libmod.so
NAME_OF_SOURCE_CODE_FOR_THE_LIBRARY := src
RELATIVE_SOURCE_FOLDER_PATH := ./$(NAME_OF_SOURCE_CODE_FOR_THE_LIBRARY)/
NAME_OF_THE_OBJECTS_OF_THE_LIBRARY := temp_obj_directory
OBJECT_FILES_OF_SOURCE_WITHOUT_PREFIX := $(patsubst %$(SOURCE_FILES_SUFFIX),%$(OBJECT_SUFFIX),$(patsubst %$(HEADER_SUFFIX),%$(OBJECT_SUFFIX),$(shell find $(RELATIVE_SOURCE_FOLDER_PATH) -type f -name "*$(HEADER_SUFFIX)")))
OBJECT_FILES_OF_SOURCE_WITH_PREFIX := $(foreach F,$(OBJECT_FILES_OF_SOURCE_WITHOUT_PREFIX),$(lastword $(subst $(RELATIVE_SOURCE_FOLDER_PATH), ,$F)))
LIBRARY_OBJECTS := $(addprefix $(NAME_OF_THE_OBJECTS_OF_THE_LIBRARY)/, $(OBJECT_FILES_OF_SOURCE_WITH_PREFIX))
SHARED := -shared -luring

# server
NAME_OF_SERVER := threaded_socket_server
NAME_OF_SOURCE_CODE_FOR_SERVER:= threaded_sockets/server
RELATIVE_SERVER_FOLDER_PATH := ./$(NAME_OF_SOURCE_CODE_FOR_SERVER)/
NAME_OF_THE_OBJECTS_OF_THE_SERVER := temp_server_obj_directory
OBJECT_FILES_OF_SERVER_WITHOUT_PREFIX := $(patsubst %$(SOURCE_FILES_SUFFIX),%$(OBJECT_SUFFIX),$(patsubst %$(HEADER_SUFFIX),%$(OBJECT_SUFFIX),$(shell find $(RELATIVE_SERVER_FOLDER_PATH) -type f -name "*$(HEADER_SUFFIX)")))
OBJECT_FILES_OF_SERVER_WITH_PREFIX := $(foreach F,$(OBJECT_FILES_OF_SERVER_WITHOUT_PREFIX),$(lastword $(subst $(RELATIVE_SERVER_FOLDER_PATH), ,$F)))
SERVER_OBJECTS :=$(addprefix $(NAME_OF_THE_OBJECTS_OF_THE_SERVER)/, $(OBJECT_FILES_OF_SERVER_WITH_PREFIX))
LIBS := -lsubunit -lm -lcheck -luring $(NAME_OF_THE_LIBRARY)

# client
NAME_OF_CLIENT := threaded_socket_client
NAME_OF_SOURCE_CODE_FOR_CLIENT:= threaded_sockets/client
RELATIVE_CLIENT_FOLDER_PATH := ./$(NAME_OF_SOURCE_CODE_FOR_CLIENT)/
NAME_OF_THE_OBJECTS_OF_THE_CLIENT := temp_client_obj_directory
OBJECT_FILES_OF_CLIENT_WITHOUT_PREFIX := $(patsubst %$(SOURCE_FILES_SUFFIX),%$(OBJECT_SUFFIX),$(patsubst %$(HEADER_SUFFIX),%$(OBJECT_SUFFIX),$(shell find $(RELATIVE_CLIENT_FOLDER_PATH) -type f -name "*$(HEADER_SUFFIX)")))
OBJECT_FILES_OF_CLIENT_WITH_PREFIX := $(foreach F,$(OBJECT_FILES_OF_CLIENT_WITHOUT_PREFIX),$(lastword $(subst $(RELATIVE_CLIENT_FOLDER_PATH), ,$F)))
CLIENT_OBJECTS :=$(addprefix $(NAME_OF_THE_OBJECTS_OF_THE_CLIENT)/, $(OBJECT_FILES_OF_CLIENT_WITH_PREFIX))
LIBS := -lsubunit -lm -lcheck $(NAME_OF_THE_LIBRARY)

script: ./cgi/cgi.c
	$(CXX) ./cgi/cgi.c -o $@

MEM_CHECK_FILE := valgrind_results.txt

# build the library
build: clean $(NAME_OF_THE_LIBRARY)

# compile everything needed for the library
$(NAME_OF_THE_LIBRARY): lib_obj_dirs $(LIBRARY_OBJECTS)
	$(CXX) $(CXXFLAGS) $(SHARED) $(LIBRARY_OBJECTS) -o $(NAME_OF_THE_LIBRARY)

# compile command for each source file
$(NAME_OF_THE_OBJECTS_OF_THE_LIBRARY)/%$(OBJECT_SUFFIX): $(NAME_OF_SOURCE_CODE_FOR_THE_LIBRARY)/%$(SOURCE_FILES_SUFFIX) $(NAME_OF_SOURCE_CODE_FOR_THE_LIBRARY)/%$(HEADER_SUFFIX)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# make temporary directory for the library objects
lib_obj_dirs:
	cp -R --attributes-only ./$(NAME_OF_SOURCE_CODE_FOR_THE_LIBRARY)/ ./$(NAME_OF_THE_OBJECTS_OF_THE_LIBRARY)
	find ./$(NAME_OF_THE_OBJECTS_OF_THE_LIBRARY) -type f -exec rm {} \;
	
# build the server
server: clean $(NAME_OF_THE_LIBRARY) $(NAME_OF_SERVER)

# build an executable that runs the server
$(NAME_OF_SERVER): server_dirs $(SERVER_OBJECTS)
	$(CXX) $(CXXFLAGS) $(SERVER_OBJECTS) -o $(NAME_OF_SERVER) -L. $(LIBS) 

# compile command for each server file
$(NAME_OF_THE_OBJECTS_OF_THE_SERVER)/%$(OBJECT_SUFFIX): $(NAME_OF_SOURCE_CODE_FOR_SERVER)/%$(SOURCE_FILES_SUFFIX) $(NAME_OF_SOURCE_CODE_FOR_SERVER)/%$(HEADER_SUFFIX)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# make temporary directory for the server objects
server_dirs:
	cp -R --attributes-only ./$(NAME_OF_SOURCE_CODE_FOR_SERVER)/ ./$(NAME_OF_THE_OBJECTS_OF_THE_SERVER)
	find ./$(NAME_OF_THE_OBJECTS_OF_THE_SERVER) -type f -exec rm {} \;

# build the client
client: clean $(NAME_OF_THE_LIBRARY) $(NAME_OF_CLIENT)

# build an executable that runs the server
$(NAME_OF_CLIENT): client_dirs $(CLIENT_OBJECTS)
	$(CXX) $(CXXFLAGS) $(CLIENT_OBJECTS) -o $(NAME_OF_CLIENT) -L. $(LIBS) 

# compile command for each server file
$(NAME_OF_THE_OBJECTS_OF_THE_CLIENT)/%$(OBJECT_SUFFIX): $(NAME_OF_SOURCE_CODE_FOR_CLIENT)/%$(SOURCE_FILES_SUFFIX) $(NAME_OF_SOURCE_CODE_FOR_CLIENT)/%$(HEADER_SUFFIX)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# make temporary directory for the server objects
client_dirs:
	cp -R --attributes-only ./$(NAME_OF_SOURCE_CODE_FOR_CLIENT)/ ./$(NAME_OF_THE_OBJECTS_OF_THE_CLIENT)
	find ./$(NAME_OF_THE_OBJECTS_OF_THE_CLIENT) -type f -exec rm {} \;

both: server client

clean:
	rm -f *~ *$(OBJECT_SUFFIX) $(NAME_OF_THE_LIBRARY) $(NAME_OF_SERVER) $(NAME_OF_CLIENT) $(MEM_CHECK_FILE)
	rm -rf $(NAME_OF_THE_OBJECTS_OF_THE_LIBRARY) $(NAME_OF_THE_OBJECTS_OF_THE_SERVER) $(NAME_OF_THE_OBJECTS_OF_THE_CLIENT)

memory_check: test
	valgrind -s --leak-check=full --show-leak-kinds=all --leak-resolution=med --track-origins=yes --verbose --log-file=$(MEM_CHECK_FILE) ./$(NAME_OF_TEST_EXECUTABLE)