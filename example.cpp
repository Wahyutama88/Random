#include <iostream>
#include <Random.h>

int main()
{
    Random<double> Rng(0, 1555);
    // or as an int generator
    // Random<int> Rng(0, 1555);


    for (auto i = 0; i < 89; ++i)
        std::cout << Rng.Generate() << ", ";
    std::cout << "\b\b " << std::endl << Rng.Seed() << std::endl;

    Rng.push_state();
    for (auto i = 0; i < 89; ++i)
        std::cout << Rng.Generate() << ", ";
    std::cout << "\b\b " << std::endl << Rng.Seed() << std::endl;

    Rng.Seed(500);
    for (auto i = 0; i < 89; ++i)
        std::cout << Rng.Generate() << ", ";
    std::cout << "\b\b " << std::endl << Rng.Seed() << std::endl;

    Rng.pop_state();
    for (auto i = 0; i < 89; ++i)
        std::cout << Rng.Generate() << ", ";
    std::cout << "\b\b " << std::endl << Rng.Seed() << std::endl;

    Rng.Reset(89, 0, 89);
    for (auto i = 0; i < 89; ++i)
        std::cout << Rng.Generate() << ", ";
    std::cout << "\b\b " << std::endl << Rng.Seed() << std::endl;
}
