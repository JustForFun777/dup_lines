CXX=gcc
CXXFLAGS = -O3 -std=c99

dupl:
	if [ ! -d "build" ]; then mkdir -p "build"; fi
	$(CXX) $(CXXFLAGS) main.c -o build/dupl

project: dupl

test: test_env test_start

test_env: dupl
	if [ ! -d "build/test" ]; then mkdir -p "build"; fi
	mkdir -p build/test
	cp -Rf tests/* build/test/

test_start:
	./build/test/test.sh ./build/

debug: clean
	$(CXX) $(CXXFLAGS) main.c -DDEBUG -o dupl

clean:
	rm -Rf build
