help:
	@echo "Usage:"
	@echo "	make help"
	@echo "	make shared"
	@echo "	make static"
	@echo "	make release"
	@echo "	make tests"
	@echo "	make doc"
	@echo "	make cppcheck"
	@echo "	make debug"
	@echo "	make paranoid"
	@echo "	make coverage"
	@echo "	make coverage-report"
	@echo "	make init"
	@echo "	make clean"
	@echo "	make update"
	@echo "	make upgrade"
	@echo "Example:"
	@echo "	make static "
	@echo "	make shared VERBOSE=1 ARGS=-j5"
	@echo "	BUILD_SHARED=OFF make tests"

doc:
	rm -rf docs
	if hash doxygen 2>/dev/null; then doxygen; fi
runup:
	mkdir -p build
	rm -rf ./build/CMakeCache.txt
	if [ ! -d external/cmake-ci/cmake ]; then git submodule update --init external/cmake-ci; fi
init: runup
	cmake -B ./build
cppcheck: runup
	./external/cmake-ci/scripts/cppcheck-ci.sh
release: runup
	cd build && cmake .. -DDISABLE_WARNINGS=ON
	cmake --build ./build -- $(or ${ARGS},-j4)
static: runup
	cd build && cmake .. -DBUILD_SHARED_LIBS=OFF -DDISABLE_WARNINGS=ON
	cmake --build ./build -- $(or ${ARGS},-j4)
shared: runup
	cd build && cmake .. -DBUILD_SHARED_LIBS=ON -DDISABLE_WARNINGS=ON
	cmake --build ./build -- $(or ${ARGS},-j4)
tests: 	runup
	cd build && cmake .. -DBUILD_TESTING=ON
	cmake --build ./build -- $(or ${ARGS},-j4)
	cd build && ctest --output-on-failure
paranoid: runup
	cd build && cmake .. -DBUILD_TESTING=ON -DPARANOID_WARNINGS=ON
	cmake --build ./build -- $(or ${ARGS},-j4)
debug: runup
	cd build && cmake .. -DBUILD_TESTING=ON -DCMAKE_BUILD_TYPE="Debug" -DEXTRA_WARNINGS=ON
	cmake --build ./build -- $(or ${ARGS},-j4)
coverage: runup
	cd build && cmake .. -DCODE_COVERAGE=ON -DDISABLE_WARNINGS=ON
	cmake --build ./build -- $(or ${ARGS},-j4)
	cd build && ctest
	./external/cmake-ci/scripts/coverage-report.sh build summary
coverage-report: runup
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
update: runup
	./external/cmake-ci/scripts/update.sh
upgrade: update
	./external/cmake-ci/scripts/upgrade.sh
