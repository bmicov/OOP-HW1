#include "funcAndStruct.h"
#include <cstring>
#include <fstream>
#include <iostream>
#include<ctime>

using namespace std;
double calculateFmicoins(Transaction * trans, int transCnt, Wallet &wallet)
{
	double fmiCoinsWalletID = 0;
	for (int i = 0; i < transCnt; i++)
	{
		if (trans[i].receiverId == wallet.id)	fmiCoinsWalletID += trans[i].fmiCoins;//count received coins
		if (trans[i].senderId == wallet.id)	fmiCoinsWalletID -= trans[i].fmiCoins;//count sent coins
	}
	return fmiCoinsWalletID;
}
void walletInfo(Wallet * wallets, int walletsCnt, int walletID, Transaction * trans, int transCnt)
{
	for (int i = 0; i < walletsCnt; i++)
	{
		if (walletID == wallets[i].id)
		{
			double fmiCoins = calculateFmicoins(trans, transCnt, wallets[i]);
			cout << wallets[i].name << " " << wallets[i].fiatMoney << " " << fmiCoins;
			return;
		}
	}
}
void printWallet(Wallet& w, Transaction * trans, int transCnt)
{
	int cnt = 0;
	long long firstTime;
	long long lastTime;
	bool isFirst = true;
	for (int i = 0; i < transCnt; i++)
	if (trans[i].receiverId == w.id || trans[i].senderId == w.id)
	{
		cnt++;
		if (isFirst)
		{
			firstTime = trans[i].time;
			isFirst = false;
		}
		lastTime = trans[i].time;

	}
	if (!isFirst)// we have more than one 
		cout << calculateFmicoins(trans, transCnt, w) << " " << cnt << " " << firstTime << " " << lastTime << endl;

	
}
void showFirstTen(Wallet * wallets, int cnt, Transaction * trans, int transCnt)
{
	double max=0;
	Wallet * maxPtr=nullptr;
	Wallet * current=nullptr;
	Wallet * nextCurrent=nullptr;
	double * arrayFmiCoins = new double[cnt];
	for (int i = 0; i < cnt; i++)
	{
		arrayFmiCoins[i] = calculateFmicoins(trans, transCnt, wallets[i]);
	}

	if (cnt <= 10)
	{
		for (int i = 0; i < cnt; i++)
			printWallet(wallets[i], trans, transCnt);
		return;
	}
		
	for (int i = 0; i < 10; i++)
	{
		max = arrayFmiCoins[i];
		maxPtr = &wallets[i];//wrong we are skipping the first but we have to skip which is printed
		int maxindx = 0;
		for (int k = 0; k < cnt; k++)
		{
			if (max < arrayFmiCoins[k])
			{
				maxPtr = &wallets[k];
				max = arrayFmiCoins[k];
				maxindx = k;
			}	
		}
		arrayFmiCoins[maxindx] = -1;
		printWallet(*maxPtr, trans, transCnt);
		cout << endl;
	}

	
}
int fileSize(const char * fileName)
{
	ifstream in(fileName, ios::binary | ios::ate);
	int size = in.tellg();
	in.close();
	if (size == -1)return 0;
	return size;
}
unsigned fillWallet(char * name, double fiatMoney,unsigned id, Wallet & w)
{
	w.fiatMoney = fiatMoney;
	strcpy_s(w.name, 255, name);
	w.id = id;
	id++;
	
	return id;
}
void writeWallet(const Wallet &w)
{
	ofstream ofs;
	ofs.open(FILE_NAME_WALLETS, ios::binary | ios::out | ios::app);
	ofs.write((char*)&w, sizeof(Wallet));
	ofs.close();
}

int loadWallets(Wallet *& wallets)
{
	ifstream myFile;
	myFile.open(FILE_NAME_WALLETS, ios::binary | ios::in);	
	unsigned i = 0;
	unsigned nameLen = 0;


	if (myFile.is_open())
	{//not good
		myFile.seekg(0, ios::end);
		int size = myFile.tellg();
		myFile.seekg(0, ios::beg);
		delete[] wallets;
		wallets = new Wallet[size / sizeof(Wallet)];

		int help=0;
		char padding[5];
		if (!size)return 0;
		while (size>help)
		{			
			myFile.read((char*)&wallets[i].fiatMoney, sizeof(wallets[i].fiatMoney));
			myFile.read((char*)&wallets[i].id, sizeof(wallets[i].id));
			myFile.read(wallets[i].name, 255);
			myFile.read(padding, 5);
			//myFile.read((char*)&wallets[i], sizeof(wallets[i]));
			help = myFile.tellg();
			i++;
		}
	}
	myFile.close();
	return wallets[i-1].id+1;//next id
}
int addWallet(char * name, double fiatMoney, unsigned id, Wallet *& wallets)
{
	Wallet w;
	fillWallet(name, fiatMoney, id, w);
	writeWallet(w);
	Transaction t;
	fillTransaction(4294967295, id, fiatMoney / 375, t);
	writeTransaction(t);
	return loadWallets(wallets);//next id
}

