#ifndef GUID_H
#define GUID_H

typedef struct guid_t
{
private:
	unsigned long value;

public:
	guid_t(unsigned long v = 0) : value(v) {}

	static guid_t GetNewGUID()
	{
		static unsigned long s_value = 0;
		return guid_t(++s_value);
	}

	operator unsigned long() const
	{
		return value;
	}
} guid_t;

#endif /* GUID_H */