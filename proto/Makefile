all:
	rm -rf build
	mkdir -p build build/cpp build/python
	protoc --cpp_out=./build/cpp --python_out=./build/python ./account.proto