int findWallet(unsigned id, const Wallet * wallets, unsigned cnt)
{
	for (int i = 0; i < cnt; i++)
		if (wallets[i].id == id) return i;
	return -1;
}
void rewriteWallets(const Wallet * wallets, int firstWallet, int secondWallet)
{//i am sure that the wallet i am looking for is at sizeof(Wallet)*firstWallet+sizeof (double) because ids are getting incremented by 1 every time i write
	//2 wallets are affected by an order, so I rewrite only them, not all the wallets
	//sync between wallets array and wallets file
	int amount = -1;
	int id = -1;
	fstream ofs(FILE_NAME_WALLETS, ios::binary|ios::in|ios::out);
	int offset = sizeof(Wallet)*firstWallet + sizeof(double);
	ofs.seekg(offset,ios::beg);
	ofs.read((char*)&id, sizeof(id));
	if (firstWallet == id)
	{
		ofs.seekp(sizeof(Wallet)*firstWallet, ios::beg);
		ofs.write((char*)&wallets[firstWallet].fiatMoney, sizeof(double)); 
	}
	offset = sizeof(Wallet)*secondWallet + sizeof(double);
	ofs.seekg(offset, ios::beg);
	ofs.read((char*)&id, sizeof(id));
	if (secondWallet == id)
	{
		ofs.seekp(sizeof(Wallet)*secondWallet, ios::beg);
		ofs.write((char*)&wallets[secondWallet].fiatMoney, sizeof(double));
	}
	ofs.close();

	haveToUpdateWalletsArray = true;
}
//transaction functions
void fillTransaction(unsigned senderID, unsigned recieverID, double FMIcoins, Transaction & t)
{
	t.fmiCoins = FMIcoins;
	t.receiverId = recieverID;
	t.senderId = senderID;
	t.time = time(nullptr);
}
void writeTransaction(const Transaction & t)
{
	ofstream ofs(FILE_NAME_TRANS, ios::binary | ios::app);
	ofs.write((char*)&t, sizeof(t));
	ofs.close();
	haveToUpdateTransactionsArray = true;
}
void writeTransactionToTxt(const Transaction & t,const char* fileName)
{
	ofstream ofs(fileName, ios::out | ios::app);
	ofs << t.receiverId << ", " << t.senderId<<", " << t.fmiCoins<< endl;
	ofs.close();
}
int loadTran(Transaction *& transactions)
{	
	
	int size = fileSize(FILE_NAME_TRANS);
	ifstream ifs(FILE_NAME_TRANS, ios::binary);
	
	int transCnt = size / sizeof(Transaction);
	//we will load transactions if only we have written something new
	if (haveToUpdateTransactionsArray == false) {
		ifs.close(); return transCnt;
	}

	if (!transCnt)return 0;
	delete[] transactions;
	transactions = new Transaction[transCnt];
	for (int i = 0; i < transCnt ; i++)
	{
		ifs.read((char*)&transactions[i].time, sizeof(transactions[i].time));
		ifs.read((char*)&transactions[i].senderId, sizeof(transactions[i].senderId));
		ifs.read((char*)&transactions[i].receiverId, sizeof(transactions[i].receiverId));
		ifs.read((char*)&transactions[i].fmiCoins, sizeof(transactions[i].fmiCoins));
	}
	ifs.close();
	haveToUpdateTransactionsArray = false;
	return transCnt;
}

//oreder functioins
bool fillOrder(const char* type, unsigned walletId, double fmiCoins, Order & order)
{
	order.fmiCoins = fmiCoins;
	order.walletId = walletId;
	if (!strcmp(type, "SELL")) {
		order.type=Order::Type::SELL; return true;
	}
	if (!strcmp(type, "BUY")) { order.type=Order::Type::BUY;
	return true; }
	return false;
}

