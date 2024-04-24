#pragma once

class RightValue
{
public:
	RightValue();
	~RightValue();

	void Display_RValue();
private:
	//test01
	void test01();
	int g_var = 8;
	int& returnALvalue() {
		return g_var; //here we return a left value
	}
	int returnARvalue() {
		return g_var; //here we return a r-value
	}

	//test02
	void test02();
	void print(const std::string& name) {
		printf("rvalue detected:%s\n", name.c_str());
	}
	void print(std::string& name) {
		printf("lvalue detected:%s\n", name.c_str());
	}

	//test03
	void test03();
	void print3(const std::string& name) {
		printf("const value detected : % s\n", name.c_str());
	}
	void print3(std::string& name) {
		printf("lvalue detected % s\n", name.c_str());
	}
	void print3(std::string&& name) {
		printf("rvalue detected : % s\n", name.c_str());
	}

	//test04
	void test04();
	RightValue(const char res[]) {
		theResource = new std::string(res);
		printf("init %s\n", theResource->c_str());
	}
	RightValue(const RightValue& other) {
		printf("copy %s\n", other.theResource->c_str());
		theResource = new std::string(other.theResource->c_str());
	}
	RightValue& operator=(const RightValue& other) {
		RightValue tmp(other);//¿½±´
		swap(theResource, tmp.theResource);//½»»»
		printf("assign %s\n", other.theResource->c_str());
		return *this;
	}
	RightValue& operator=(RightValue&& other) {
		printf("move assign %s\n", other.theResource->c_str());
		theResource = std::move(other.theResource);
		other.theResource = nullptr;
		return *this;
	}
	std::string * theResource;
};

RightValue::RightValue()
{
}

RightValue::~RightValue()
{
	if (theResource) {
		printf("destructor %s\n", theResource->c_str());
		delete theResource;
	}
}

inline void RightValue::Display_RValue()
{
	//test01();
	//test02();
	//test03();
	test04();
}

inline void RightValue::test01()
{
	printf("%d\r\n", returnALvalue()++); // g_var += 1;
	printf("%d\r\n", returnARvalue());
}

inline void RightValue::test02()
{
	std::string name = "lvalue";
	print(name); //compiler can detect the right function for lvalue
	print("rvalue"); // likewise for rvalue
}

inline void RightValue::test03()
{
	std::string name = "lvalue";
	const std::string cname = "cvalue";

	print3(name);
	print3(cname);
	print3("rvalue");
}

inline void RightValue::test04()
{
	// case 1
	printf("=====start testCopy()=====\n");
	RightValue res1("res1");
	RightValue res2 = res1;
	//copy res1
	printf("=====destructors for stack vars, ignore=====\n");
	printf("\n");

	// case 2
	printf("=====start testAssign()=====\n");
	RightValue res3("res3");
	RightValue res4("res4");
	res4 = res3;
	//copy res3, assign res3, destrctor res4
	printf("=====destructors for stack vars, ignore=====\n");
	printf("\n");

	// case 3
	printf("=====start testRValue()=====\n");
	RightValue res6("res6");
	res6 = RightValue("res5");
	//copy res5, assign res5, destructor res6, destructor res5
	printf("=====destructors for stack vars, ignore=====\n");
	printf("\n");

	//case 4
	printf("=====start move testRValue()=====\n");
	RightValue&& res8("res8");
	res8 = std::move("res7");
	//move assign res7
	printf("=====destructors for stack vars, ignore=====\n");
}
