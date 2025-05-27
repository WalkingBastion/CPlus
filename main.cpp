#include <iostream>
#include <vector>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <algorithm>

using namespace std;

struct Product {
    unsigned int code;
    string name;
    string type;
    float base_price;
    float discount;

    float getSalePrice() const {
        return base_price * (1.0f - discount / 100.0f);
    }

    void print() const {
        cout << setw(10) << code << setw(20) << name << setw(15) << type
             << setw(12) << base_price << setw(10) << discount
             << setw(12) << getSalePrice() << '\n';
    }

    string toCSV() const {
        stringstream ss;
        ss << code << "," << name << "," << type << ","
           << base_price << "," << discount;
        return ss.str();
    }

    static Product fromCSV(const string& line) {
        stringstream ss(line);
        string token;
        Product p;
        getline(ss, token, ','); p.code = stoi(token);
        getline(ss, p.name, ',');
        getline(ss, p.type, ',');
        getline(ss, token, ','); p.base_price = stof(token);
        getline(ss, token, ','); p.discount = stof(token);
        return p;
    }
};

class ProductDB {
private:
    vector<Product> products;

public:
    void loadFromFile(const string& filename) {
        ifstream file(filename);
        string line;
        products.clear();
        while (getline(file, line)) {
            if (!line.empty())
                products.push_back(Product::fromCSV(line));
        }
        file.close();
        cout << "База данных загружена.\n";
    }

    void saveToFile(const string& filename) {
        ofstream file(filename);
        for (const auto& p : products)
            file << p.toCSV() << '\n';
        file.close();
        cout << "База данных сохранена.\n";
    }

    void viewTable() const {
        cout << setw(10) << "Код" << setw(20) << "Название"
             << setw(15) << "Тип" << setw(12) << "Цена"
             << setw(10) << "Скидка" << setw(12) << "Продажа\n";
        cout << string(79, '-') << '\n';
        for (const auto& p : products)
            p.print();
    }

    void addProduct() {
        Product p;
        cout << "Введите код товара: "; cin >> p.code;
        cout << "Введите название: "; cin.ignore(); getline(cin, p.name);
        cout << "Введите тип: "; getline(cin, p.type);
        cout << "Введите цену за штуку: "; cin >> p.base_price;
        cout << "Введите скидку (%): "; cin >> p.discount;
        products.push_back(p);
        cout << "Товар добавлен.\n";
    }

    void deleteProductByIndex(int index) {
        if (index < 0 || index >= products.size()) {
            cout << "Неверный номер.\n";
            return;
        }
        products.erase(products.begin() + index);
        cout << "Товар удален.\n";
    }

    void sortByCode() {
        sort(products.begin(), products.end(),
             [](const Product& a, const Product& b) { return a.code < b.code; });
        cout << "БД отсортирована по коду.\n";
    }

    void searchByCode(unsigned int code) const {
        for (const auto& p : products) {
            if (p.code == code) {
                p.print();
                return;
            }
        }
        cout << "Товар не найден.\n";
    }

    void selectByPriceRange(float minPrice, float maxPrice) const {
        for (const auto& p : products) {
            float price = p.base_price;
            if (price >= minPrice && price <= maxPrice)
                p.print();
        }
    }

    void applyPromoDiscount(const vector<string>& types, float extraDiscount) {
        for (auto& p : products) {
            if (find(types.begin(), types.end(), p.type) != types.end())
                p.discount += extraDiscount;
        }
        cout << "Скидка по акции применена.\n";
    }

    void removeCheaperThan(float threshold) {
        products.erase(remove_if(products.begin(), products.end(),
            [threshold](const Product& p) {
                return p.getSalePrice() < threshold;
            }), products.end());
        cout << "Удалены товары дешевле " << threshold << ".\n";
    }
};

// Меню
void showMenu() {
    cout << "\n1. Загрузить БД из файла\n"
         << "2. Просмотр таблицы\n"
         << "3. Добавить запись\n"
         << "4. Удалить запись по номеру\n"
         << "5. Сохранить БД в файл\n"
         << "6. Сортировка по коду\n"
         << "7. Поиск по коду\n"
         << "8. Выборка по диапазону цен\n"
         << "9. Применить акционную скидку\n"
         << "10. Удалить товары дешевле заданной цены\n"
         << "0. Выход\n"
         << "Выберите пункт меню: ";
}

int main() {
    ProductDB db;
    int choice;
    do {
        showMenu();
        cin >> choice;
        switch (choice) {
        case 1: {
            string filename;
            cout << "Имя файла: ";
            cin >> filename;
            db.loadFromFile(filename);
            break;
        }
        case 2:
            db.viewTable();
            break;
        case 3:
            db.addProduct();
            break;
        case 4: {
            int index;
            cout << "Введите номер записи (начиная с 0): ";
            cin >> index;
            db.deleteProductByIndex(index);
            break;
        }
        case 5: {
            string filename;
            cout << "Имя файла: ";
            cin >> filename;
            db.saveToFile(filename);
            break;
        }
        case 6:
            db.sortByCode();
            break;
        case 7: {
            unsigned int code;
            cout << "Введите код товара: ";
            cin >> code;
            db.searchByCode(code);
            break;
        }
        case 8: {
            float minPrice, maxPrice;
            cout << "Введите минимальную и максимальную цену: ";
            cin >> minPrice >> maxPrice;
            db.selectByPriceRange(minPrice, maxPrice);
            break;
        }
        case 9: {
            int n;
            float extraDiscount;
            cout << "Сколько типов участвует в акции? ";
            cin >> n;
            vector<string> types(n);
            cin.ignore();
            for (int i = 0; i < n; ++i) {
                cout << "Тип " << i + 1 << ": ";
                getline(cin, types[i]);
            }
            cout << "Введите размер дополнительной скидки (%): ";
            cin >> extraDiscount;
            db.applyPromoDiscount(types, extraDiscount);
            break;
        }
        case 10: {
            float threshold;
            cout << "Введите порог цены продажи: ";
            cin >> threshold;
            db.removeCheaperThan(threshold);
            break;
        }
        }
    } while (choice != 0);
    return 0;
}