bool writeOrder(const Order & order)
{
	ofstream ofs(FILE_NAME_ORDERS, ios::binary | ios::out | ios::app);
	if (ofs)
	ofs.write((char*)&order, sizeof(order));
	ofs.close();
	return true;
}
void loadOrders(OrdersArray & arr)
{
	int size = fileSize(FILE_NAME_ORDERS);
	arr.size = size / sizeof(Order);
	if (!arr.size) return;
	delete[] arr.orders;
	arr.orders = new Order[arr.size];
	ifstream ifs(FILE_NAME_ORDERS, ios::binary);
	for (int i = 0; i < arr.size; i++)
	{
		ifs.read((char*)&arr.orders[i].type, sizeof(arr.orders[i].type));
		ifs.read((char*)&arr.orders[i].walletId, sizeof(arr.orders[i].walletId));
		ifs.read((char*)&arr.orders[i].fmiCoins, sizeof(arr.orders[i].fmiCoins));
	}
	ifs.close();
}
bool writeOrdersArray(const OrdersArray &arr)//writing only non-zero orders
{

	ofstream trunc(FILE_NAME_ORDERS, ios::binary | ios::trunc);
	if (!trunc)return false;
	for (int i = 0; i < arr.size; i++)
	{
		if (arr.orders[i].fmiCoins!=0)
			trunc.write((char*)&arr.orders[i], sizeof(Order));
	}
	trunc.close();
	return true;
}
void addToArray(const Order & o, OrdersArray & arr)
{

	if (arr.size == 0){
		arr.orders = new Order[1];
		arr.orders[0].type = o.type;
		arr.orders[0].fmiCoins = o.fmiCoins;
		arr.orders[0].walletId = o.walletId;
		arr.size = 1;
		return;
	}
	Order * help = new Order[arr.size];
	for (int i = 0; i < arr.size; i++)
		help[i] = arr.orders[i];
	
	delete[] arr.orders;
	arr.orders = new Order[arr.size + 1];
	for (int i = 0; i < arr.size; i++)
	{
		arr.orders[i] = help[i];
	}
	arr.orders[arr.size].fmiCoins = o.fmiCoins;
	arr.orders[arr.size].type = o.type;
	arr.orders[arr.size].walletId = o.walletId;
	arr.size++;
}
bool zeroOrder(OrdersArray & arr, int i)
{
	if (i>arr.size - 1)return false;
		arr.orders[i].fmiCoins = 0;
	return true;
}

void makeFileName(unsigned id, char fileName[])
{
	_itoa_s(id, fileName,10, 10);
	int idDigitsCnt = 0;
	while (id / 10)
	{idDigitsCnt++;
	id /= 10;
	}
		
	_itoa_s(time(nullptr), fileName + idDigitsCnt + 1 ,14, 10);
}

