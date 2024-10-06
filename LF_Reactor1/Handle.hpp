#ifndef HANDLE_HPP
#define HANDLE_HPP

#include <iostream>
#include <functional>
#include <memory>

class Handle {
public:
    virtual ~Handle() = default;

    // Method to be overridden by concrete handles
    virtual void process() = 0;

    // Optional: Method to get the status or type of the handle
    virtual std::string getType() const = 0;
};

using HandlePtr = std::shared_ptr<Handle>;

#endif // HANDLE_HPP

