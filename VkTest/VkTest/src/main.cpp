#include <Core/Sandbox.h>

#include <iostream>
#include <stdexcept>

int main() {
    Sandbox sandbox;

    try {
        sandbox.Run();
    }
    catch (std::runtime_error& e) {
        std::cerr << e.what() << '\n';
    }
    catch (std::exception& e) {
        std::cerr << e.what() << '\n';
    }

    return 0;
}