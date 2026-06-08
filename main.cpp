#include <iostream>
#include <string>
#include <vector>
#include <algorithm>

struct Money {
    int rubles;
    int kopecks;
};

union Bonus {
    int flatAmount;
    double multiplier;
};

class Employee {
protected:
    std::string name;
    int baseIncome;

public:
    Employee(std::string empName, int income) : name(empName), baseIncome(income) {}
    virtual ~Employee() = default;

    virtual void work() = 0; 
    virtual int getIncome() const { return baseIncome; }
    std::string getName() const { return name; }
};

class JuniorDev : public Employee {
public:
    JuniorDev(std::string name) : Employee(name, 5) {}

    void work() override {
        std::cout << "[!] " << name << " пишет код со звуками паники...\n";
    }
};

class SeniorDev : public Employee {
public:
    SeniorDev(std::string name) : Employee(name, 25) {}

    void work() override {
        std::cout << "[!] " << name << " исправил баг одним взглядом!\n";
    }
};

int calculateUpgradeCost(int currentLevel) {
    if (currentLevel <= 1) return 100;
    return 2 * calculateUpgradeCost(currentLevel - 1);
}

void showMenu(std::string menuItems[], int size) {
    std::cout << "\n=== ДОСТУПНЫЕ ДЕЙСТВИЯ ===\n";
    for (int i = 0; i < size; i++) {
        std::cout << i + 1 << ". " << menuItems[i] << "\n";
    }
    std::cout << "Выберите действие (или 0 для выхода): ";
}

int main() {
    std::system("chcp 65001 > nul");

    Money companyWallet = {0, 0};
    int officeLevel = 1;

    const int MENU_SIZE = 4;
    std::string actionMenu[MENU_SIZE] = {
        "Кликнуть (написать строчку кода сам)",
        "Нанять Джуниора (стоит 150 руб)",
        "Нанять Сеньора (стоит 500 руб)",
        "Прокачать офис (рекурсивный расчет цены)"
    };

    std::vector<Employee*> team;

    std::cout << "=== ДОБРО ПОЖАЛОВАТЬ В СТАРТАП СИМУЛЯТОР ===\n";

    int choice = -1;
    while (choice != 0) {
        std::cout << "\n----------------------------------------\n";
        std::cout << "Баланс компании: " << companyWallet.rubles << " руб.\n";
        std::cout << "Уровень офиса: " << officeLevel << "\n";
        std::cout << "Сотрудников в штате: " << team.size() << "\n";

        showMenu(actionMenu, MENU_SIZE);
        std::cin >> choice;
        std::cout << "\n----------------------------------------\n";

        switch (choice) {
            case 1: {
                int clickIncome = 10 * officeLevel;
                companyWallet.rubles += clickIncome;
                std::cout << "[+] Вы написали код и заработали " << clickIncome << " руб.!\n";

                std::for_each(team.begin(), team.end(), [&companyWallet](Employee* emp) {
                    emp->work();
                    companyWallet.rubles += emp->getIncome();
                });
                break;
            }
            case 2: {
                if (companyWallet.rubles >= 150) {
                    companyWallet.rubles -= 150;
                    team.push_back(new JuniorDev("Джун Вася"));
                    std::cout << "[+] Вы наняли Джуниора Васю! Теперь он помогает вам кодить.\n";
                } else {
                    std::cout << "[X] Не хватает денег! Нужно 150 руб.\n";
                }
                break;
            }
            case 3: {
                if (companyWallet.rubles >= 500) {
                    companyWallet.rubles -= 500;
                    team.push_back(new SeniorDev("Сеньор Петр"));
                    std::cout << "[+] Вы наняли Сеньора Петра! Доходы резко растут.\n";
                } else {
                    std::cout << "[X] Не хватает денег! Нужно 500 руб.\n";
                }
                break;
            }
            case 4: {
                int cost = calculateUpgradeCost(officeLevel + 1);
                std::cout << "Стоимость улучшения офиса до " << officeLevel + 1 << " уровня: " << cost << " руб.\n";
                std::cout << "Желаете улучшить? (1 - да, 0 - нет): ";
                int upgradeChoice;
                std::cin >> upgradeChoice;

                if (upgradeChoice == 1) {
                    if (companyWallet.rubles >= cost) {
                        companyWallet.rubles -= cost;
                        officeLevel++;
                        std::cout << "[▲] Офис успешно улучшен! Ваши личные клики стали эффективнее.\n";
                    } else {
                        std::cout << "[X] Не хватает денег для апгрейда!\n";
                    }
                }
                break;
            }
            case 0:
                std::cout << "Выход из игры. Ваш итоговый счет: " << companyWallet.rubles << " руб.\n";
                break;
            default:
                std::cout << "Неверный пункт меню!\n";
                break;
        }
    }

    for (auto emp : team) {
        delete emp;
    }
    team.clear();

    return 0;
}