#ifndef __TRANSACTIONENGINE__
#define __TRANSACTIONENGINE__

#include "Ledger.h"
#include "SerializedTransaction.h"
#include "SerializedLedger.h"

// A TransactionEngine applies serialized transactions to a ledger
// It can also, verify signatures, verify fees, and give rejection reasons

enum TransactionEngineResult
{
	// Note: Numbers are currently unstable.  Use tokens.

	// tenCAN_NEVER_SUCCEED = <0

	// Malformed: Fee claimed
	tenGEN_IN_USE	= -300,
	tenBAD_ADD_AUTH,
	tenBAD_AMOUNT,
	tenBAD_CLAIM_ID,
	tenBAD_GEN_AUTH,
	tenBAD_SET_ID,
	tenCREATEXNS,
	tenDST_IS_SRC,
	tenDST_NEEDED,
	tenEXPLICITXNS,
	tenREDUNDANT,
	tenRIPPLE_EMPTY,

	// Invalid: Ledger won't allow.
	tenCLAIMED		= -200,
	tenBAD_AUTH_MASTER,
	tenBAD_RIPPLE,
	tenCREATED,
	tenMSG_SET,
	terALREADY,

	// Other
	tenFAILED		= -100,
	tenINSUF_FEE_P,
	tenINVALID,
	tenUNKNOWN,

	terSUCCESS		= 0,

	// terFAILED_BUT_COULD_SUCCEED = >0
	// Conflict with ledger database: Fee claimed
	// Might succeed if not conflict is not caused by transaction ordering.
	terBAD_AUTH,
	terBAD_RIPPLE,
	terBAD_SEQ,
	terCREATED,
	terDIR_FULL,
	terFUNDS_SPENT,
	terINSUF_FEE_B,
	terINSUF_FEE_T,
	terNODE_NOT_FOUND,
	terNODE_NOT_MENTIONED,
	terNODE_NO_ROOT,
	terNO_ACCOUNT,
	terNO_DST,
	terNO_LINE_NO_ZERO,
	terNO_PATH,
	terOVER_LIMIT,
	terPAST_LEDGER,
	terPAST_SEQ,
	terPRE_SEQ,
	terSET_MISSING_DST,
	terUNCLAIMED,
	terUNFUNDED,
};

bool transResultInfo(TransactionEngineResult terCode, std::string& strToken, std::string& strHuman);

enum TransactionEngineParams
{
	tepNONE          = 0,
	tepNO_CHECK_SIGN = 1,	// Signature already checked
	tepNO_CHECK_FEE  = 2,	// It was voted into a ledger anyway
	tepUPDATE_TOTAL  = 4,	// Update the total coins
};

enum TransactionAccountAction
{
	taaACCESS,
	taaCREATE,
	taaMODIFY,
	taaDELETE
};

typedef std::pair<TransactionAccountAction, SerializedLedgerEntry::pointer> AffectedAccount;

class TransactionEngine
{
private:
	TransactionEngineResult dirAdd(
		std::vector<AffectedAccount>&	accounts,
		uint64&							uNodeDir,		// Node of entry.
		const uint256&					uBase,
		const uint256&					uLedgerIndex);

	TransactionEngineResult dirDelete(
		std::vector<AffectedAccount>&	accounts,
		const uint64&					uNodeDir,		// Node item is mentioned in.
		const uint256&					uBase,			// Key of item.
		const uint256&					uLedgerIndex);	// Item being deleted

	TransactionEngineResult	setAuthorized(const SerializedTransaction& txn, std::vector<AffectedAccount>& accounts, bool bMustSetGenerator);

protected:
	Ledger::pointer mDefaultLedger, mAlternateLedger;
	Ledger::pointer mLedger;

	TransactionEngineResult doAccountSet(const SerializedTransaction& txn, std::vector<AffectedAccount>& accounts);
	TransactionEngineResult doClaim(const SerializedTransaction& txn, std::vector<AffectedAccount>& accounts);
	TransactionEngineResult doCreditSet(const SerializedTransaction& txn, std::vector<AffectedAccount>& accounts,
								const uint160& uSrcAccountID);
	TransactionEngineResult doDelete(const SerializedTransaction& txn, std::vector<AffectedAccount>& accounts);
	TransactionEngineResult doInvoice(const SerializedTransaction& txn, std::vector<AffectedAccount>& accounts);
	TransactionEngineResult doOffer(const SerializedTransaction& txn, std::vector<AffectedAccount>& accounts);
	TransactionEngineResult doNicknameSet(const SerializedTransaction& txn, std::vector<AffectedAccount>& accounts,
								const uint160& uSrcAccountID);
	TransactionEngineResult doPasswordFund(const SerializedTransaction& txn, std::vector<AffectedAccount>& accounts,
								const uint160& uSrcAccountID);
	TransactionEngineResult doPasswordSet(const SerializedTransaction& txn, std::vector<AffectedAccount>& accounts);
	TransactionEngineResult doPayment(const SerializedTransaction& txn, std::vector<AffectedAccount>& accounts,
								const uint160& uSrcAccountID);
	TransactionEngineResult doStore(const SerializedTransaction& txn, std::vector<AffectedAccount>& accounts);
	TransactionEngineResult doTake(const SerializedTransaction& txn, std::vector<AffectedAccount>& accounts);
	TransactionEngineResult doTransitSet(const SerializedTransaction& txn, std::vector<AffectedAccount>& accounts);
	TransactionEngineResult doWalletAdd(const SerializedTransaction& txn, std::vector<AffectedAccount>& accounts);

public:
	TransactionEngine() { ; }
	TransactionEngine(Ledger::pointer ledger) : mDefaultLedger(ledger) { ; }

	Ledger::pointer getDefaultLedger()				{ return mDefaultLedger; }
	void setDefaultLedger(Ledger::pointer ledger)	{ mDefaultLedger = ledger; }
	Ledger::pointer getAlternateLedger()			{ return mAlternateLedger; }
	void setAlternateLedger(Ledger::pointer ledger)	{ mAlternateLedger = ledger; }
	void setLedger(Ledger::pointer ledger)			{ mDefaultLedger = ledger;
													  mAlternateLedger = Ledger::pointer(); }

	Ledger::pointer getTransactionLedger(uint32 targetLedger);
	TransactionEngineResult applyTransaction(const SerializedTransaction&, TransactionEngineParams,
		Ledger::pointer ledger);
	TransactionEngineResult applyTransaction(const SerializedTransaction&, TransactionEngineParams,
		uint32 targetLedger);
};

inline TransactionEngineParams operator|(const TransactionEngineParams& l1, const TransactionEngineParams& l2)
{
	return static_cast<TransactionEngineParams>(static_cast<int>(l1) | static_cast<int>(l2));
}

inline TransactionEngineParams operator&(const TransactionEngineParams& l1, const TransactionEngineParams& l2)
{
	return static_cast<TransactionEngineParams>(static_cast<int>(l1) & static_cast<int>(l2));
}

#endif
// vim:ts=4