bool checkOrder(Order & order, Wallet *& wallets, Transaction *& trans, OrdersArray & arr)//transactions should be updated before calling this function
{	
	loadTran(trans);
	loadOrders(arr);
	int walletsCnt = fileSize(FILE_NAME_WALLETS) / sizeof(Wallet);

	int walletIndx = findWallet(order.walletId, wallets, walletsCnt);
	if (walletIndx == -1) 
		return false; // no such wallet
	
	if (order.type==0){
	int transactionCnt = fileSize(FILE_NAME_TRANS) / sizeof(Transaction);

	char fileName[128];
	makeFileName(order.walletId, fileName);
	
	int transAffected = 0;
		//sell order check
		double fmiCoinsWalletID = 0;
		double orderedCoins = order.fmiCoins;
		for (int i = 0; i < transactionCnt; i++)
		{
			if (trans[i].receiverId == wallets[walletIndx].id)	fmiCoinsWalletID += trans[i].fmiCoins;//count received coins
			if (trans[i].senderId == wallets[walletIndx].id)	fmiCoinsWalletID -= trans[i].fmiCoins;//count sent coins
		}
		if (fmiCoinsWalletID < orderedCoins) 
			return false;//not enough fmicoins in walletid

		//check if there are buy orders to execute this sell order 
		bool flag = false;
		for (int i = 0; i < arr.size; i++)
		{
			if (arr.orders[i].type == 1 && arr.orders[i].fmiCoins != 0 && orderedCoins != 0)//buy
			{
				if (orderedCoins - arr.orders[i].fmiCoins < 0) {
					//make last transaction here
					Transaction t; transAffected++;
					fillTransaction(order.walletId, arr.orders[i].walletId, orderedCoins,t);
					writeTransaction(t);
					//txt writing
									
					writeTransactionToTxt(t, fileName);


					//make order with what is left 
						Order o;
						fillOrder("BUY", arr.orders[i].walletId, arr.orders[i].fmiCoins - orderedCoins, o);
						addToArray(o, arr);
						//change fiatMoney in receiver's and sender's wallets
						wallets[findWallet(order.walletId, wallets, walletsCnt)].fiatMoney -= arr.orders[i].fmiCoins * 375;
						wallets[findWallet(arr.orders[i].walletId, wallets, walletsCnt)].fiatMoney += arr.orders[i].fmiCoins * 375;
						rewriteWallets(wallets, order.walletId, arr.orders[i].walletId);
						
						//delete last order
						arr.orders[i].fmiCoins = 0;

						flag = true;//to know we have stopped here
					break; 
				}

				orderedCoins -= arr.orders[i].fmiCoins;
				//create transaction
				Transaction t; transAffected++;
				fillTransaction(order.walletId, arr.orders[i].walletId, arr.orders[i].fmiCoins, t);
				writeTransaction(t);
				writeTransactionToTxt(t, fileName);

				//change fiatMoney in receiver's and sender's wallets
				wallets[findWallet(order.walletId, wallets, walletsCnt)].fiatMoney -= arr.orders[i].fmiCoins * 375;
				wallets[findWallet(arr.orders[i].walletId, wallets, walletsCnt)].fiatMoney += arr.orders[i].fmiCoins * 375;

				//delete i order from orders
				arr.orders[i].fmiCoins = 0;

			}

			
		}
		
		if (!flag)
		{
			Order o;
			fillOrder("SELL", order.walletId, orderedCoins, o);
			addToArray(o, arr);

		}
		else
		{
			ofstream ofs(fileName, ios::out | ios::app);
			ofs << transAffected;
			ofs.close();
		}

	
		
	}
	
	if (order.type==1)
	{
		char fileName[128];
		makeFileName(order.walletId, fileName);
		int transAffected = 0;
		double orderedCoins = order.fmiCoins;
		bool flag = false;

		if (wallets[walletIndx].fiatMoney < orderedCoins * 375)return false; //not enough fiatMoney
		//buy order check
		for (int z = 0; z < arr.size; z++)	
		{
			if (arr.orders[z].type == 0 && arr.orders[z].fmiCoins != 0 && orderedCoins!=0)// check for sell orders
			{
				if (orderedCoins - arr.orders[z].fmiCoins < 0 ) {
					//make last transaction here
					Transaction t; transAffected++;
					fillTransaction(arr.orders[z].walletId,order.walletId,  orderedCoins, t);
					writeTransaction(t);
					//txt
					
					writeTransactionToTxt(t,fileName);

					rewriteWallets(wallets, order.walletId, arr.orders[z].walletId);//this function is used to sync file and array

					//make order with what is left 
					Order o;
					fillOrder("SELL", arr.orders[z].walletId, arr.orders[z].fmiCoins - orderedCoins, o);					
					addToArray(o, arr);
					//change fiatMoney in receiver's and sender's wallets
					wallets[findWallet(order.walletId, wallets, walletsCnt)].fiatMoney += arr.orders[z].fmiCoins * 375;
					wallets[findWallet(arr.orders[z].walletId, wallets, walletsCnt)].fiatMoney -= arr.orders[z].fmiCoins * 375;
					//delete last order
					arr.orders[z].fmiCoins = 0;
					flag = true;//to know we have stopped here
					
					break;
				}

				orderedCoins -= arr.orders[z].fmiCoins;
				//create transaction
				Transaction t; transAffected++;
				fillTransaction(arr.orders[z].walletId,order.walletId, arr.orders[z].fmiCoins, t);
				writeTransaction(t);
				
				
				writeTransactionToTxt(t, fileName);
				//change fiatMoney in receiver's and sender's wallets
				wallets[findWallet(order.walletId, wallets, walletsCnt)].fiatMoney += arr.orders[z].fmiCoins * 375;
				wallets[findWallet(arr.orders[z].walletId, wallets, walletsCnt)].fiatMoney -= arr.orders[z].fmiCoins * 375;
				//delete i order from orders
				arr.orders[z].fmiCoins = 0;
				
			}
			//
		}
		if (!flag)
		{
			Order o;
			fillOrder("BUY", order.walletId, orderedCoins, o);
			addToArray(o, arr);
		}

		ofstream ofs(fileName, ios::out | ios::app);
		ofs << transAffected;
		ofs.close();
	}
	//rewrite wallets!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	
	writeOrdersArray(arr);//write only non-zero => executed will not be written
	loadOrders(arr);//sync orders array and orders file
	loadTran(trans);//update transactions after writing to file
	return true;
}


