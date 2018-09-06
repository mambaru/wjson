all:
	mkdir -p build
	cd build && cmake .. -DCMAKE_BUILD_TYPE=Release -DBUILD_TESTING=ON -DCMAKE_CXX_STANDRD=11
	cmake --build ./build 
	cd build && ctest
doc:
	rm -rf ./docs
	if hash doxygen 2>/dev/null; then doxygen; fi
clean:
	rm -rf docs

