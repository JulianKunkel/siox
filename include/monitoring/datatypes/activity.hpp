#infdef SIOX_ACTIVITY_H
#define SIOX_ACTIVITY_H

namespace monitoring {
    

enum ActivityStatus {
    empty,
    initialized,
    started,
    stopped,
    finished
}


class Activity
{
private:
    long aid;
    long paid;
    long unid;
    long time_start;
    long time_stop;
    int status;
    char[] comment;

    //List<Attribute> attributes;

public:
    Activity(void);
    Activity(Component component, Timestamp t_start = NULL, char[] comment="");
    ~Activity(void);

    void start(Timestamp t=NULL);
    void stop(Timestamp t=NULL);
    void end(void);

};


}
#endif // SIOX_ACTIVITY_H