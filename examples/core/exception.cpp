#include <spine/core/debugging.hpp>
#include <spine/core/exception.hpp>
#include <spine/core/logging.hpp>

int main() {
    using namespace spn::core;

    class MyExceptionHandler final : public ExceptionHandler {
    public:
        MyExceptionHandler() {}

        void handle_exception(const spn::core::Exception& exception) override {
            ERR("MyExceptionHandler: Handling exception: %s", exception.error_type());
            // perform a safe shutdown
        }
    };

    set_machine_exception_handler(std::make_unique<MyExceptionHandler>()); // set the machine's global exception handler

    assert(false); // when this assertion fails, an exception will be raised. The machine's exception handler will then
                   // be called

    return 0;
}