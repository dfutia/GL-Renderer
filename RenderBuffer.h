#ifndef RENDERBUFFER_H
#define RENDERBUFFER_H

class Renderbuffer
{
public:
	Renderbuffer();
	Renderbuffer(const Renderbuffer& other);
	Renderbuffer();

	~Renderbuffer();

	const Renderbuffer& operator=(const Renderbuffer& other);

	void Storage();
private:
	unsigned int id;
};

#endif