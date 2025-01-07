#include <iostream>
#include <fstream>
#include <vector>
#include <unordered_map>
#include <stdexcept>
#include <limits>
#include <memory>
#include <string>
#include <sstream>
#include <cctype>

// Clasa de baza pentru evenimente
class Eveniment
{
protected:
    std::string nume;
    std::string locatie;
    std::string data;

public:
    Eveniment(const std::string &nume, const std::string &locatie, const std::string &data)
        : nume(nume), locatie(locatie), data(data) {}

    virtual ~Eveniment() {}

    // Metoda pentru afisare detalii eveniment
    virtual void afisare(std::ostream &os) const
    {
        os << "Eveniment: " << nume << ", Locatie: " << locatie << ", Data: " << data << std::endl;
    }

    // Creeaza o copie a obiectului
    virtual std::unique_ptr<Eveniment> clone() const = 0;

    const std::string &getNume() const
    {
        return nume;
    }

    friend std::ostream &operator<<(std::ostream &os, const Eveniment &ev)
    {
        ev.afisare(os);
        return os;
    }
};

// Clasa derivata pentru concerte
class Concert : public Eveniment
{
    std::string genMuzical;

public:
    Concert(const std::string &nume, const std::string &locatie, const std::string &data, const std::string &genMuzical)
        : Eveniment(nume, locatie, data), genMuzical(genMuzical) {}

    void afisare(std::ostream &os) const override
    {
        os << "Tip eveniment: Concert, Eveniment: " << nume
           << ", Locatie: " << locatie
           << ", Data: " << data
           << "\nGen Muzical: " << genMuzical;
    }

    std::unique_ptr<Eveniment> clone() const override
    {
        return std::make_unique<Concert>(*this);
    }
};

// Clasa derivata pentru teatru
class Teatru : public Eveniment
{
    std::string tipPiesa;

public:
    Teatru(const std::string &nume, const std::string &locatie, const std::string &data, const std::string &tipPiesa)
        : Eveniment(nume, locatie, data), tipPiesa(tipPiesa) {}

    void afisare(std::ostream &os) const override
    {
        os << "Tip eveniment: Teatru, Eveniment: " << nume
           << ", Locatie: " << locatie
           << ", Data: " << data
           << "\nTip Piesa: " << tipPiesa;
    }

    std::unique_ptr<Eveniment> clone() const override
    {
        return std::make_unique<Teatru>(*this);
    }
};

// Clasa derivata pentru seminarii
class Seminar : public Eveniment
{
    std::string tema;

public:
    Seminar(const std::string &nume, const std::string &locatie, const std::string &data, const std::string &tema)
        : Eveniment(nume, locatie, data), tema(tema) {}

    void afisare(std::ostream &os) const override
    {
        os << "Tip eveniment: Seminar, Eveniment: " << nume
           << ", Locatie: " << locatie
           << ", Data: " << data
           << "\nTema: " << tema;
    }

    std::unique_ptr<Eveniment> clone() const override
    {
        return std::make_unique<Seminar>(*this);
    }
};

// Clasa de baza pentru utilizatori
class Utilizator
{
protected:
    std::string email;

public:
    Utilizator(const std::string &email) : email(email) {}

    virtual ~Utilizator() {}

    virtual void afisare(std::ostream &os) const
    {
        os << "Utilizator: " << email << std::endl;
    }

    friend std::ostream &operator<<(std::ostream &os, const Utilizator &user)
    {
        user.afisare(os);
        return os;
    }

    std::string getEmail() const { return email; }
};

// Clasa derivata pentru clienti
class Client : public Utilizator
{
    std::vector<std::unique_ptr<Eveniment>> bileteCumparate;

public:
    Client(const std::string &email) : Utilizator(email) {}

    // Adauga bilet la lista clientului
    void adaugaBilet(const Eveniment &eveniment)
    {
        bileteCumparate.push_back(eveniment.clone());
        updateUserTickets();
    }

