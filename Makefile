#
# The Falcon Programming Language
#
# fx-chat - Falcon script Xchat plugin
#
#   GNU Makefile for Linux, BSD and generic unices
#
#

#Define here the list of files to be compiled

OBJECTS= build/fxchat.o \
	build/fxchat_ext.o \
	build/fxchat_errhand.o \
	build/fxchat_stream.o \
	build/fxchat_vm.o \
	build/fxchat_script.o \
	build/fxchat_events.o

all: builddir fxchat.so

fxchat.so: $(OBJECTS)
	g++ $(LDFLAGS) -o fxchat.so $(OBJECTS) $$(falcon-conf -l)

build/%.o : src/%.cpp src/*.h
	g++ -c $$(falcon-conf -c) $(CXXFLAGS) $< -o $@

clean:
	rm -f build/*.o
	rm -f *.so
	rm -f *.tar.gz

dist:
	tar -czvf fxchat.0.9.tar.gz --exclude "*.fam" --exclude ".svn" --exclude "*.tar.gz" --exclude "*.o" .


builddir:
	mkdir -p build

.PHONY: clean builddir

