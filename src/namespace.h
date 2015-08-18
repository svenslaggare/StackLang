#pragma once
#include <vector>
#include <string>

//Represents a namespace
class Namespace {
private:
	std::vector<std::string> mParts;
	std::string mName;
	std::string mVMName;

	std::string getName(std::string sep);
public:
	//Creates a global namespace
	Namespace();

	//Creates a new namespace using the given parts
	Namespace(std::vector<std::string> parts);

	//Returns the parts in the namespace name
	const std::vector<std::string>& parts() const;

	//Returns the name of namespace
	std::string name() const;

	//Returns the name of the namespace in the VM
	std::string vmName() const;
};