    void afisare(std::ostream &os) const override
    {
        Utilizator::afisare(os);
        os << "Bilete cumparate:\n";
        for (const auto &bilet : bileteCumparate)
        {
            os << *bilet << std::endl;
        }
    }

private:
    // Salveaza biletele clientului an fisier
    void updateUserTickets()
    {
        std::ofstream file("users_tickets.txt", std::ios_base::app);
        if (file.is_open())
        {
            file << email << ": ";
            for (const auto &ticket : bileteCumparate)
            {
                file << ticket->getNume() << ", ";
            }
            file << std::endl;
            file.close();
        }
        else
        {
            throw std::runtime_error("Nu s-a putut deschide fisierul pentru actualizare bilete.");
        }
    }
};

// Clasa template pentru gestionarea evenimentelor
template <typename T>
class Catalog
{
    std::vector<T> evenimente;

public:
    void adaugaEveniment(const T &eveniment)
    {
        evenimente.push_back(eveniment);
    }

    void afisareEvenimente() const
    {
        if (evenimente.empty())
        {
            std::cout << "Nu exista evenimente disponibile." << std::endl;
            return;
        }
        int index = 1; // Indexare prietenoasa cu utilizatorul
        for (const auto &ev : evenimente)
        {
            std::cout << index++ << ") " << *ev << std::endl;
        }
    }

    T &getEveniment(int index)
    {
        if (index < 0 || index > evenimente.size() - 1)
        {
            throw std::out_of_range("Index invalid! Verificati din nou.");
        }
        return evenimente[index];
    }

    int getNumarEvenimente() const
    {
        return evenimente.size();
    }
};

// Singleton pentru gestionarea utilizatorilor
class UserManager
{
private:
    std::unordered_map<std::string, std::vector<int>> users;
    static UserManager *instance;

    UserManager()
    {
        loadUsersFromFile();
    }

public:
    static UserManager *getInstance()
    {
        if (!instance)
        {
            instance = new UserManager();
        }
        return instance;
    }

    bool userExists(const std::string &email) const
    {
        return users.find(email) != users.end();
    }

    void addUser(const std::string &email)
    {
        if (!userExists(email))
        {
            users[email] = {};
            saveUsersToFile();
        }
    }

    void addTicket(const std::string &email, int ticketIndex)
    {
        if (userExists(email))
        {
            users[email].push_back(ticketIndex);
            saveUsersToFile();
        }
    }

    std::shared_ptr<Client> getClient(const std::string &email)
    {
        if (!userExists(email))
        {
            std::cout << "Utilizatorul nu exista. Se creeaza un cont nou.\n";
            addUser(email);
        }
        else
        {
            std::cout << "Bine ati revenit!\n";
        }
        return std::make_shared<Client>(email);
    }

    std::vector<int> getUserTickets(const std::string &email) const
    {
        if (userExists(email))
        {
            return users.at(email);
        }
        return {};
    }

    bool isNumeric(const std::string &s)
    {
        for (char c : s)
        {
            if (!std::isdigit(static_cast<unsigned char>(c)))
            {
                return false;
            }
        }
        return !s.empty();
    }

    void loadUsersFromFile()
    {
        std::ifstream file("users.txt");
        if (!file.is_open())
        {
            std::cerr << "Eroare: Nu s-a putut deschide fisierul users.txt.\n";
            return;
        }

        std::string line;
        while (getline(file, line))
        {
            std::istringstream iss(line);
            std::string email;
            if (getline(iss, email, ':'))
            {
                email.erase(0, email.find_first_not_of(" \t"));
                email.erase(email.find_last_not_of(" \t") + 1);

                std::vector<int> tickets;
                std::string ticketIndexes;

                if (getline(iss, ticketIndexes))
                {
                    std::istringstream ticketStream(ticketIndexes);
                    std::string ticket;
                    while (getline(ticketStream, ticket, ','))
                    {
                        try
                        {
                            ticket.erase(0, ticket.find_first_not_of(" \t"));
                            ticket.erase(ticket.find_last_not_of(" \t") + 1);
                            if (!ticket.empty() && isNumeric(ticket))
                            {
                                tickets.push_back(std::stoi(ticket));
                            }
                            else if (!ticket.empty())
                            {
                                std::cerr << "Eroare: Index invalid '" << ticket << "' in users.txt.\n";
                            }
                        }
                        catch (const std::exception &e)
                        {
                            std::cerr << "Eroare la procesarea indexului '" << ticket << "': " << e.what() << "\n";
                        }
                    }
                }

                users[email] = tickets;
            }
        }
        file.close();
    }

