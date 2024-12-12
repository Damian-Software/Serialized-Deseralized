#include <sstream>      // pro pr�ci s proudy v pam�ti (std::ostringstream, std::istringstream)
#include <iostream>     // pro standardn� vstup/v�stup (std::cout, std::cerr)
#include <string>       // pro pr�ci s textov�mi �et�zci (std::string)
#include <tuple>        // pro datovou strukturu tuple (std::tuple)
#include <type_traits>  // pro typov� introspekce (nap�. std::is_same_v)
#include <stdexcept>    // pro signalizaci chyb (std::runtime_error)

/**
 * @brief Serializuje zadan� data do jednoho textov�ho �et�zce.
 *
 * Tato funkce p�ij�m� libovoln� po�et argument� a spoj� je do jednoho �et�zce,
 * kde jsou jednotliv� hodnoty odd�leny koncem ��dku ('\n').
 *
 * @tparam Args Typy argument�, kter� maj� b�t serializov�ny.
 * @param args Hodnoty, kter� budou serializov�ny.
 * @return std::string Serializovan� data ve form� textov�ho �et�zce.
 */
template <typename... Args>
std::string Serialize(const Args&... args)
{
    std::ostringstream os;
    ((os << args << '\n'), ...); // Odd�len� pol� pomoc� '\n'
    return os.str();
}

/**
 * @brief Deserializuje textov� �et�zec zp�t na zadan� datov� typy.
 *
 * Tato funkce na�te jednotliv� hodnoty z �et�zce a p�evede je na zadan� typy.
 * Pro �et�zce pou��v� std::getline a pro ��seln� hodnoty oper�tor '>>'.
 *
 * @tparam Args Typy hodnot, kter� maj� b�t deserializov�ny.
 * @param data Serializovan� textov� �et�zec obsahuj�c� hodnoty odd�len� '\n'.
 * @return std::tuple<Args...> Hodnoty na�ten� a p�eveden� na odpov�daj�c� typy.
 * @throws std::runtime_error Pokud se nepoda�� na��st nebo p�ev�st hodnotu.
 */
template <typename... Args>
std::tuple<Args...> Deserialize(const std::string& data)
{
    std::istringstream is(data);
    std::tuple<Args...> result;

    // Pro ka�d� prvek n-tice na�teme odpov�daj�c� hodnotu
    std::apply([&](auto&... fields) {
        ([&] {
            if constexpr (std::is_same_v<std::decay_t<decltype(fields)>, std::string>)
            {
                // Pro �et�zce �teme cel� ��dek
                if (!std::getline(is, fields))
                    throw std::runtime_error("Failed to deserialize string field!");
            }
            else
            {
                // Pro ��seln� typy
                if (!(is >> fields))
                    throw std::runtime_error("Failed to deserialize numeric field!");
                is.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); // Ignorujeme zbytek ��dku
            }
            }(), ...); // Rozbalen� parametr�
        }, result);

    return result;
}

int main()
{
    // P��klad dat
    int packetType = 123456;                      // ��seln� typ pro identifikaci bal��ku
    std::string message = "Hello, World!";        // Textov� zpr�va
    long long id = 29;                            // ��slo ID
    float someFloat = 3.14f;                      // Hodnota s plovouc� desetinnou ��rkou

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
