help:
	@echo "Usage:"
	@echo "	make help"
	@echo "	make shared"
	@echo "	make static"
	@echo "	make tests"
	@echo "	make doc"
	@echo "	make cppcheck"
	@echo "	make debug"
	@echo "	make paranoid"
	@echo "	make coverage"
	@echo "	make coverage-report"
	@echo "	make clean"
	@echo "	make update"
	@echo "	make upgrade"
	@echo "Example:"
	@echo "	make static "
	@echo "	make shared VERBOSE=1 ARGS=-j5"

doc:
	rm -rf docs
	if hash doxygen 2>/dev/null; then doxygen; fi
init:
	mkdir -p build
	rm -rf ./build/CMakeCache.txt 
	if [ ! -d external/cmake-ci/cmake ]; then git submodule update --init external/cmake-ci; fi
cppcheck: init
	./external/cmake-ci/scripts/cppcheck-ci.sh
static: init
	cd build && cmake .. -DBUILD_SHARED_LIBS=OFF -DDISABLE_WARNINGS=ON
	cmake --build ./build -- $(or ${ARGS},-j4)
shared: init
	cd build && cmake .. -DBUILD_SHARED_LIBS=ON -DDISABLE_WARNINGS=ON
	cmake --build ./build -- $(or ${ARGS},-j4)
tests: 	init
	cd build && cmake .. -DBUILD_TESTING=ON
	cmake --build ./build -- $(or ${ARGS},-j4)
	cd build && ctest --output-on-failure
paranoid: init
	cd build && cmake .. -DBUILD_TESTING=ON -DPARANOID_WARNINGS=ON
	cmake --build ./build -- $(or ${ARGS},-j4)
debug: init
	cd build && cmake .. -DBUILD_TESTING=ON -DCMAKE_BUILD_TYPE="Debug" -DEXTRA_WARNINGS=ON
	cmake --build ./build -- $(or ${ARGS},-j4)
coverage: init
	cd build && cmake .. -DCODE_COVERAGE=ON -DDISABLE_WARNINGS=ON
	cmake --build ./build -- $(or ${ARGS},-j4)
	cd build && ctest
	./external/cmake-ci/scripts/coverage-report.sh build summary
coverage-report: init
	cd build && cmake .. -DCODE_COVERAGE=ON -DDISABLE_WARNINGS=ON
	cmake --build ./build -- $(or ${ARGS},-j4)
	cd build && ctest
	mkdir -p docs
	mkdir -p docs/html
	./external/cmake-ci/scripts/coverage-report.sh build docs/html/cov-report
clean:
	rm -rf docs
	cd build && make clean
	rm build/CMakeCache.txt
update: init
	./external/cmake-ci/scripts/update.sh
upgrade: update
	./external/cmake-ci/scripts/upgrade.sh no-auto master
