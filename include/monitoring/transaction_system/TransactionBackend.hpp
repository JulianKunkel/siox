#ifndef TRANSACTION_BACK_END_H
#define TRANSACTION_BACK_END_H

class TransactionBackend {
public:
	virtual Callback *create_callback() = 0;
	virtual void init(const std::string &) = 0;
};

#endif
