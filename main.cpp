#include <sstream>      // pro práci s proudy v pamìti (std::ostringstream, std::istringstream)
#include <iostream>     // pro standardní vstup/výstup (std::cout, std::cerr)
#include <string>       // pro práci s textovými øetìzci (std::string)
#include <tuple>        // pro datovou strukturu tuple (std::tuple)
#include <type_traits>  // pro typové introspekce (napø. std::is_same_v)
#include <stdexcept>    // pro signalizaci chyb (std::runtime_error)

/**
 * @brief Serializuje zadaná data do jednoho textového øetìzce.
 *
 * Tato funkce pøijímá libovolný poèet argumentù a spojí je do jednoho øetìzce,
 * kde jsou jednotlivé hodnoty oddìleny koncem øádku ('\n').
 *
 * @tparam Args Typy argumentù, které mají být serializovány.
 * @param args Hodnoty, které budou serializovány.
 * @return std::string Serializovaná data ve formì textového øetìzce.
 */
template <typename... Args>
std::string Serialize(const Args&... args)
{
    std::ostringstream os;
    ((os << args << '\n'), ...); // Oddìlení polí pomocí '\n'
    return os.str();
}

/**
 * @brief Deserializuje textový øetìzec zpìt na zadané datové typy.
 *
 * Tato funkce naète jednotlivé hodnoty z øetìzce a pøevede je na zadané typy.
 * Pro øetìzce používá std::getline a pro èíselné hodnoty operátor '>>'.
 *
 * @tparam Args Typy hodnot, které mají být deserializovány.
 * @param data Serializovaný textový øetìzec obsahující hodnoty oddìlené '\n'.
 * @return std::tuple<Args...> Hodnoty naètené a pøevedené na odpovídající typy.
 * @throws std::runtime_error Pokud se nepodaøí naèíst nebo pøevést hodnotu.
 */
template <typename... Args>
std::tuple<Args...> Deserialize(const std::string& data)
{
    std::istringstream is(data);
    std::tuple<Args...> result;

    // Pro každý prvek n-tice naèteme odpovídající hodnotu
    std::apply([&](auto&... fields) {
        ([&] {
            if constexpr (std::is_same_v<std::decay_t<decltype(fields)>, std::string>)
            {
                // Pro øetìzce èteme celý øádek
                if (!std::getline(is, fields))
                    throw std::runtime_error("Failed to deserialize string field!");
            }
            else
            {
                // Pro èíselné typy
                if (!(is >> fields))
                    throw std::runtime_error("Failed to deserialize numeric field!");
                is.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); // Ignorujeme zbytek øádku
            }
            }(), ...); // Rozbalení parametrù
        }, result);

    return result;
}

int main()
{
    // Pøíklad dat
    int packetType = 123456;                      // Èíselný typ pro identifikaci balíèku
    std::string message = "Hello, World!";        // Textová zpráva
    long long id = 29;                            // Èíslo ID
    float someFloat = 3.14f;                      // Hodnota s plovoucí desetinnou èárkou

    // Serializace
    std::string serializedData = Serialize(packetType, message, id, someFloat);
    std::cout << "Serialized Data:\n" << serializedData << std::endl;

    // Deserializace
    try
    {
        auto [deserializedType, deserializedMessage, deserializedId, deserializedFloat] =
            Deserialize<int, std::string, long long, float>(serializedData);

        std::cout << "Deserialized Data:\n";
        std::cout << "Type: " << deserializedType << "\n";
        std::cout << "Message: " << deserializedMessage << "\n";
        std::cout << "ID: " << deserializedId << "\n";
        std::cout << "Float: " << deserializedFloat << "\n";
    }
    catch (const std::exception& e)
    {
        std::cerr << "Error: " << e.what() << std::endl;
    }

    return 0;
}
