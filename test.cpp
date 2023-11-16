#include <iostream>
class A
{
	public:
		void setIdea(int n, std::string idea);
		std::string getIdea(int n);
};

int main()
{
	A a;
	a.setIdea(3, "DFgghfjhg");
	{
		A b = a;
		std::cout << b.getIdea(3) << "\n";
	}
	std::cout << a.getIdea(3) << "\n";
}