#ifndef FUNCTIONS_H
#define FUNCTIONS_H
#define FILE_NAME_WALLETS "wallets.dat"
#define FILE_NAME_TRANS "transactions.dat"
#define FILE_NAME_ORDERS "orders.dat"
#include <fstream>
using namespace std;

static bool haveToUpdateWalletsArray = true;
static bool haveToUpdateWalletsFile = true;
static bool haveToUpdateTransactionsArray = true;


struct Wallet
{
	double fiatMoney;
	unsigned id;
	char name[255];
};
struct Transaction {
	long long time;
	unsigned senderId;
	unsigned receiverId;
	double fmiCoins;
};
struct Order {
	enum Type { SELL, BUY } type;
	unsigned walletId;
	double fmiCoins;
};
struct OrdersArray
{
	int size;
	Order * orders;
};
unsigned fillWallet(char * name, double fiatMoney,unsigned id, Wallet & w);
int loadWallets(Wallet *& wallets);//*& because I allocate memory in this function 
void writeWallet(const Wallet &w);// need update after using // update flag true
int addWallet(char * name, double fiatMoney, unsigned id, Wallet *& wallets);
int findWallet(unsigned id, const Wallet * wallets, unsigned cnt);
void rewriteWallets(const Wallet * wallets, int firstWallet, int secondWallet);//update flag false
void showFirstTen(Wallet * wallets, int cnt, Transaction * trans, int transCnt);
double calculateFmicoins(Transaction * trans, int transCnt, Wallet &wallet);
void printWallet(Wallet& w, Transaction * trans, int transCnt);
void walletInfo(Wallet * wallets, int walletsCnt, int walletID, Transaction * trans, int transCnt);

int fileSize(const char*);
//transactons
void fillTransaction(unsigned senderID, unsigned recieverID, double FMIcoins, Transaction & t);
void writeTransaction(const Transaction & t);// update after using it//update flag true
int loadTran(Transaction *& transactions);//update flag false
void writeTransactionToTxt(const Transaction & t, const char* fileName);

//orders
bool fillOrder(const char* type, unsigned walletId, double fmiCoins, Order & order);
bool checkOrder(Order & order, Wallet *& wallets, Transaction *& trans, OrdersArray & arr);
void loadOrders(OrdersArray & arr);//sync flag false 
bool zeroOrder(OrdersArray & orders, int i);
bool writeOrder(const Order & o);
bool writeOrdersArray(const OrdersArray& arr);//sync flag false 
void addToArray(const Order & o, OrdersArray & arr);// sync flag 

//other
void makeFileName(unsigned id, char fileName[]);
int input(Wallet *& wallets, OrdersArray& arr, Transaction *& trans, unsigned * lastID);
//TODO:
//loadOrders and loadTran
//writeOrder
//iterate Orders and check if there are suitable

#endif