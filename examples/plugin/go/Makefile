.PHONY: clean install purge

TARGET = go-test.so

$(TARGET): plugin.go
	go build -o $(TARGET) -buildmode=c-shared plugin.go

clean:
	rm -f $(TARGET) *.h

purge: clean $(TARGET)

install:
	cp -f $(TARGET) /usr/local/lib

