#ifndef ACTIVITY_H
#define ACTIVITY_H 


class Activity
{
public:
	Activity ();
	virtual ~Activity ();
	void print();

private:
	int ID;
	static int nextID;

	/* data */
};


#endif /* ACTIVITY_H */
