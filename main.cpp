#include<iostream>
#include "funcAndStruct.h"
using namespace std;


int main()
{
	int cntWallets = fileSize(FILE_NAME_WALLETS)/sizeof(Wallet);
	//int cntOrders = fileSize(FILE_NAME_ORDERS)/sizeof(Order);
	int cntTrans = fileSize(FILE_NAME_TRANS)/sizeof(Transaction);
	unsigned lastID = 0;
	Wallet* wallets = nullptr;
	OrdersArray arr;
	arr.orders = nullptr;
	arr.size = 0;
	Transaction * trans = nullptr;
	// 
	if (cntWallets!=0)
	lastID=loadWallets(wallets); haveToUpdateWalletsArray = false; haveToUpdateWalletsFile = false;
	if (cntTrans!=0)
	loadTran(trans); haveToUpdateTransactionsArray = false; 
	loadOrders(arr); 
	//
	input(wallets, arr, trans, &lastID);
	return 0;
}

//TODO :
//rewrite wallets after orderCheck() 1 hour -done/not tested
//attractInvestors 2 hours done 
//walletInfo 0.5 hours done 
//write transactions to TXT 1 hour done 

//input 40mins
//logic with updating 1.5 hours
//test 3 hours

//SUM = 8 hours
//thursday 2 hours
// friday 2 hours
//saturday 4 hours