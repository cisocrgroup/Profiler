

#include "./TestINIConfig.h"

int main() {
    std::locale::global(std::locale( "" ));

    csl::TestINIConfig ti;

    try {
	ti.run();
    } catch( std::exception& exc ) {
            std::cout << "Exception: " << exc.what() << std::endl;

    }

};
