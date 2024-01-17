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
	@echo "	make install"
	@echo "	make docker-build"
	@echo "	make docker-run"
	@echo "	make docker-rm"
	@echo "Example:"
	@echo "	make static "
	@echo "	make shared VERBOSE=1 ARGS=-j5"
	@echo "	BUILD_SHARED_LIBS=ON make tests"
	@echo "	make install ARGS=\"--prefix ./build/test-install\""

CMAKE ?= cmake
PRJ = `basename ${PWD}`

doc:
	rm -rf docs
	if hash doxygen 2>/dev/null; then doxygen; fi
runup:
	mkdir -p build
	rm -rf ./build/CMakeCache.txt
	if [ ! -d external/cmake-ci/cmake ]; then git submodule update --init external/cmake-ci; fi

init: runup
	${CMAKE} -B ./build
	./external/cmake-ci/scripts/after_make.sh
cppcheck: runup
	./external/cmake-ci/scripts/cppcheck-ci.sh
release: runup
	cd build && ${CMAKE} .. -DDISABLE_WARNINGS=ON
	${CMAKE} --build ./build -- $(or ${ARGS},-j4)
	./external/cmake-ci/scripts/after_make.sh
static: runup
	cd build && ${CMAKE} .. -DBUILD_SHARED_LIBS=OFF -DDISABLE_WARNINGS=ON
	${CMAKE} --build ./build -- $(or ${ARGS},-j4)
	./external/cmake-ci/scripts/after_make.sh
shared: runup
	cd build && ${CMAKE} .. -DBUILD_SHARED_LIBS=ON -DDISABLE_WARNINGS=ON
	${CMAKE} --build ./build -- $(or ${ARGS},-j4)
	./external/cmake-ci/scripts/after_make.sh
tests: 	runup
	cd build && ${CMAKE} .. -DBUILD_TESTING=ON
	${CMAKE} --build ./build -- $(or ${ARGS},-j4)
	cd build && ctest --output-on-failure
	./external/cmake-ci/scripts/after_make.sh
paranoid: runup
	cd build && ${CMAKE} .. -DBUILD_TESTING=ON -DPARANOID_WARNINGS=ON
	${CMAKE} --build ./build -- $(or ${ARGS},-j4)
	./external/cmake-ci/scripts/after_make.sh
debug: runup
	cd build && ${CMAKE} .. -DBUILD_TESTING=ON -DCMAKE_BUILD_TYPE="Debug" -DEXTRA_WARNINGS=ON
	${CMAKE} --build ./build -- $(or ${ARGS},-j4)
	./external/cmake-ci/scripts/after_make.sh
coverage: runup
	cd build && ${CMAKE} .. -DCODE_COVERAGE=ON -DDISABLE_WARNINGS=ON
	${CMAKE} --build ./build -- $(or ${ARGS},-j4)
	./external/cmake-ci/scripts/after_make.sh
	cd build && ctest
	./external/cmake-ci/scripts/coverage-report.sh build summary
coverage-report: runup
	cd build && ${CMAKE} .. -DCODE_COVERAGE=ON -DDISABLE_WARNINGS=ON
	${CMAKE} --build ./build -- $(or ${ARGS},-j4)
	./external/cmake-ci/scripts/after_make.sh
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
install: runup
	${CMAKE} --install ./build ${ARGS}
docker-build:
	docker build -t ${PRJ} -f Dockerfile.build .
docker-run:
	docker run --rm -it ${PRJ}
docker-rm:
	docker rmi ${PRJ}
