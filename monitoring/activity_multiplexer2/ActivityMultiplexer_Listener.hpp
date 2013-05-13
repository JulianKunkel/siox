//@author Jakob Luettgau
//TODO Code Documentation - We will set up automatic recursive doc creation
#ifndef ACTIVITYMULTIPLEXER_LISTENER_H
#define ACTIVITYMULTIPLEXER_LISTENER_H 

class Activity;
class ActivityMultiplexer;


class ActivityMultiplexer_Listener
{
public:
	ActivityMultiplexer_Listener ();
	virtual ~ActivityMultiplexer_Listener ();

	virtual void Notify(Activity * activity);
	virtual void Reset(int activities_dropped);

private:
	/* data */
};


#endif /* ACTIVITYMULTIPLEXER_LISTENER_H */
