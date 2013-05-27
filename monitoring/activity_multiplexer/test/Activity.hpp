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

	// TODO: add timestamp
	// TODO: name: z.B. A1(open) A2(doA) A3(close) A(open) A(doB) A(close) 

	/* data */
};


#endif /* ACTIVITY_H */
