#include <cstdlib>
#include <iostream>
#include <sdbusplus/asio/connection.hpp>
#include <sdbusplus/asio/object_server.hpp>
#include <string>

int exampleMethodCallback(const std::string& arg)
{
    std::cout << ">> com.example.sdbusplus.asio.ExampleMethod: " << arg << "\n";
    int converted = 0;
    try
    {
        converted = std::stoi(arg);
    }
    catch (std::invalid_argument&)
    {
        throw sdbusplus::exception::SdBusError(-EINVAL,
            "prefix");
    }

    std::cout << "<< com.example.sdbusplus.asio.ExampleMethod: "
              << converted << "\n";

    return converted;
}

int main(int argc, char* argv[])
{
    boost::asio::io_context io;
    auto bus = std::make_shared<sdbusplus::asio::connection>(io);
    sdbusplus::asio::object_server server(bus);

    auto iface = server.add_interface(
        "/com/example/sdbusplus/asio", "com.example.sdbusplus.asio");

    iface->register_method("ExampleMethod", exampleMethodCallback);
    iface->initialize();
    bus->request_name("com.example.sdbusplus.asio");

    io.run();
    exit(EXIT_SUCCESS);
}
