class Point {

	public:
	
		float x, y;

	Point();
	~Point();

	void set(float x, float y);
	bool operator ==(Point point);
	bool operator !=(Point point);
	void operator =(Point point);
	void operator +=(Point point);
	void operator -=(Point point);
	Point operator *(float f);
	
	float getDistance(Point *point);
	
	char *toString();
	void set(const char *in);

};
