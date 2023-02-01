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

	auto start = std::chrono::high_resolution_clock::now();
	for (int i = 1; i < argc; i++)
	{
		std::string file = argv[i];
		std::string ext = file.substr(file.find_last_of(".") + 1);

		bool decrypt = ext.compare("set") == 0;
		bool encrypt = ext.compare("txt") == 0;

		if (!decrypt && !encrypt)
		{
			std::cout << "File " << file << " is not supported!" << std::endl
					  << "Only .set and .txt files!";
			continue;
		}

		std::ifstream input(file, std::ios::in | std::ios::binary);
		if (!input.is_open())
		{
			std::cout << "Failed opening input file " << file << "\n";
			continue;
		}

		input.seekg(0, std::ios::end);

		int32_t size = input.tellg();
		int32_t tmpSize;

		input.seekg(0, std::ios::beg);

		char *bufIn = new char[size];

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
			std::cout << "Failed opening output file" << outName.str() << "\n";
			continue;
		}

		if (decrypt)
		{
			tmpSize = size - 1;
			int nullCount = 16 - (tmpSize % 16);
			tmpSize += nullCount;

			char *bufInToDecrypt = new char[tmpSize];
			std::copy(bufIn + 1, bufIn + size, bufInToDecrypt);

			DecodeBuffer(bufInToDecrypt, bufInToDecrypt, tmpSize);

			int32_t outputSize = tmpSize - nullCount - bufIn[0];
			output.write(bufInToDecrypt, outputSize);

			delete[] bufInToDecrypt;
		}
		else if (encrypt)
		{
			int nullCount = 16 - (size % 16);
			tmpSize = size + nullCount;

			int32_t outputSize = tmpSize + 1;

			char *bufInToEncrypt = new char[outputSize];
			std::copy(bufIn, bufIn + size, bufInToEncrypt);

			EncodeBuffer(bufInToEncrypt + 1, bufInToEncrypt + 1, tmpSize);

			bufInToEncrypt[0] = nullCount;
			output.write(bufInToEncrypt, outputSize);

			delete[] bufInToEncrypt;
		}

		delete[] bufIn;
		output.close();
	}
	if (argc == 1)
	{
		std::cout << "Please specify a file!\n";
	}

	auto end = std::chrono::high_resolution_clock::now();
	auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);

	std::cout << "Finished in " << duration.count() << "ms\n";
	std::cout << "Press any key to close\n";
	std::cin.get();
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