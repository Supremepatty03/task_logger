#include <string>
#include <fstream>

using namespace std;

class ILogWriter {
public:
    virtual void write(const string& message) = 0;
    virtual bool isOpen() const = 0;
    virtual ~ILogWriter() = default;
};

class FileLogWriter : public ILogWriter {
    std::ofstream out;
public:
    FileLogWriter(const std::string& filename) {
        out.open(filename, std::ios::app);
    }
    void write(const std::string& message) override {
        out << message << std::endl;
    }
    bool isOpen() const override {
        return out.is_open();
    }
};

class SocketLogWriter : public ILogWriter {
    int sock = -1;
    bool connected = false;
public:
    SocketLogWriter(const std::string& address, int port);

    void write(const std::string& message) override;

    bool isOpen() const override;
    ~SocketLogWriter();
};