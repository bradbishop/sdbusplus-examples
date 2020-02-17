#include <cstdlib>
#include <iostream>
#include <sdbusplus/bus.hpp>
#include <sdbusplus/vtable.hpp>
#include <sdbusplus/server/interface.hpp>
#include <sdeventplus/event.hpp>
#include <string>
#include <vector>

struct MyInterface
{
    static const std::vector<sdbusplus::vtable::vtable_t> _vtable;
    sdbusplus::server::interface::interface _interface;

    explicit MyInterface(sdbusplus::bus::bus& bus) : _interface(
        bus,
        "/com/example/sdbusplus/sdevent",
        "com.example.sdbusplus.sdevent",
        &_vtable[0],
        this)
    {

    }

    int methodCallback(
        sd_bus_message* msg,
        sd_bus_error* e)
    {
        auto m = sdbusplus::message::message(msg);
        std::string arg;

        m.read(arg);
        std::cout << ">> com.example.sdbusplus.sdevent.ExampleMethod: " << arg << "\n";
        int converted = 0;

        try
        {
            converted = std::stoi(arg);
        }
        catch (std::invalid_argument&)
        {
            return sd_bus_error_set_errno(e, EINVAL);
        }

        auto reply = m.new_method_return();
        reply.append(converted);
        reply.method_return();

        std::cout << "<< com.example.sdbusplus.sdevent.ExampleMethod: "
                  << converted << "\n";

        return 1;
    }

    static int _methodCallback(
        sd_bus_message* msg,
        void* context,
        sd_bus_error* e)
    {
        auto o = static_cast<MyInterface*>(context);
        return o->methodCallback(msg, e);
    }
};

const std::vector<sdbusplus::vtable::vtable_t> MyInterface::_vtable{
    sdbusplus::vtable::start(),
    sdbusplus::vtable::method(
        "ExampleMethod", // com.example.sdbusplus.sdevent.ExampleMethod
        "s",
        "i",
        _methodCallback),
    sdbusplus::vtable::end()
};

int main(int argc, char *argv[])
{
    auto bus = sdbusplus::bus::new_default();
    auto event = sdeventplus::Event::get_default();
    MyInterface i(bus);

    bus.attach_event(event.get(), SD_EVENT_PRIORITY_NORMAL);
    bus.request_name("com.example.sdbusplus.sdevent");

    event.loop();

    exit(EXIT_SUCCESS);
}