int input(Wallet *& wallets, OrdersArray& arr, Transaction *& trans, unsigned * lastId)
{
	char buffer[300] = {""};
	char *doubleBuffer = nullptr;

	int cntWallets = fileSize(FILE_NAME_WALLETS) / sizeof(Wallet);
	int cntTrans = fileSize(FILE_NAME_TRANS) / sizeof(Transaction);

	
	while (strcmp(buffer, "quit"))
	{
		
		doubleBuffer = new char[32];
		cin.getline(buffer, 300);

		if (buffer[0] == 'a' && buffer[1] == 'd' && buffer[2] == 'd' && buffer[3] == '-' && buffer[4] == 'w' && buffer[5] == 'a' && buffer[6] == 'l' && buffer[7] == 'l' && buffer[8] == 'e'&&buffer[9] == 't' &&buffer[10] == ' ')
		{
			int i = 11;
			while (buffer[i] != ' ')
			{
				doubleBuffer[i - 11] = buffer[i];
				i++;
			}

			*lastId = addWallet(buffer + i + 1, atof(doubleBuffer), *lastId, wallets);

		}
		
		if (buffer[0] == 'm' && buffer[1] == 'a' && buffer[2] == 'k' && buffer[3] == 'e' && buffer[4] == '-' && buffer[5] == 'o' && buffer[6] == 'r' && buffer[7] == 'd' && buffer[8] == 'e'&&buffer[9] == 'r' &&buffer[10] == ' ')
		{

			char oType[5];
			int offset = 11;
			if (buffer[11] == 'S' && buffer[12] == 'E' && buffer[13] == 'L' && buffer[14] == 'L' && buffer[15] == ' ') {
				strcpy_s(oType, 5, "SELL"); offset += 5;
			}
			if (buffer[11] == 'B' && buffer[12] == 'U' && buffer[13] == 'Y'&& buffer[14] == ' '){
				strcpy_s(oType, 4, "BUY"); offset += 4;
			}
			int i = 0;
			while (buffer[offset] != ' ')
			{
				doubleBuffer[i] = buffer[offset];
				i++;
				offset++;
			}
			Order o;
			int a = atoi(buffer + offset + 1);
			double b = atof(doubleBuffer);
			fillOrder(oType, a, b, o);
			if (!checkOrder(o, wallets, trans, arr))cout << "no such id or not enough money";

		}
		if (buffer[0] == 'a' && buffer[1] == 't' && buffer[2] == 't' && buffer[3] == 'r' && buffer[4] == 'a' && buffer[5] == 'c' && buffer[6] == 't' && buffer[7] == '-' && buffer[8] == 'i' && buffer[9] == 'n' &&buffer[10] == 'v'
			&& buffer[11] == 'e' && buffer[12] == 's' && buffer[13] == 't'&& buffer[14] == 'o' && buffer[15] == 'r' && buffer[16] == 's'&& buffer[17] == '\0')
		{
					
					cntWallets = fileSize(FILE_NAME_WALLETS) / sizeof(Wallet);
					cntTrans=loadTran(trans);
					showFirstTen(wallets, cntWallets, trans, cntTrans);
		}
		if (buffer[0] == 'w' && buffer[1] == 'a' && buffer[2] == 'l' && buffer[3] == 'l' && buffer[4] == 'e' && buffer[5] == 't' && buffer[6] == '-' && buffer[7] == 'i' && buffer[8] == 'n' &&buffer[9] == 'f' && buffer[10] == 'o' && buffer[11] == ' ')
			{
				int id = atoi(buffer + 12);
				walletInfo(wallets, cntWallets, id, trans, cntTrans);
			}

		
		delete[] doubleBuffer;
		
	}
	return true;
}