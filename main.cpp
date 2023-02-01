#include <cstdio>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <chrono>
#include "Rijndael.h"

void DecodeBuffer(char *buffer, char *output, unsigned int size);
void EncodeBuffer(char *buffer, char *output, unsigned int size);

int main(int argc, char **argv)
{

	if (argc == 2)
	{

		auto start = std::chrono::high_resolution_clock::now();

		std::string file = argv[1];
		std::string ext = file.substr(file.find_last_of(".") + 1);

		bool decrypt = ext.compare("set") == 0;
		bool encrypt = ext.compare("txt") == 0;

		if (!decrypt && !encrypt)
		{
			std::cout << "File is not supported!" << std::endl
					  << "Only .set and .txt files!";
			std::cin.get();
			std::exit(1);
		}

		std::ifstream input(file, std::ios::in | std::ios::binary);
		if (!input.is_open())
		{
			std::cout << "Failed opening input file\n";
			std::cin.get();
			std::exit(1);
		}

		input.seekg(0, std::ios::end);

		int32_t size = input.tellg();
		int32_t tmpSize = size - 1;

		input.seekg(0, std::ios::beg);

		char *bufIn = new char[size];
		char *data = new char[size];

		input.read(bufIn, size);
		input.close();

		std::ostringstream outName;
		if (decrypt)
		{
			outName << file << ".txt";
		}
		else if (encrypt)
		{
			outName << file << ".set";
		}

		std::ofstream output(outName.str(), std::ios::out | std::ios::binary);
		if (!output.is_open())
		{
			std::cout << "Failed opening output file\n";
			std::cin.get();
			std::exit(1);
		}

		output.write(bufIn, 1);

		std::copy(bufIn + 1, bufIn + size, bufIn);

		if (decrypt)
		{
			DecodeBuffer(bufIn, data, tmpSize);
		}
		else if (encrypt)
		{
			EncodeBuffer(bufIn, data, tmpSize);
		}

		output.write(data, tmpSize);

		delete[] bufIn;
		delete[] data;
		output.close();

		auto end = std::chrono::high_resolution_clock::now();
		auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);

		std::cout << "Finished in " << duration.count() << "ms\n";
		std::cout << "Press any key to close\n";
		std::cin.get();
	}

	return 0;
}

void DecodeBuffer(char *buffer, char *output, unsigned int size)
{

	CRijndael rijndael;
	rijndael.MakeKey("TIMOTEI_ZION", CRijndael::sm_chain0, 16, 16);
	rijndael.Decrypt(buffer, output, size);
	rijndael.ResetChain();
}

void EncodeBuffer(char *buffer, char *output, unsigned int size)
{

	CRijndael rijndael;
	rijndael.MakeKey("TIMOTEI_ZION", CRijndael::sm_chain0, 16, 16);
	rijndael.Encrypt(buffer, output, size);
	rijndael.ResetChain();
}