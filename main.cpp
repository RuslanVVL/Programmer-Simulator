#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <cstdlib>
#include <ctime>

struct Money {
    int rubles;
    int kopecks;

    void normalize() {
        if (kopecks >= 100) {
            rubles += kopecks / 100;
            kopecks %= 100;
        }
        if (kopecks < 0 && rubles > 0) {
            rubles -= 1 + (-kopecks) / 100;
            kopecks = 100 - (-kopecks) % 100;
        }
    }
};

union Bonus {
    int flatAmount;
    double multiplier;
};

class Employee {
protected:
    std::string name;
    int baseIncomeKopecks;

public:
    Employee(std::string empName, int incomeKopecks) : name(empName), baseIncomeKopecks(incomeKopecks) {}
    virtual ~Employee() = default;

    virtual void work() = 0; 
    virtual int getIncomeKopecks() const { return baseIncomeKopecks; }
    std::string getName() const { return name; }
};

class JuniorDev : public Employee {
public:
    JuniorDev(std::string name) : Employee(name, 500) {}

    void work() override {
        std::cout << "[!] " << name << " пишет код со звуками паники...\n";
    }
};

class SeniorDev : public Employee {
private:
    double activeMultiplier = 1.0;
public:
    SeniorDev(std::string name, Bonus bonus, bool isMultiplier) : Employee(name, 2500) {
        if (isMultiplier) {
            activeMultiplier = bonus.multiplier;
        }
    }

    void work() override {
        std::cout << "[!] " << name << " исправил баг одним взглядом! ";
        if (activeMultiplier > 1.0) {
            std::cout << "(Эффективность x" << activeMultiplier << ")";
        }
        std::cout << "\n";
    }

    int getIncomeKopecks() const override {
        return static_cast<int>(baseIncomeKopecks * activeMultiplier);
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
    std::srand(static_cast<unsigned int>(std::time(nullptr)));

    Money companyWallet = {0, 0};
    int officeLevel = 1;

    const int MENU_SIZE = 4;
    std::string actionMenu[MENU_SIZE] = {
        "Кликнуть (написать строчку кода)",
        "Нанять Джуниора (стоит 150 руб)",
        "Нанять Сеньора (стоит 500 руб)",
        "Прокачать офис (рекурсивный расчет цены)"
    };

    std::vector<Employee*> team;

    std::cout << "=== ДОБРО ПОЖАЛОВАТЬ В СТАРТАП СИМУЛЯТОР ===\n";

    int choice = -1;
    while (choice != 0) {
        std::cout << "\n----------------------------------------\n";
        std::cout << "Баланс компании: " << companyWallet.rubles << "." 
                  << (companyWallet.kopecks < 10 ? "0" : "") << companyWallet.kopecks << " руб.\n";
        std::cout << "Уровень офиса: " << officeLevel << "\n";
        std::cout << "Сотрудников в штате: " << team.size() << "\n";

        showMenu(actionMenu, MENU_SIZE);
        
        if (!(std::cin >> choice)) {
            std::cout << "[X] Ошибка ввода! Введите число.\n";
            std::cin.clear();
            std::cin.ignore(10000, '\n');
            continue;
        }
        std::cout << "\n----------------------------------------\n";

        switch (choice) {
            case 1: {
                int clickRubles = 10 * officeLevel;
                int clickKopecks = 50 * officeLevel;
                
                companyWallet.rubles += clickRubles;
                companyWallet.kopecks += clickKopecks;
                companyWallet.normalize();

                std::cout << "[+] Вы заработали " << clickRubles << " руб. " << clickKopecks << " коп.!\n";

                std::for_each(team.begin(), team.end(), [&companyWallet](Employee* emp) {
                    emp->work();
                    companyWallet.kopecks += emp->getIncomeKopecks();
                });
                companyWallet.normalize();
                break;
            }
            case 2: {
                if (companyWallet.rubles >= 150) {
                    companyWallet.rubles -= 150;
                    team.push_back(new JuniorDev("Джун Вася"));
                    std::cout << "[+] Вы наняли Джуниора Васю! Теперь он паникует за деньги.\n";
                } else {
                    std::cout << "[X] Не хватает денег! Нужно 150 руб.\n";
                }
                break;
            }
            case 3: {
                if (companyWallet.rubles >= 500) {
                    companyWallet.rubles -= 500;

                    Bonus seniorBonus;
                    bool isMultiplier = std::rand() % 2 == 0;

                    if (isMultiplier) {
                        seniorBonus.multiplier = 1.2 + (std::rand() % 4) * 0.1;
                        team.push_back(new SeniorDev("Сеньор Петр", seniorBonus, true));
                        std::cout << "[+] Нанят Сеньор Петр с бонусом: скорость кодинга x" << seniorBonus.multiplier << "!\n";
                    } else {
                        seniorBonus.flatAmount = 5000 + (std::rand() % 10001);
                        companyWallet.kopecks += seniorBonus.flatAmount;
                        companyWallet.normalize();
                        
                        Bonus dummyBonus; dummyBonus.multiplier = 1.0;
                        team.push_back(new SeniorDev("Сеньор Петр", dummyBonus, true));
                        
                        std::cout << "[+] Нанят Сеньор Петр! Получен грант " 
                                  << seniorBonus.flatAmount / 100 << " руб. " 
                                  << seniorBonus.flatAmount % 100 << " коп. за его привлечение!\n";
                    }
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
                        std::cout << "[▲] Офис успешно улучшен! Клики стали эффективнее.\n";
                    } else {
                        std::cout << "[X] Не хватает денег для апгрейда!\n";
                    }
                }
                break;
            }
            case 0:
                std::cout << "Выход из игры. Ваш итоговый счет: " << companyWallet.rubles << "." 
                          << (companyWallet.kopecks < 10 ? "0" : "") << companyWallet.kopecks << " руб.\n";
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