    void saveUsersToFile() const
    {
        std::ofstream file("users.txt");
        for (const auto &user : users)
        {
            file << user.first << ": ";
            for (size_t i = 0; i < user.second.size(); ++i)
            {
                file << user.second[i];
                if (i != user.second.size() - 1)
                {
                    file << ",";
                }
            }
            file << "\n";
        }
        file.close();
    }
};

UserManager *UserManager::instance = nullptr;

// Functie afisare meniu
void afisareMeniu()
{
    std::cout << "1. Afiseaza evenimente\n";
    std::cout << "2. Cumpara bilet\n";
    std::cout << "3. Afiseaza biletele mele\n";
    std::cout << "4. Iesire\n\n";
    std::cout << "Alege o optiune: ";
}

int main()
{
    Catalog<std::shared_ptr<Eveniment>> catalog;
    catalog.adaugaEveniment(std::make_shared<Concert>("Concert Ed Sheeran", "Arena Nationala", "24.08.2024", "Pop"));
    catalog.adaugaEveniment(std::make_shared<Teatru>("O scrisoare pierduta", "Teatrul National", "13.03.2025", "Comedie"));
    catalog.adaugaEveniment(std::make_shared<Seminar>("Tehnologii AI", "Centrul de Conferinte", "18.01.2025", "Inteligenta Artificiala"));

    UserManager *userManager = UserManager::getInstance();
    std::string email;

    std::cout << "Introduceti adresa de email: ";
    std::cin >> email;
    auto client = userManager->getClient(email);

    int optiune;
    do
    {
        afisareMeniu();
        std::cin >> optiune;

        if (std::cin.fail())
        {
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            continue;
        }

        switch (optiune)
        {
        case 1:
            catalog.afisareEvenimente();
            break;
        case 2:
        {
            std::cout << "Introduceti indexul evenimentului pentru care doriti sa cumparati bilet: ";
            int index;
            std::cin >> index;

            if (std::cin.fail() || index < 1 || index > catalog.getNumarEvenimente())
            {
                std::cout << "Index invalid! Incercati din nou.\n";
                std::cin.clear();
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                break;
            }

            try
            {
                std::shared_ptr<Eveniment> eveniment = catalog.getEveniment(index - 1);
                userManager->addTicket(email, index);
                std::cout << "Bilet cumparat cu succes pentru evenimentul: " << eveniment->getNume() << "\n";
            }
            catch (const std::exception &e)
            {
                std::cout << "Eroare: " << e.what() << "\n";
            }
            break;
        }

        case 3:
        {
            std::vector<int> tickets = userManager->getUserTickets(email);
            if (tickets.empty())
            {
                std::cout << "Nu aveti bilete achizitionate.\n";
            }
            else
            {
                std::cout << "Biletele dumneavoastra:\n";
                for (int index : tickets)
                {
                    try
                    {
                        std::shared_ptr<Eveniment> eveniment = catalog.getEveniment(index - 1);
                        std::cout << "- " << eveniment->getNume() << "\n";
                    }
                    catch (const std::exception &e)
                    {
                        std::cout << "Eroare: " << e.what() << "\n";
                    }
                }
            }
            break;
        }

        case 4:
            std::cout << "Iesire din program.\n";
            break;
        default:
            std::cout << "Optiune invalida. Incercati din nou.\n";
            break;
        }
    } while (optiune != 4);

    return 0;
}
