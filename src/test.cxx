#include <iostream>
#include "aojack.cxx"
using namespace std;

int main() {
    try {
        jackhost host(5555);
        cout << host.getInvite() << endl;
        host.waitJack();

        string opponentName;
        host >> opponentName;
        host << "george";
    } catch (const JackError& error) {
        cerr << error.what() << endl;
    }
}