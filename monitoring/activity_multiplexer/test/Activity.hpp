#ifndef ACTIVITY_H
#define ACTIVITY_H 

class Activity
{
public:
	Activity ();
	virtual ~Activity ();
	void print();

private:
	static int nextAID;
	int AID;
	/* data */
};


#endif /* ACTIVITY_H */
