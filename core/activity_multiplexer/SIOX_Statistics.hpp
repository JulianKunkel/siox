// @author Jakob Lüttgau

#ifndef SIOX_STATISTICS_H
#define SIOX_STATISTICS_H 

/**
 * 
 *
 */
class SIOX_Statistics
{
public:
	virtual ~SIOX_Statistics ();

	/**
	 * add value to key
	 *
	 * @param int key
	 * @param T value
	 */
	virtual void addValue(int key, int   value);	
	virtual void addValue(int key, float value);	
	virtual void addValue(int key, bool  value);	
	virtual void addValue(int key, char* value);	

	/**
	 * get value of key
	 *
	 * @param int key
	 * @param int index of value
	 */
	virtual int   getValue(int key, int index);
	virtual float getValue(int key, int index);
	virtual bool  getValue(int key, int index);
	virtual char* getValue(int key, int index);

	/**
	 * list all available values of key
	 */
	virtual void listValues(int key);

	/** 
	 * print statistics like: "key", value
	 */
	virtual void shutdown();

private:
	/* data */
};

#endif /* SIOX_STATISTICS_H */
