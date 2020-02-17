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
    std::string _examplePropertyCurrentValue;

    explicit MyInterface(sdbusplus::bus::bus& bus) : _interface(
        bus,
        "/com/example/sdbusplus",
        "com.example.sdbusplus",
        &_vtable[0],
        this), _examplePropertyCurrentValue("Hello World!")
    {

    }

    int methodCallback(
        sd_bus_message* msg,
        sd_bus_error* e)
    {
        auto m = sdbusplus::message::message(msg);
        std::string arg;

        m.read(arg);
        std::cout << ">> com.example.sdbusplus.ExampleMethod: " << arg << "\n";
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

        std::cout << "<< com.example.sdbusplus.ExampleMethod: "
                  << converted << "\n";

        return 1;
    }

    int propertyGetCallback(
        sd_bus* bus,
        const char* path,
        const char* interface,
        const char* property,
        sd_bus_message* reply,
        sd_bus_error* e)
    {
        std::cout << ">> com.example.sdbusplus.ExampleProperty\n";

        auto r = sdbusplus::message::message(reply);
        r.append(_examplePropertyCurrentValue);

        std::cout << "<< com.example.sdbusplus.ExampleProperty: "
                  << _examplePropertyCurrentValue << "\n";

        return 1;
    }

    int propertySetCallback(
        sd_bus* bus,
        const char* path,
        const char* interface,
        const char* property,
        sd_bus_message* value,
        sd_bus_error* e)
    {
        std::cout << ">> com.example.sdbusplus.ExampleProperty: "
                  << _examplePropertyCurrentValue << "\n";

        auto v = sdbusplus::message::message(value);
        v.read(_examplePropertyCurrentValue);

        std::cout << "<< com.example.sdbusplus.ExampleProperty: "
                  << _examplePropertyCurrentValue << "\n";

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

    static int _propertyGetCallback(
        sd_bus* bus,
        const char* path,
        const char* interface,
        const char* property,
        sd_bus_message* reply,
        void* context,
        sd_bus_error* e)
    {
        auto o = static_cast<MyInterface*>(context);
        return o->propertyGetCallback(bus, path, interface, property, reply, e);
    }

    static int _propertySetCallback(
        sd_bus* bus,
        const char* path,
        const char* interface,
        const char* property,
        sd_bus_message* value,
        void* context,
        sd_bus_error* e)
    {
        auto o = static_cast<MyInterface*>(context);
        return o->propertySetCallback(bus, path, interface, property, value, e);
    }
};

const std::vector<sdbusplus::vtable::vtable_t> MyInterface::_vtable{
    sdbusplus::vtable::start(),
    sdbusplus::vtable::method(
        "ExampleMethod", // com.example.sdbusplus.ExampleMethod
        "s",
        "i",
        _methodCallback),
    sdbusplus::vtable::property(
        "ExampleProperty", // com.example.sdbusplus.ExampleProperty
        "s",
        _propertyGetCallback,
        _propertySetCallback),
    sdbusplus::vtable::end()
};

int main(int argc, char *argv[])
{
    auto bus = sdbusplus::bus::new_default();
    auto event = sdeventplus::Event::get_default();
    MyInterface i(bus);

    bus.attach_event(event.get(), SD_EVENT_PRIORITY_NORMAL);
    bus.request_name("com.example.sdbusplus");

    event.loop();

    exit(EXIT_SUCCESS);
}
